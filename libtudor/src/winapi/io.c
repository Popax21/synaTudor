#include <stdlib.h>
#include "internal.h"

struct winfile_op {
    struct winfile *file;
    void *op_ctx;  
};

static inline bool init_overlapped(OVERLAPPED *ovlp, struct winfile *file, void ***op_ctx) {
    struct winfile_op *op = (struct winfile_op*) malloc(sizeof(struct winfile_op));
    if(!op) { winerr_set_errno(); return false; }
    *op = (struct winfile_op) { .file = file, .op_ctx = NULL };
    *op_ctx = &op->op_ctx;

    ovlp->Internal = STATUS_PENDING;
    ovlp->InternalHigh = 0;
    ovlp->Pointer = op;

    if(!ovlp->hEvent) ovlp->hEvent = win_create_event(NULL, false, false);
    if(!ovlp->hEvent) { free(op); return false; }
    win_reset_event(ovlp->hEvent);

    return true;
}

NTSTATUS winio_cancel_overlapped(OVERLAPPED *ovlp) {
    if((NTSTATUS) ovlp->Internal != STATUS_PENDING) return STATUS_SUCCESS;

    struct winfile_op *op = (struct winfile_op*) ovlp->Pointer;
    if(!op) return STATUS_SUCCESS;

    NTSTATUS status = STATUS_SUCCESS;
    if(op->op_ctx && op->file->cancel_fnc) status = op->file->cancel_fnc(op->file->ctx, op->op_ctx);
    free(ovlp->Pointer);
    ovlp->Pointer = NULL;
    return status;
}

void winio_complete_overlapped(OVERLAPPED *ovlp, NTSTATUS status, size_t num_transfered) {
    ovlp->Internal = status;
    if(ovlp->Internal == STATUS_SUCCESS) ovlp->InternalHigh = num_transfered;
    if(ovlp->hEvent) win_set_event(ovlp->hEvent);
}

NTSTATUS winio_wait_overlapped(OVERLAPPED *ovlp, size_t *num_transfered) {
    //Wait for the operation to be completed
    if(ovlp->Internal == STATUS_PENDING) win_wait_sync_obj(ovlp->hEvent, INFINITE);

    NTSTATUS status = (NTSTATUS) ovlp->Internal;
    if(status == STATUS_SUCCESS && num_transfered) *num_transfered = ovlp->InternalHigh;

    free(ovlp->Pointer);
    ovlp->Pointer = NULL;

    return status;
}

struct winfile {
    void *ctx;
    bool is_async;
    winio_read_fnc *read_fnc;
    winio_write_fnc *write_fnc;
    winio_devctrl_fnc *devctrl_fnc;
    winio_cancel_fnc *cancel_fnc;
    winio_destroy_fnc *destroy_fnc;
};

static void file_destr(struct winfile *file) {
    if(file->destroy_fnc) file->destroy_fnc(file->ctx);
    free(file);
}

HANDLE winio_create_file(void *ctx, bool is_async, winio_read_fnc *read_fnc, winio_write_fnc *write_fnc, winio_devctrl_fnc *devctrl_fnc, winio_destroy_fnc *destroy_fnc, winio_cancel_fnc *cancel_fnc) {
    struct winfile *file = (struct winfile*) malloc(sizeof(struct winfile));
    if(!file) { perror("Couldn't allocate file object"); abort(); }
    file->ctx = ctx;
    file->is_async = is_async;
    file->read_fnc = read_fnc;
    file->write_fnc = write_fnc;
    file->devctrl_fnc = devctrl_fnc;
    file->cancel_fnc = cancel_fnc;
    file->destroy_fnc = destroy_fnc;
    return winhandle_create(file, (winhandle_destr_fnc*) file_destr);
}

void *winio_get_file_context(HANDLE file) { return ((struct winfile*) file->data)->ctx; }

__winfnc BOOL AreFileApisANSI() { return TRUE; }
WINAPI(AreFileApisANSI)

__winfnc BOOL CancelIoEx(HANDLE file, OVERLAPPED *ovlp) {
    winio_cancel_overlapped(ovlp);
    return TRUE;
}
WINAPI(CancelIoEx)

__winfnc BOOL GetOverlappedResult(HANDLE file, OVERLAPPED *ovlp, DWORD *num_transfered, BOOL wait) {
    if(ovlp->Internal == STATUS_PENDING && !wait) return FALSE;

    size_t sz;
    NTSTATUS status = winio_wait_overlapped(ovlp, &sz);
    if(status == STATUS_SUCCESS) *num_transfered = (DWORD) sz;
    return status;
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

    if(file->is_async) return TRUE;

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, num_read, TRUE);
}
WINAPI(ReadFile)

__winfnc BOOL WriteFile(HANDLE handle, const void *buf, DWORD buf_size, DWORD *num_written, OVERLAPPED *ovlp) {
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
    NTSTATUS status = file->write_fnc(file->ctx, ovlp, off, buf, buf_size, op_ctx);
    winerr_set_code(status);
    if(status != STATUS_SUCCESS) return FALSE;

    if(file->is_async) return TRUE;

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, num_written, TRUE);
}
WINAPI(WriteFile)

__winfnc BOOL DeviceIoControl(HANDLE handle, DWORD code, const void *in_buf, DWORD in_size, void *out_buf, DWORD out_size, DWORD *out_ret, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->read_fnc) { winerr_set(); return FALSE; }

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

    if(file->is_async) return TRUE;

    //Wait for the operation to complete
    return GetOverlappedResult(handle, ovlp, out_ret, TRUE);
}
WINAPI(DeviceIoControl)