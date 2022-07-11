#include "internal.h"

typedef enum {
    WdfRequestStopActionInvalid,
    WdfRequestStopActionSuspend,
    WdfRequestStopActionPurge,
    WdfRequestStopRequestCancelable
} WDF_REQUEST_STOP_ACTION_FLAGS;

typedef void __winfnc WDF_IO_QUEUE_IO_DEFAULT(WDFOBJECT Queue, WDFOBJECT Request);
typedef void __winfnc WDF_IO_QUEUE_IO_STOP(WDFOBJECT Queue, WDFOBJECT Request, WDF_REQUEST_STOP_ACTION_FLAGS ActionFlags);
typedef void __winfnc WDF_IO_QUEUE_IO_RESUME(WDFOBJECT Queue, WDFOBJECT Request);
typedef void __winfnc WDF_IO_QUEUE_IO_READ(WDFOBJECT Queue, WDFOBJECT Request, size_t Length);
typedef void __winfnc WDF_IO_QUEUE_IO_WRITE(WDFOBJECT Queue, WDFOBJECT Request, size_t Length);
typedef void __winfnc WDF_IO_QUEUE_IO_DEVICE_CONTROL(WDFOBJECT Queue, WDFOBJECT Request, size_t OutputBufferLength, size_t InputBufferLength, ULONG IoControlCode);
typedef void __winfnc WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(WDFOBJECT Queue, WDFOBJECT Request, size_t OutputBufferLength, size_t InputBufferLength, ULONG IoControlCode);
typedef void __winfnc WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE(WDFOBJECT Queue, WDFOBJECT Request);

typedef struct {
    ULONG Size;
    ULONG DispatchType;
    ULONG PowerManaged;
    BOOLEAN AllowZeroLengthRequests;
    BOOLEAN DefaultQueue;
    WDF_IO_QUEUE_IO_DEFAULT *EvtIoDefault;
    WDF_IO_QUEUE_IO_READ *EvtIoRead;
    WDF_IO_QUEUE_IO_WRITE *EvtIoWrite;
    WDF_IO_QUEUE_IO_DEVICE_CONTROL *EvtIoDeviceControl;
    WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL *EvtIoInternalDeviceControl;
    WDF_IO_QUEUE_IO_STOP *EvtIoStop;
    WDF_IO_QUEUE_IO_RESUME *EvtIoResume;
    WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE *EvtIoCanceledOnQueue;
} WDF_IO_QUEUE_CONFIG;

struct winwdf_queue {
    struct wdf_object object;
    struct winwdf_device *device;

    WDF_IO_QUEUE_CONFIG config;

    bool stopped;
};

static void queue_destr(struct winwdf_queue *queue) {
    //Stop the queue
    if(queue->config.PowerManaged && !queue->stopped && queue->config.EvtIoStop) queue->config.EvtIoStop(&queue->object, NULL, WdfRequestStopActionSuspend); 

    //Remove queue from device
    wdf_remove_device_queue(queue->device, queue);

    //Free memory
    wdf_cleanup_obj(&queue->object);
    free(queue);
}

void wdf_queue_resume(struct winwdf_queue *queue) {
    if(!queue->config.PowerManaged || !queue->stopped) return;
    if(queue->config.EvtIoResume) queue->config.EvtIoResume(&queue->object, NULL);
    queue->stopped = false;
}

void wdf_queue_stop(struct winwdf_queue *queue) {
    if(!queue->config.PowerManaged || queue->stopped) return;
    if(queue->config.EvtIoStop) queue->config.EvtIoStop(&queue->object, NULL, WdfRequestStopActionSuspend);
    queue->stopped = true;
}

void wdf_queue_create(struct winwdf_queue *queue, struct winwdf_request *req) {
    if(queue->stopped || !queue->config.EvtIoDefault) {
        log_warn("Couldn't process WDF queue create request!");
    } else queue->config.EvtIoDefault(&queue->object, (WDFOBJECT) req);
}

void wdf_queue_read(struct winwdf_queue *queue, struct winwdf_request *req, size_t len) {
    if(queue->stopped || !queue->config.EvtIoRead) {
        log_warn("Couldn't process WDF queue read request!");
    } else queue->config.EvtIoRead(&queue->object, (WDFOBJECT) req, len);
}

void wdf_queue_write(struct winwdf_queue *queue, struct winwdf_request *req, size_t len) {
    if(queue->stopped || !queue->config.EvtIoWrite) {
        log_warn("Couldn't process WDF queue write request!");
    } else queue->config.EvtIoWrite(&queue->object, (WDFOBJECT) req, len);
}

void wdf_queue_devctrl(struct winwdf_queue *queue, struct winwdf_request *req, ULONG code, size_t in_len, size_t out_len) {
    if(queue->stopped || !queue->config.EvtIoDeviceControl) {
        log_warn("Couldn't process WDF queue devctrl request!");
    } else queue->config.EvtIoDeviceControl(&queue->object, (WDFOBJECT) req, out_len, in_len, code);
}

void wdf_queue_int_devctrl(struct winwdf_queue *queue, struct winwdf_request *req, ULONG code, size_t in_len, size_t out_len) {
    if(queue->stopped || !queue->config.EvtIoInternalDeviceControl) {
        log_warn("Couldn't process WDF queue internal devctrl request!");
    } else queue->config.EvtIoInternalDeviceControl(&queue->object, (WDFOBJECT) req, out_len, in_len, code);
}

__winfnc NTSTATUS WdfIoQueueCreate(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device, WDF_IO_QUEUE_CONFIG *config, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    if(config->Size < sizeof(WDF_IO_QUEUE_CONFIG)) return WINERR_SET_CODE;

    //Create the queue object
    struct winwdf_queue *queue = (struct winwdf_queue*) malloc(sizeof(struct winwdf_queue));
    if(!queue) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &queue->object, (wdf_obj_destr_fnc*) queue_destr, obj_attrs);
    queue->device = (struct winwdf_device*) device;
    queue->config = *config;

    queue->stopped = false;

    //Add queue to device
    wdf_add_device_queue(queue->device, queue);

    if(out) *out = &queue->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfIoQueueCreate, 85)

__winfnc NTSTATUS WdfDeviceConfigureRequestDispatching(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device, WDFOBJECT queue_obj, WDF_REQUEST_TYPE req_type) {
    struct winwdf_queue *queue = (struct winwdf_queue*) queue_obj;

    if(queue->device != (struct winwdf_device*) device) return WINERR_SET_CODE;

    //Translate the request type
    enum wdf_queue_req_type req;
    switch(req_type) {
        case WdfRequestTypeCreate: req = WDF_QUEUE_REQ_CREATE; break;
        case WdfRequestTypeRead: req = WDF_QUEUE_REQ_READ; break;
        case WdfRequestTypeWrite: req = WDF_QUEUE_REQ_WRITE; break;
        case WdfRequestTypeDeviceControl: req = WDF_QUEUE_REQ_DEVCTRL; break;
        case WdfRequestTypeDeviceControlInternal: req = WDF_QUEUE_REQ_DEVCTRL_INT; break;
        default: return WINERR_SET_CODE;
    }

    //Configure the dispatch
    wdf_configure_device_dispatch(queue->device, queue, req);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfDeviceConfigureRequestDispatching, 40)

__winfnc void WdfIoQueueStart(WDF_DRIVER_GLOBALS *globals, WDFOBJECT queue_obj) {
    
}
WDFFUNC(WdfIoQueueStart, 87)