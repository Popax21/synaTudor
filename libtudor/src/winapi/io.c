#include <stdlib.h>
#include "internal.h"

struct winfile_op {
    struct winfile *file;
    void *op_ctx;

    volatile NTSTATUS cb_status;
    winio_overlapped_cb_fnc *cb_fnc;
    void *cb_ctx;
    bool cb_new_thread;
};

struct winfile {
    void *ctx;
    bool is_async;
    winio_read_fnc *read_fnc;
    winio_write_fnc *write_fnc;
    winio_devctrl_fnc *devctrl_fnc;
    winio_cancel_fnc *cancel_fnc;
    winio_cleanup_fnc *cleanup_fnc;
    winio_destroy_fnc *destroy_fnc;
};

static inline bool init_overlapped(OVERLAPPED *ovlp, struct winfile *file, void ***op_ctx) {
    struct winfile_op *op = (struct winfile_op*) malloc(sizeof(struct winfile_op));
    if(!op) { winerr_set_errno(); return false; }
    *op = (struct winfile_op) {
        .file = file,
        .op_ctx = NULL,

        .cb_status = STATUS_PENDING,
        .cb_fnc = NULL,
        .cb_ctx = NULL
    };
    *op_ctx = &op->op_ctx;

    ovlp->Internal = STATUS_PENDING;
    ovlp->InternalHigh = 0;
    ovlp->Pointer = op;

    if(!ovlp->hEvent) ovlp->hEvent = win_create_event(NULL, false, false);
    if(!ovlp->hEvent) { free(op); return false; }
    win_reset_event(ovlp->hEvent);

    return true;
}

//The following code was written at 23:56, please ignore
//<< START CODE >>

struct ovlp_cb {
    OVERLAPPED *ovlp;
    NTSTATUS status;
    winio_overlapped_cb_fnc *cb;
    void *ctx;
};

static void *ovlp_cb_thread_func(struct ovlp_cb *cb) {
    cb->cb(cb->ovlp, cb->status, cb->ctx);
    free(cb);
    return NULL;
}

static inline void call_overlapped_cb(OVERLAPPED *ovlp, NTSTATUS status, winio_overlapped_cb_fnc *cb, void *ctx, bool new_thread) {
    if(!new_thread) {
        cb(ovlp, status, ctx);
        return;
    }

    struct ovlp_cb *c = (struct ovlp_cb*) malloc(sizeof(struct ovlp_cb));
    if(!c) { perror("Couldn't allocate OVERLAPPED callback"); abort(); }
    *c = (struct ovlp_cb) { .ovlp = ovlp, .status = status, .cb = cb, .ctx = ctx };

    pthread_t thread;
    cant_fail_ret(pthread_create(&thread, NULL, (void *(*)(void*)) ovlp_cb_thread_func, c));
    cant_fail_ret(pthread_detach(thread));
}

//<< END CODE >>

void winio_set_overlapped_callback(OVERLAPPED *ovlp, winio_overlapped_cb_fnc *cb, void *ctx, bool new_thread) {
    struct winfile_op *op = (struct winfile_op*) ovlp->Pointer;
    if(!op) {
        call_overlapped_cb(ovlp, (NTSTATUS) ovlp->Internal, cb, ctx, new_thread);
        return;
    }

    op->cb_ctx = ctx;
    op->cb_new_thread = new_thread;
    __atomic_store_n(&op->cb_fnc, cb, __ATOMIC_RELEASE);

    NTSTATUS status = __atomic_load_n(&op->cb_status, __ATOMIC_ACQUIRE);
    if(status != STATUS_PENDING) {
        cb = __atomic_exchange_n(&op->cb_fnc, NULL, __ATOMIC_ACQ_REL);
        if(cb) call_overlapped_cb(ovlp, status, cb, ctx, new_thread);
    }
}

void winio_cancel_overlapped(OVERLAPPED *ovlp) {
    if(ovlp->Internal != STATUS_PENDING) {
        winio_cleanup_overlapped(ovlp);
        return;
    }

    struct winfile_op *op = (struct winfile_op*) ovlp->Pointer;
    if(op && op->op_ctx && op->file->cancel_fnc) {
        op->file->cancel_fnc(op->file->ctx, ovlp, op->op_ctx);
    }

    winio_wait_overlapped(ovlp, NULL);
}

void winio_complete_overlapped(OVERLAPPED *ovlp, NTSTATUS status, size_t num_transfered) {
    //Complete overlapped
    ovlp->Internal = status;
    if(ovlp->Internal == STATUS_SUCCESS) ovlp->InternalHigh = num_transfered;
    if(ovlp->hEvent) win_set_event(ovlp->hEvent);

    //Call callback
    struct winfile_op *op = (struct winfile_op*) ovlp->Pointer;
    __atomic_store_n(&op->cb_status, status, __ATOMIC_RELEASE);

    winio_overlapped_cb_fnc *cb = __atomic_exchange_n(&op->cb_fnc, NULL, __ATOMIC_ACQ_REL);
    if(cb) call_overlapped_cb(ovlp, status, cb, op->cb_ctx, op->cb_new_thread);
}

NTSTATUS winio_wait_overlapped(OVERLAPPED *ovlp, size_t *num_transfered) {
    //Wait for the operation to be completed
    if(ovlp->Internal == STATUS_PENDING) win_wait_sync_obj(ovlp->hEvent, INFINITE);

    NTSTATUS status = (NTSTATUS) ovlp->Internal;
    if(status == STATUS_SUCCESS && num_transfered) *num_transfered = ovlp->InternalHigh;
    winio_cleanup_overlapped(ovlp);
    return status;
}

