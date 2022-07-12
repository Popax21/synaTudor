#include "internal.h"

//TODO Rewrite this mess, and clearly seperate driver-issued and framework-issued requests

typedef void __winfnc WDF_REQUEST_CANCEL_FNC(WDFOBJECT Request);

#define WDF_REQUEST_SEND_OPTION_TIMEOUT 0x00000001
#define WDF_REQUEST_SEND_OPTION_SYNCHRONOUS 0x00000002

typedef struct {
    ULONG Size;
    ULONG Flags;
    LONGLONG Timeout;
} WDF_REQUEST_SEND_OPTIONS;

struct req_callback {
    struct req_callback *next;
    winwdf_request_cb_fnc *cb_func;
    void *ctx;
};

struct winwdf_request {
    struct wdf_object object;

    //Internal state
    pthread_mutex_t lock;
    bool is_configured, is_started, is_done, is_resetting;
    NTSTATUS status;
    pthread_cond_t compl_cond;

    //Configuration data
    void *context, *data;
    wdf_request_start_fnc *start_fnc;
    wdf_request_cancel_fnc *cancel_fnc;
    wdf_request_cleanup_fnc *cleanup_fnc;

    //API data
    struct winwdf_queue *io_queue;
    WDF_REQUEST_CANCEL_FNC *drv_cancel;
    WDF_REQUEST_PARAMETERS params;
    WDF_REQUEST_COMPLETION_PARAMS *compl_params;
    void *in_buf, *out_buf;
    size_t in_buf_size, out_buf_size;
    ULONG_PTR drv_info;

    //Callbacks
    struct req_callback *callbacks_head;
};

static void req_destr(struct winwdf_request *req) {
    //Reset the request
    wdf_reset_request(req);

    //Free memory
    cant_fail_ret(pthread_mutex_destroy(&req->lock));
    cant_fail_ret(pthread_cond_destroy(&req->compl_cond));
    wdf_cleanup_obj(&req->object);
    if(req->compl_params && req->compl_params->Type == WdfRequestTypeUsb) free(req->compl_params->Parameters.Usb.Completion);
    free(req->compl_params);
    free(req);
}

struct winwdf_request *wdf_create_request(struct wdf_object *parent, WDF_OBJECT_ATTRIBUTES *obj_attrs) {
    //Create the request object
    struct winwdf_request *req = (struct winwdf_request*) malloc(sizeof(struct winwdf_request));
    if(!req) { perror("Error allocating WDF request memory"); abort(); }

    wdf_create_obj(parent, &req->object, (wdf_obj_destr_fnc*) req_destr, obj_attrs);

    pthread_mutexattr_t lock_attr;
    cant_fail_ret(pthread_mutexattr_init(&lock_attr));
    cant_fail_ret(pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE));
    cant_fail_ret(pthread_mutex_init(&req->lock, &lock_attr));
    cant_fail_ret(pthread_mutexattr_destroy(&lock_attr));
    req->is_configured = req->is_started = req->is_done = false;
    req->status = STATUS_SUCCESS;
    cant_fail_ret(pthread_cond_init(&req->compl_cond, NULL));

    req->context = req->data = NULL;

    req->io_queue = NULL;
    req->drv_cancel = NULL;
    req->compl_params = NULL;
    req->in_buf = req->out_buf = NULL;
    req->drv_info = 0;

    req->callbacks_head = NULL;

    return req;
}

void wdf_reset_request(struct winwdf_request *req) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(!req->is_configured || req->is_resetting) {
        while(req->is_configured && req->is_started && !req->is_done) cant_fail_ret(pthread_cond_wait(&req->compl_cond, &req->lock));
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        return;
    }
    req->is_resetting = true;

    //Call callbacks
    if(req->is_started && !req->is_done) winwdf_cancel_request(req);
    while(req->is_configured && req->is_started && !req->is_done) cant_fail_ret(pthread_cond_wait(&req->compl_cond, &req->lock));
    if(req->cleanup_fnc) req->cleanup_fnc(req, req->context, req->data);

    //Reset state
    req->is_configured = req->is_started = req->is_done = req->is_resetting = false;
    req->status = STATUS_SUCCESS;
    if(req->compl_params && req->compl_params->Type == WdfRequestTypeUsb) free(req->compl_params->Parameters.Usb.Completion);
    free(req->compl_params);
    req->compl_params = NULL;

    req->context = req->data = NULL;

    req->io_queue = NULL;
    req->drv_cancel = NULL;
    free(req->in_buf);
    free(req->out_buf);
    req->in_buf = req->out_buf = NULL;
    req->drv_info = 0;

    for(struct req_callback *cb = req->callbacks_head, *ncb = cb ? cb->next : NULL; cb; cb = ncb, ncb = ncb ? ncb->next : NULL) free(cb);

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

