#include <stdlib.h>
#include "internal.h"

NTSTATUS winio_wait_overlapped(OVERLAPPED *ovlp, size_t *num_transfered) {
    //Wait for the operation to be completed
    if(ovlp->Internal == STATUS_PENDING) win_wait_sync_obj(ovlp->hEvent, INFINITE);
    if(num_transfered) *num_transfered = ovlp->InternalHigh;
    return (NTSTATUS) ovlp->Internal;
}

static inline bool init_overlapped(OVERLAPPED *ovlp) {
    ovlp->Internal = STATUS_PENDING;
    ovlp->InternalHigh = 0;
    if(!ovlp->hEvent) ovlp->hEvent = win_create_event(NULL, false, false);
    if(!ovlp->hEvent) return false;
    win_reset_event(ovlp->hEvent);
    return true;
}

struct winfile {
    void *ctx;
    bool is_async;
    winio_read_fnc *read_fnc;
    winio_write_fnc *write_fnc;
    winio_devctrl_fnc *devctrl_fnc;
    winio_destroy_fnc *destroy_fnc;
};

static void file_destr(struct winfile *file) {
    if(file->destroy_fnc) file->destroy_fnc(file->ctx);
    free(file);
}

HANDLE winio_create_file(void *ctx, bool is_async, winio_read_fnc *read_fnc, winio_write_fnc *write_fnc, winio_devctrl_fnc *devctrl_fnc, winio_destroy_fnc *destroy_fnc) {
    struct winfile *file = (struct winfile*) malloc(sizeof(struct winfile));
    if(!file) { perror("Couldn't allocate file object"); abort(); }
    file->ctx = ctx;
    file->is_async = is_async;
    file->read_fnc = read_fnc;
    file->write_fnc = write_fnc;
    file->devctrl_fnc = devctrl_fnc;
    file->destroy_fnc = destroy_fnc;
    return winhandle_create(file, (winhandle_destr_fnc*) file_destr);
}

void *winio_get_file_context(HANDLE file) { return ((struct winfile*) file->data)->ctx; }

__winfnc BOOL AreFileApisANSI() { return TRUE; }
WINAPI(AreFileApisANSI)

struct read_file_ctx {
    struct winfile *file;
    OVERLAPPED *ovlp;

    void *buf;
    size_t buf_size;
};

static void *read_file_thread(struct read_file_ctx *ctx) {
    //Call callback
    off_t off = ctx->ovlp ? (ctx->ovlp->Offset | ((off_t) ctx->ovlp->OffsetHigh) << 32) : 0;
    size_t sz = ctx->buf_size;
    ctx->ovlp->Internal = ctx->file->read_fnc(ctx->file->ctx, off, ctx->buf, &sz);

    //Setup overlapped
    if(ctx->ovlp->Internal == STATUS_SUCCESS) ctx->ovlp->InternalHigh = sz;
    if(ctx->ovlp->hEvent) win_set_event(ctx->ovlp->hEvent);

    free(ctx);
    return NULL;
}

__winfnc BOOL ReadFile(HANDLE handle, void *buf, DWORD buf_size, DWORD *num_read, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->read_fnc) { winerr_set(); return FALSE; }

    if(file->is_async) {
        //Setup overlapped
        if(!init_overlapped(ovlp)) return FALSE;

        //Start thread
        struct read_file_ctx *ctx = (struct read_file_ctx*) malloc(sizeof(struct read_file_ctx));
        if(!ctx) { winerr_set_errno(); return FALSE; }
        ctx->file = file;
        ctx->ovlp = ovlp;
        ctx->buf = buf;
        ctx->buf_size = buf_size;

        pthread_t thread;
        cant_fail(pthread_create(&thread, NULL, (void*(*)(void*)) read_file_thread, ctx));

        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    } else {
        off_t off = ovlp ? (ovlp->Offset | ((off_t) ovlp->OffsetHigh) << 32) : 0;
        size_t sz = buf_size;
        NTSTATUS status = file->read_fnc(file->ctx, off, buf, &sz);

        winerr_set_code(status);
        if(status == STATUS_SUCCESS) *num_read = (DWORD) sz;
        return status == STATUS_SUCCESS;
    }
}
WINAPI(ReadFile)

struct write_file_ctx {
    struct winfile *file;
    OVERLAPPED *ovlp;

    const void *buf;
    size_t buf_size;
};

static void *write_file_thread(struct write_file_ctx *ctx) {
    //Call callback
    off_t off = ctx->ovlp ? (ctx->ovlp->Offset | ((off_t) ctx->ovlp->OffsetHigh) << 32) : 0;
    size_t sz = ctx->buf_size;
    ctx->ovlp->Internal = ctx->file->write_fnc(ctx->file->ctx, off, ctx->buf, &sz);

    //Setup overlapped
    if(ctx->ovlp->Internal == STATUS_SUCCESS) ctx->ovlp->InternalHigh = sz;
    if(ctx->ovlp->hEvent) win_set_event(ctx->ovlp->hEvent);

    free(ctx);
    return NULL;
}