inline void winio_cleanup_overlapped(OVERLAPPED *ovlp) {
    struct winfile_op *op = (struct winfile_op*) ovlp->Pointer;
    if(!op) return;
    ovlp->Pointer = NULL;

    if(op->op_ctx && op->file->cleanup_fnc) op->file->cleanup_fnc(op->file->ctx, ovlp, op->op_ctx);
    free(op);
}

static void file_destr(struct winfile *file) {
    if(file->destroy_fnc) file->destroy_fnc(file->ctx);
    free(file);
}

HANDLE winio_create_file(void *ctx, bool is_async, winio_read_fnc *read_fnc, winio_write_fnc *write_fnc, winio_devctrl_fnc *devctrl_fnc, winio_cancel_fnc *cancel_fnc, winio_cleanup_fnc *cleanup_fnc, winio_destroy_fnc *destroy_fnc) {
    struct winfile *file = (struct winfile*) malloc(sizeof(struct winfile));
    if(!file) { perror("Couldn't allocate file object"); abort(); }
    file->ctx = ctx;
    file->is_async = is_async;
    file->read_fnc = read_fnc;
    file->write_fnc = write_fnc;
    file->devctrl_fnc = devctrl_fnc;
    file->cancel_fnc = cancel_fnc;
    file->cleanup_fnc = cleanup_fnc;
    file->destroy_fnc = destroy_fnc;
    return winhandle_create(file, (winhandle_destr_fnc*) file_destr);
}

void *winio_get_file_context(HANDLE file) { return ((struct winfile*) file->data)->ctx; }

__winfnc BOOL AreFileApisANSI() { return TRUE; }
WINAPI(AreFileApisANSI)

__winfnc BOOL CancelIoEx(HANDLE file, OVERLAPPED *ovlp) {
    if(!ovlp) return TRUE;
    winio_cancel_overlapped(ovlp);
    return TRUE;
}
WINAPI(CancelIoEx)

__winfnc BOOL GetOverlappedResult(HANDLE file, OVERLAPPED *ovlp, DWORD *num_transfered, BOOL wait) {
    if(ovlp->Internal == STATUS_PENDING && !wait) return FALSE;

    size_t sz;
    NTSTATUS status = winio_wait_overlapped(ovlp, &sz);
    if(status != STATUS_SUCCESS) {
        winerr_set_code(status);
        return FALSE;
    }

    *num_transfered = (DWORD) sz;
    return TRUE;
}
WINAPI(GetOverlappedResult)

__winfnc BOOL ReadFile(HANDLE handle, void *buf, DWORD buf_size, DWORD *num_read, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->read_fnc) { winerr_set(); return FALSE; }

    //Setup overlapped
    OVERLAPPED lovlp = {0};
    if(!file->is_async && !ovlp) ovlp = &lovlp;
    if(!ovlp) { winerr_set(); return FALSE; }

    void **op_ctx;
    off_t off = ovlp ? (ovlp->Offset | ((off_t) ovlp->OffsetHigh) << 32) : 0;
    if(!init_overlapped(ovlp, file, &op_ctx)) return FALSE;

    //Start the operation
    NTSTATUS status = file->read_fnc(file->ctx, ovlp, off, buf, buf_size, op_ctx);
    winerr_set_code(status);
    if(status != STATUS_SUCCESS) return FALSE;

    if(file->is_async) {
        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    }

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, num_read, TRUE);
}
WINAPI(ReadFile)

__winfnc BOOL WriteFile(HANDLE handle, const void *buf, DWORD buf_size, DWORD *num_written, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->write_fnc) { winerr_set(); return FALSE; }

    //Setup overlapped
    OVERLAPPED lovlp = {0};
    if(!file->is_async && !ovlp) ovlp = &lovlp;
    if(!ovlp) { winerr_set(); return FALSE; }

    void **op_ctx;
    off_t off = ovlp ? (ovlp->Offset | ((off_t) ovlp->OffsetHigh) << 32) : 0;
    if(!init_overlapped(ovlp, file, &op_ctx)) return FALSE;

    //Start the operation
    NTSTATUS status = file->write_fnc(file->ctx, ovlp, off, buf, buf_size, op_ctx);
    winerr_set_code(status);
    if(status != STATUS_SUCCESS) return FALSE;

    if(file->is_async) {
        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    }

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, num_written, TRUE);
}
WINAPI(WriteFile)

__winfnc BOOL DeviceIoControl(HANDLE handle, DWORD code, const void *in_buf, DWORD in_size, void *out_buf, DWORD out_size, DWORD *out_ret, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->devctrl_fnc) { winerr_set(); return FALSE; }

    //Setup overlapped
    OVERLAPPED lovlp = {0};
    if(!file->is_async && !ovlp) ovlp = &lovlp;
    if(!ovlp) { winerr_set(); return FALSE; }

    void **op_ctx;
    if(!init_overlapped(ovlp, file, &op_ctx)) return FALSE;

    //Start the operation
    NTSTATUS status = file->devctrl_fnc(file->ctx, ovlp, code, in_buf, in_size, out_buf, out_size, op_ctx);
    winerr_set_code(status);
    if(status != STATUS_SUCCESS) return FALSE;

    if(file->is_async) {
        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    }

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, out_ret, TRUE);
}
WINAPI(DeviceIoControl)