void wdf_configure_request(struct winwdf_request *req, struct winwdf_queue *io_queue, void *ctx, size_t in_size, size_t out_size, wdf_request_start_fnc *start_fnc, wdf_request_cancel_fnc *cancel_fnc, wdf_request_cleanup_fnc *cleanup_fnc, WDF_REQUEST_PARAMETERS *params) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    while(req->is_configured && req->is_started && !req->is_done) cant_fail_ret(pthread_cond_wait(&req->compl_cond, &req->lock));
    if(req->is_configured) wdf_reset_request(req);

    //Set state
    req->is_configured = true;
    req->is_started = req->is_done = req->is_resetting = false;
    req->status = STATUS_SUCCESS;

    req->context = ctx;
    req->data = NULL;
    req->start_fnc = start_fnc;
    req->cancel_fnc = cancel_fnc;
    req->cleanup_fnc = cleanup_fnc;

    req->io_queue = io_queue;
    req->drv_cancel = NULL;
    if(params) memcpy(&req->params, params, sizeof(WDF_REQUEST_PARAMETERS));
    else memset(&req->params, 0, sizeof(WDF_REQUEST_PARAMETERS));
    req->compl_params = NULL;

    if(in_size > 0) {
        req->in_buf = malloc(in_size);
        if(!req->in_buf) { perror("Couldn't allocate WDF request input buffer"); abort(); }
    } else req->in_buf = NULL;
    req->in_buf_size = in_size;

    if(out_size > 0) {
        req->out_buf = malloc(out_size);
        if(!req->out_buf) { perror("Couldn't allocate WDF request input buffer"); abort(); }
    } else req->out_buf = NULL;
    req->out_buf_size = out_size;

    req->drv_info = 0;

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

void wdf_start_request(struct winwdf_request *req, struct wdf_object *target, int timeout) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured && !req->is_started) {
        if(req->start_fnc) req->status = req->start_fnc(req, req->context, target, timeout, &req->data);
        else {
            req->status = 0;
            req->data = NULL;
        }
        req->is_started = true;
    }
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

void wdf_complete_request(struct winwdf_request *req, NTSTATUS status, WDF_REQUEST_COMPLETION_PARAMS *params) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));

    if(!req->is_configured || !req->is_started || req->is_done) {
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        log_warn("wdf_complete_request called on non-pending request!");
        return;
    }

    //Set status
    req->is_done = true;
    req->status = status;

    if(params) {
        req->compl_params = (WDF_REQUEST_COMPLETION_PARAMS*) malloc(sizeof(WDF_REQUEST_COMPLETION_PARAMS));
        if(!req->compl_params) { perror("Couldn't allocate WDF request completion params"); abort(); }
        memcpy(req->compl_params, params, sizeof(WDF_REQUEST_COMPLETION_PARAMS));
        if(params->Type == WdfRequestTypeUsb && params->Parameters.Usb.Completion) {
            req->compl_params->Parameters.Usb.Completion = (WDF_USB_REQUEST_COMPLETION_PARAMS*) malloc(sizeof(WDF_USB_REQUEST_COMPLETION_PARAMS));
            if(!req->compl_params->Parameters.Usb.Completion) { perror("Couldn't allocate WDF USB request completion params"); abort(); }
            memcpy(req->compl_params->Parameters.Usb.Completion, params->Parameters.Usb.Completion, sizeof(WDF_USB_REQUEST_COMPLETION_PARAMS));
        }
    } else req->compl_params = NULL;

    //Signal that the request is done
    cant_fail_ret(pthread_cond_broadcast(&req->compl_cond));

    //Call callbacks
    for(struct req_callback *cb = req->callbacks_head; cb; cb = cb->next) cb->cb_func(req, status, cb->ctx);

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