__winfnc BOOL WriteFile(HANDLE handle, const void *buf, DWORD buf_size, DWORD *num_written, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->write_fnc) { winerr_set(); return FALSE; }

    if(file->is_async) {
        //Setup overlapped
        if(!init_overlapped(ovlp)) return FALSE;

        //Start thread
        struct write_file_ctx *ctx = (struct write_file_ctx*) malloc(sizeof(struct write_file_ctx));
        if(!ctx) { winerr_set_errno(); return FALSE; }
        ctx->file = file;
        ctx->ovlp = ovlp;
        ctx->buf = buf;
        ctx->buf_size = buf_size;

        pthread_t thread;
        cant_fail(pthread_create(&thread, NULL, (void*(*)(void*)) write_file_thread, ctx));

        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    } else {
        off_t off = ovlp ? (ovlp->Offset | ((off_t) ovlp->OffsetHigh) << 32) : 0;
        size_t sz = buf_size;
        NTSTATUS status = file->write_fnc(file->ctx, off, buf, &sz);

        winerr_set_code(status);
        if(status == STATUS_SUCCESS) *num_written = (DWORD) sz;
        return status == STATUS_SUCCESS;
    }
}
WINAPI(WriteFile)

struct devctrl_ctx {
    struct winfile *file;
    OVERLAPPED *ovlp;

    DWORD code;
    const void *in_buf;
    size_t in_size;
    void *out_buf;
    size_t out_size;
};

static void *devctrl_thread(struct devctrl_ctx *ctx) {
    //Call callback
    size_t sz = ctx->out_size;
    ctx->ovlp->Internal = ctx->file->devctrl_fnc(ctx->file->ctx, ctx->code, ctx->in_buf, ctx->in_size, ctx->out_buf, &sz);

    //Setup overlapped
    if(ctx->ovlp->Internal == STATUS_SUCCESS) ctx->ovlp->InternalHigh = sz;
    if(ctx->ovlp->hEvent) win_set_event(ctx->ovlp->hEvent);

    free(ctx);
    return NULL;
}

__winfnc BOOL DeviceIoControl(HANDLE handle, DWORD code, const void *in_buf, DWORD in_size, void *out_buf, DWORD out_size, DWORD *out_ret, OVERLAPPED *ovlp) {
    struct winfile *file = (struct winfile*) handle->data;
    if(!file->devctrl_fnc) { winerr_set(); return FALSE; }

    if(file->is_async) {
        //Setup overlapped
        if(!init_overlapped(ovlp)) return FALSE;

        //Start thread
        struct devctrl_ctx *ctx = (struct devctrl_ctx*) malloc(sizeof(struct devctrl_ctx));
        if(!ctx) { winerr_set_errno(); return FALSE; }
        ctx->file = file;
        ctx->ovlp = ovlp;
        ctx->code = code;
        ctx->in_buf = in_buf;
        ctx->in_size = in_size;
        ctx->out_buf = out_buf;
        ctx->out_size = out_size;

        pthread_t thread;
        cant_fail(pthread_create(&thread, NULL, (void*(*)(void*)) devctrl_thread, ctx));

        winerr_set_code(ERROR_IO_PENDING);
        return FALSE;
    } else {
        size_t sz = out_size;
        NTSTATUS status = file->devctrl_fnc(file->ctx, code, in_buf, in_size, out_buf, &sz);

        winerr_set_code(status);
        if(status == STATUS_SUCCESS) *out_ret = (DWORD) sz;
        return status == STATUS_SUCCESS;
    }
}
WINAPI(DeviceIoControl)

__winfnc BOOL CancelIoEx(HANDLE file, OVERLAPPED *ovlp) {
    //We don't support IO cancellation
    return TRUE;
}
WINAPI(CancelIoEx)

__winfnc BOOL GetOverlappedResult(HANDLE file, OVERLAPPED *ovlp, DWORD *num_transfered, BOOL wait) {
    //Wait for the operation to be completed
    if(ovlp->Internal == STATUS_PENDING) {
        if(!wait) {
            winerr_set(ERROR_IO_INCOMPLETE);
            return FALSE;
        }

        win_wait_sync_obj(ovlp->hEvent, INFINITE);
    }

    //Return result
    if(num_transfered) *num_transfered = (DWORD) ovlp->InternalHigh;
    winerr_set_code((int) ovlp->Internal);
    return ovlp->Internal == ERROR_SUCCESS;
}
WINAPI(GetOverlappedResult)