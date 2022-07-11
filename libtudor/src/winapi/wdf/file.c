#include "internal.h"

struct winwdf_file {
    struct wdf_object object;
    struct winwdf_device *device;
};

static void file_destr(struct winwdf_file *file) {
    //Call callbacks
    WDF_FILEOBJECT_CONFIG *cfg = wdf_get_fs_config(file->device);
    if(cfg) {
        if(cfg->EvtFileCleanup) cfg->EvtFileCleanup(&file->object);
        if(cfg->EvtFileClose) cfg->EvtFileClose(&file->object);
    }

    //Free memory
    wdf_cleanup_obj(&file->object);
    free(file);
}

NTSTATUS winwdf_open_device(struct winwdf_device *device, struct winwdf_file **out) {
    struct winwdf_queue *create_queue = wdf_get_dispatch_queue(device, WDF_QUEUE_REQ_CREATE);

    //Allocate the file object
    struct winwdf_file *file = (struct winwdf_file*) malloc(sizeof(struct winwdf_file));
    if(!file) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) device, &file->object, (wdf_obj_destr_fnc*) file_destr, wdf_get_fs_obj_attrs(device));
    file->device = device;

    //Create request
    struct winwdf_request *req = wdf_create_request(&file->object, wdf_get_fs_obj_attrs(device));
    wdf_configure_request(req, create_queue, file, 0, 0, NULL, NULL, NULL, &(WDF_REQUEST_PARAMETERS) {
        .Size = sizeof(WDF_REQUEST_PARAMETERS),
        .MinorFunction = 0,
        .Type = WdfRequestTypeCreate,
        .Parameters.Create.SecurityContext = NULL,
        .Parameters.Create.Options = 0,
        .Parameters.Create.FileAttributes = 0x80, //FILE_ATTRIBUTE_NORMAL
        .Parameters.Create.ShareAccess = 0,
        .Parameters.Create.EaLength = 0
    });
    wdf_start_request(req, &file->object, -1);

    //Call callbacks
    WDF_FILEOBJECT_CONFIG *cfg = wdf_get_fs_config(device);
    if(create_queue) {
        wdf_queue_create(create_queue, req);
    } else if(cfg) {
        if(cfg->EvtDeviceFileCreate) cfg->EvtDeviceFileCreate((WDFOBJECT) device, (WDFOBJECT) req, &file->object);
    }

    NTSTATUS status = winwdf_wait_request(req);
    if(status == STATUS_SUCCESS) *out = file;

    winwdf_destroy_object((WDFOBJECT) req);
    return status;
}

void winwdf_close_file(struct winwdf_file *file) {
    winwdf_destroy_object(&file->object);
}

struct devctrl_params {
    void *out_buf;
    size_t out_size;
};

static void devctrl_cb(struct winwdf_request *req, NTSTATUS status, struct devctrl_params *params) {
    if(status == STATUS_SUCCESS) {
        size_t act_size = wdf_get_info(req);
        memcpy(params->out_buf, wdf_get_request_out_buf(req), (params->out_size < act_size) ? params->out_size : act_size);
    }
    free(params);
}

NTSTATUS winwdf_devctrl_file(struct winwdf_file *file, ULONG code, const void *in_buf, size_t in_size, void *out_buf, size_t out_size, struct winwdf_request **out) {
    struct winwdf_queue *devctrl_queue = wdf_get_dispatch_queue(file->device, WDF_QUEUE_REQ_DEVCTRL);
    if(!devctrl_queue) {
        log_warn("WDF device has no associated devctrl queue!");
        return WINERR_SET_CODE;
    }

    //Create request
    struct winwdf_request *req = wdf_create_request(&file->object, wdf_get_fs_obj_attrs(file->device));
    wdf_configure_request(req, devctrl_queue, file, in_size, out_size, NULL, NULL, NULL, &(WDF_REQUEST_PARAMETERS) {
        .Size = sizeof(WDF_REQUEST_PARAMETERS),
        .MinorFunction = 0,
        .Type = WdfRequestTypeDeviceControl,
        .Parameters.DeviceIoControl.IoControlCode = code,
        .Parameters.DeviceIoControl.InputBufferLength = in_size,
        .Parameters.DeviceIoControl.OutputBufferLength = out_size,
        .Parameters.DeviceIoControl.Type3InputBuffer = 0
    });
    memcpy(wdf_get_request_in_buf(req), in_buf, in_size);

    struct devctrl_params *params = (struct devctrl_params*) malloc(sizeof(struct devctrl_params));
    if(!params) return winerr_from_errno();
    *params = (struct devctrl_params) { .out_buf = out_buf, .out_size = out_size };
    winwdf_add_request_callback(req, (winwdf_request_cb_fnc*) devctrl_cb, params);

    wdf_start_request(req, &file->object, -1);

    //Call callbacks
    wdf_queue_devctrl(devctrl_queue, req, code, in_size, out_size);

    *out = req;
    return STATUS_SUCCESS;
}