void winwdf_add_request_callback(struct winwdf_request *req, winwdf_request_cb_fnc *cb_fnc, void *ctx) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));

    if(!req->is_configured) {
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        log_warn("winwdf_add_request_callback called on non-configured request!");
        return;
    }

    //Check if the request is already done
    if(req->is_started && req->is_done) {
        cb_fnc(req, req->status, ctx);
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        return;
    }

    //Allocate a new callback structure
    struct req_callback *cb = (struct req_callback*) malloc(sizeof(struct req_callback));
    if(!cb) { perror("Couldn't allocate memory for WDF request callback"); abort(); }
    *cb = (struct req_callback) {
        .next = req->callbacks_head,
        .cb_func = cb_fnc,
        .ctx = ctx
    };
    req->callbacks_head = cb;

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

void winwdf_cancel_request(struct winwdf_request *req) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));

    if(!req->is_configured || !req->is_started) {
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        log_warn("winwdf_cancel_request called on non-configured or non-started request!");
        return;
    }

    //Call cancel functions
    if(!req->is_done) {
        if(req->drv_cancel) {
            cant_fail_ret(pthread_mutex_unlock(&req->lock));
            req->drv_cancel(&req->object);
            cant_fail_ret(pthread_mutex_lock(&req->lock));
        }

        if(req->cancel_fnc) {
            req->cancel_fnc(req, req->context, req->data);
        }
    }
 
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}

NTSTATUS winwdf_wait_request(struct winwdf_request *req) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));

    NTSTATUS status = WINERR_SET_CODE;
    if(req->is_configured) {
        while(req->is_configured && !req->is_done) cant_fail_ret(pthread_cond_wait(&req->compl_cond, &req->lock));
        status = req->status;
    }

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return status;
}

bool winwdf_get_request_info(struct winwdf_request *req, NTSTATUS *status, const void **in_buf, size_t *in_buf_size, const void **out_buf, size_t *out_buf_size, ULONG_PTR *drv_info) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));

    if(!req->is_configured || !req->is_started) {
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        log_warn("winwdf_get_request_info called on non-configured or non-started request!");
        return false;
    }

    if(status) *status = req->status;
    if(in_buf) *in_buf = req->in_buf;
    if(in_buf_size) *in_buf_size = req->in_buf_size;
    if(out_buf) *out_buf = req->out_buf;
    if(out_buf_size) *out_buf_size = req->out_buf_size;
    if(drv_info) *drv_info = req->drv_info;

    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return true;
}

NTSTATUS wdf_get_request_status(struct winwdf_request *req) {
    NTSTATUS status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured && req->is_started && req->is_done) status = req->status;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return status;
}

void *wdf_get_request_in_buf(struct winwdf_request *req) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    void *in_buf = req->is_configured ? req->in_buf : NULL;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return in_buf;
}

void *wdf_get_request_out_buf(struct winwdf_request *req) {
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    void *out_buf = req->is_configured ? req->out_buf : NULL;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return out_buf;
}

ULONG_PTR wdf_get_info(struct winwdf_request *req) {
    ULONG_PTR info = 0;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) info = req->drv_info;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return info;
}

__winfnc NTSTATUS WdfRequestCreate(WDF_DRIVER_GLOBALS *globals, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT io_target, WDFOBJECT *out) {
    *out = &wdf_create_request((WDFOBJECT) winwdf_get_driver(globals), obj_attrs)->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfRequestCreate, 148)

__winfnc NTSTATUS WdfRequestMarkCancelable(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, WDF_REQUEST_CANCEL_FNC *cancel_fnc) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    NTSTATUS status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) {
        req->drv_cancel = cancel_fnc;
        status = STATUS_SUCCESS;
    }
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return status;
}
WDFFUNC(WdfRequestMarkCancelable, 154)

__winfnc NTSTATUS WdfRequestUnmarkCancelable(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj) {

    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    NTSTATUS status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) {
        req->drv_cancel = NULL;
        status = STATUS_SUCCESS;
    }
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
    return status;
}
WDFFUNC(WdfRequestUnmarkCancelable, 156)

__winfnc BOOLEAN WdfRequestSend(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, WDFOBJECT target, WDF_REQUEST_SEND_OPTIONS *opts) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    //Determine the timeout
    int timeout = -1;
    if(opts->Flags & WDF_REQUEST_SEND_OPTION_TIMEOUT) {
        if(opts->Timeout > 0) { log_warn("WdfRequestSend called with absolute timeout value!"); }
        else if(opts->Timeout < 0) timeout = (int) (-opts->Timeout / 10000);
    }

    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(!req->is_configured || req->is_started) {
        cant_fail_ret(pthread_mutex_unlock(&req->lock));
        return FALSE;
    }

    //Start the request
    if(req->start_fnc) req->status = req->start_fnc(req, req->context, target, timeout, &req->data);
    else {
        req->status = 0;
        req->data = NULL;
    }

    NTSTATUS status = req->status;
    if(status == STATUS_SUCCESS) {
        req->is_started = true;

        if(opts->Flags & WDF_REQUEST_SEND_OPTION_SYNCHRONOUS) {
            //Wait for the request to be completed
            while(req->is_configured && !req->is_done) cant_fail_ret(pthread_cond_wait(&req->compl_cond, &req->lock));
            status = req->status;
        }
    } else req->is_done = true;

    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return status == STATUS_SUCCESS;
}
WDFFUNC(WdfRequestSend, 152)

__winfnc NTSTATUS WdfRequestGetStatus(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    NTSTATUS status;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    status = (req->is_configured && req->is_started && req->is_done) ? req->status : WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return status;
}
WDFFUNC(WdfRequestGetStatus, 153)

__winfnc WDFOBJECT WdfRequestGetIoQueue(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    WDFOBJECT queue;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    queue = (req->is_configured) ? (WDFOBJECT) req->io_queue : NULL;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return queue;
}
WDFFUNC(WdfRequestGetIoQueue, 175)

__winfnc void WdfRequestGetParameters(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, WDF_REQUEST_PARAMETERS *params) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) memcpy(params, &req->params, sizeof(WDF_REQUEST_PARAMETERS));
    else *params = (WDF_REQUEST_PARAMETERS) {0};
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}
WDFFUNC(WdfRequestGetParameters, 165)

__winfnc NTSTATUS WdfRequestGetCompletionParams(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, WDF_REQUEST_COMPLETION_PARAMS *params) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    NTSTATUS status;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured && req->is_started && req->is_done) {
        *params = *req->compl_params;
        status = STATUS_SUCCESS;
    } else status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return status;
}
WDFFUNC(WdfRequestGetCompletionParams, 161)

__winfnc NTSTATUS WdfRequestRetrieveInputBuffer(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, size_t min_len, void **buf, size_t *len) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    NTSTATUS status;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) {
        if(req->in_buf_size < min_len) status = STATUS_BUFFER_TOO_SMALL;
        else {
            *buf = req->in_buf;
            if(len) *len = req->in_buf_size;
            status = STATUS_SUCCESS;
        }
    } else status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return status;
}
WDFFUNC(WdfRequestRetrieveInputBuffer, 168)

__winfnc NTSTATUS WdfRequestRetrieveOutputBuffer(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, size_t min_len, void **buf, size_t *len) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    NTSTATUS status;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) {
        if(req->out_buf_size < min_len) status = STATUS_BUFFER_TOO_SMALL;
        else {
            *buf = req->out_buf;
            if(len) *len = req->out_buf_size;
            status = STATUS_SUCCESS;
        }
    } else status = WINERR_SET_CODE;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return status;
}
WDFFUNC(WdfRequestRetrieveOutputBuffer, 169)

__winfnc ULONG_PTR WdfRequestGetInformation(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    ULONG_PTR info = 0;
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) info = req->drv_info;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));

    return info;
}
WDFFUNC(WdfRequestGetInformation, 171)

__winfnc void WdfRequestSetInformation(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, ULONG_PTR info) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    
    cant_fail_ret(pthread_mutex_lock(&req->lock));
    if(req->is_configured) req->drv_info = info;
    cant_fail_ret(pthread_mutex_unlock(&req->lock));
}
WDFFUNC(WdfRequestSetInformation, 170)

__winfnc void WdfRequestComplete(WDF_DRIVER_GLOBALS *globals, WDFOBJECT req_obj, NTSTATUS status) {
    struct winwdf_request *req = (struct winwdf_request*) req_obj;
    wdf_complete_request(req, status, NULL);
}
WDFFUNC(WdfRequestComplete, 163)