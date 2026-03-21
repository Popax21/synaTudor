#include "internal.h"

typedef struct {
    ULONG USBDI_Version;
    ULONG Supported_USB_Version;
} USBD_VERSION_INFORMATION;

typedef struct {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bInterfaceNumber;
    UCHAR bAlternateSetting;
    UCHAR bNumEndpoints;
    UCHAR bInterfaceClass;
    UCHAR bInterfaceSubClass;
    UCHAR bInterfaceProtocol;
    UCHAR iInterface;
} USB_INTERFACE_DESCRIPTOR;

typedef struct {
    ULONG Size;
    USBD_VERSION_INFORMATION UsbdVersionInformation;
    ULONG HcdPortCapabilities;
    ULONG Traits;
} WDF_USB_DEVICE_INFORMATION;

typedef enum {
    WdfUsbPipeTypeInvalid,
    WdfUsbPipeTypeControl,
    WdfUsbPipeTypeIsochronous,
    WdfUsbPipeTypeBulk,
    WdfUsbPipeTypeInterrupt
} WDF_USB_PIPE_TYPE;

typedef struct {
    ULONG Size;
    ULONG MaximumPacketSize;
    UCHAR EndpointAddress;
    UCHAR Interval;
    UCHAR SettingIndex;
    WDF_USB_PIPE_TYPE PipeType;
    ULONG MaximumTransferSize;
} WDF_USB_PIPE_INFORMATION;

typedef struct {
    ULONG Size;
    ULONG Flags;
    LONGLONG Timeout;
} WDF_REQUEST_SEND_OPTIONS;

struct wdf_usb_interface;

struct wdf_usb_device {
    struct wdf_object object;
    struct winwdf_device *device;
    bool is_dying;

    libusb_device_handle *libusb_dev;
    struct libusb_config_descriptor *cfg_descr;

    pthread_mutex_t ifs_lock;
    struct wdf_usb_interface *ifs_head;
};

static void usb_dev_destr(struct wdf_usb_device *usb_dev) {
    //Remove from device
    wdf_set_usb_device(usb_dev->device, NULL);

    //Destroy interfaces
    usb_dev->is_dying = true;
    cant_fail_ret(pthread_mutex_lock(&usb_dev->ifs_lock));
    while(usb_dev->ifs_head != NULL) winwdf_destroy_object((WDFOBJECT) usb_dev->ifs_head);
    cant_fail_ret(pthread_mutex_unlock(&usb_dev->ifs_lock));
    cant_fail_ret(pthread_mutex_destroy(&usb_dev->ifs_lock));

    //Free memory
    wdf_cleanup_obj(&usb_dev->object);
    free(usb_dev);
}

struct wdf_usb_pipe;

struct wdf_usb_interface {
    struct wdf_object object;
    struct wdf_usb_device *usb_device;
    struct wdf_usb_interface *prev, *next;

    BYTE interface_idx;
    const struct libusb_interface *libusb_if;

    int num_pipes;
    struct wdf_usb_pipe *pipes;
};

struct wdf_usb_pipe {
    struct wdf_object object;
    struct wdf_usb_interface *usb_if;

    UCHAR pipe_idx;
    const struct libusb_endpoint_descriptor *libusb_ep;
};

static void usb_if_destr(struct wdf_usb_interface *usb_if) {
    if(usb_if->libusb_if) {
        //Unlink from USB device
        if(!usb_if->usb_device->is_dying) cant_fail_ret(pthread_mutex_lock(&usb_if->usb_device->ifs_lock));

        if(usb_if->prev) usb_if->prev->next = usb_if->next;
        else usb_if->usb_device->ifs_head = usb_if->next;
        if(usb_if->next) usb_if->next->prev = usb_if->prev;

        if(!usb_if->usb_device->is_dying) cant_fail_ret(pthread_mutex_unlock(&usb_if->usb_device->ifs_lock));

        //Destroy pipes
        for(int i = 0; i < usb_if->num_pipes; i++) winwdf_destroy_object((WDFOBJECT) &usb_if->pipes[i]);

        //Release the interface
        int usb_err;
        if((usb_err = libusb_release_interface(usb_if->usb_device->libusb_dev, usb_if->interface_idx)) != 0) {
            log_warn("libusb_release_interface failed: %d [%s]", usb_err, libusb_error_name(usb_err));
            abort();
        }
    }

    //Free memory
    wdf_cleanup_obj(&usb_if->object);
    free(usb_if->pipes);
    free(usb_if);
}

__winfnc NTSTATUS WdfUsbTargetDeviceCreate(WDF_DRIVER_GLOBALS *globals, WDFOBJECT dev_obj, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    struct winwdf_device *dev = (struct winwdf_device*) dev_obj;
    if(!wdf_get_libusb_device(dev)) {
        log_warn("Attempted to create WDF USB device for non-USB device!");
        return WINERR_SET_CODE;
    }

    if(wdf_get_usb_device(dev)) return WINERR_SET_CODE;

    //Create USB device
    struct wdf_usb_device *usb_dev = (struct wdf_usb_device*) malloc(sizeof(struct wdf_usb_device));
    if(!usb_dev) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &usb_dev->object, (wdf_obj_destr_fnc*) usb_dev_destr, obj_attrs);
    usb_dev->device = dev;
    usb_dev->is_dying = false;

    usb_dev->libusb_dev = wdf_get_libusb_device(dev);

    cant_fail_ret(pthread_mutex_init(&usb_dev->ifs_lock, NULL));
    usb_dev->ifs_head = NULL;

    //Initialize device
    int usb_err;
    if((usb_err = libusb_reset_device(usb_dev->libusb_dev)) != 0) {
        log_error("libusb_reset_device failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }
    if((usb_err = libusb_get_config_descriptor(libusb_get_device(usb_dev->libusb_dev), 0, &usb_dev->cfg_descr)) != 0) {
        log_error("libusb_get_config_descriptor failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }
    if((usb_err = libusb_set_configuration(usb_dev->libusb_dev, usb_dev->cfg_descr->bConfigurationValue)) != 0) {
        log_error("libusb_set_configuration failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }

    //Detach kernel drivers
    for(int i = 0; i < usb_dev->cfg_descr->bNumInterfaces; i++) {
        if(libusb_kernel_driver_active(usb_dev->libusb_dev, i)) {
            log_debug("Detaching USB kernel driver from interface %d...", i);
            if((usb_err = libusb_detach_kernel_driver(usb_dev->libusb_dev, i)) != 0) {
                log_error("libusb_detach_kernel_driver failed: %d [%s]", usb_err, libusb_error_name(usb_err));
                return WINERR_SET_CODE;
            }
        }
    }

    //Send vendor init sequence to wake the sensor
    //The sensor needs SET_IDLE + vendor 0x19/0x1a before accepting interrupt transfers
    {
        uint8_t resp;

        //SET_IDLE: bmRequestType=0x21 (class, interface, OUT), bRequest=0x0A
        libusb_control_transfer(usb_dev->libusb_dev, 0x21, 0x0A, 0, 0, NULL, 0, 1000);
        log_info("Sensor init: SET_IDLE sent");

        //Vendor OUT: bRequest=0x19 (session/reset start)
        libusb_control_transfer(usb_dev->libusb_dev, 0x40, 0x19, 0, 0, NULL, 0, 1000);
        log_info("Sensor init: Vendor 0x19 sent");

        //Vendor IN: bRequest=0x1a (session ack, 1 byte)
        resp = 0;
        int ret = libusb_control_transfer(usb_dev->libusb_dev, 0xC0, 0x1a, 0, 0, &resp, 1, 1000);
        log_info("Sensor init: Vendor 0x1a → ret=%d resp=0x%02x", ret, resp);
    }

    wdf_set_usb_device(dev, usb_dev);
    *out = &usb_dev->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetDeviceCreate, 202)

__winfnc NTSTATUS WdfUsbTargetDeviceRetrieveInformation(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_dev_obj, WDF_USB_DEVICE_INFORMATION *ver) {
    ver->Size = sizeof(WDF_USB_DEVICE_INFORMATION);
    ver->UsbdVersionInformation.USBDI_Version = 0x00000600;
    ver->UsbdVersionInformation.Supported_USB_Version = 0x0302;
    ver->HcdPortCapabilities = 0;
    ver->Traits = 0;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetDeviceRetrieveInformation, 204)

__winfnc NTSTATUS WdfUsbTargetDeviceGetDeviceDescriptor(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_dev_obj, USB_DEVICE_DESCRIPTOR *descrp) {
    struct wdf_usb_device *usb_dev = (struct wdf_usb_device*) usb_dev_obj;

    int usb_err;
    if((usb_err = libusb_get_device_descriptor(libusb_get_device(usb_dev->libusb_dev), (struct libusb_device_descriptor*) descrp)) != 0) {
        log_error("libusb_get_device_descriptor failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;        
    }

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetDeviceGetDeviceDescriptor, 205)

__winfnc NTSTATUS WdfUsbTargetDeviceResetPortSynchronously(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_dev_obj) {
    struct wdf_usb_device *usb_dev = (struct wdf_usb_device*) usb_dev_obj;

    int usb_err;
    if((usb_err = libusb_reset_device(usb_dev->libusb_dev)) != 0) {
        log_error("libusb_reset_device failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetDeviceResetPortSynchronously, 214)

__winfnc WDFOBJECT WdfUsbTargetDeviceGetInterface(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_dev_obj, UCHAR interface_idx) {
    struct wdf_usb_device *usb_dev = (struct wdf_usb_device*) usb_dev_obj;
    if(interface_idx >= usb_dev->cfg_descr->bNumInterfaces) return NULL;

    //Create USB interface
    struct wdf_usb_interface *usb_if = (struct wdf_usb_interface*) malloc(sizeof(struct wdf_usb_interface));
    if(!usb_if) { perror("Couldn't allocate USB interface object"); return NULL; }

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &usb_if->object, (wdf_obj_destr_fnc*) usb_if_destr, NULL);
    usb_if->usb_device = usb_dev;
    usb_if->interface_idx = interface_idx;

    //Claim the interface
    int usb_err;
    if((usb_err = libusb_claim_interface(usb_dev->libusb_dev, interface_idx)) != 0) {
        log_error("libusb_claim_interface failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        winwdf_destroy_object(&usb_if->object);
        return NULL;
    }
    usb_if->libusb_if = &usb_dev->cfg_descr->interface[interface_idx];

    //Create pipes
    const struct libusb_interface_descriptor *if_descrp = &usb_if->libusb_if->altsetting[0];

    usb_if->num_pipes = if_descrp->bNumEndpoints;
    usb_if->pipes = (struct wdf_usb_pipe*) malloc(usb_if->num_pipes * sizeof(struct wdf_usb_pipe));
    if(!usb_if->pipes) {
        perror("Couldn't allocate USB pipe object");
        winwdf_destroy_object(&usb_if->object);
        return NULL;
    }

    for(int i = 0; i < usb_if->num_pipes; i++) {
        struct wdf_usb_pipe *usb_pipe = &usb_if->pipes[i];

        wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &usb_pipe->object, wdf_cleanup_obj, NULL);
        usb_pipe->usb_if = usb_if;

        usb_pipe->pipe_idx = i;
        usb_pipe->libusb_ep = &if_descrp->endpoint[i];
    }

    //Link into USB device interface list
    cant_fail_ret(pthread_mutex_lock(&usb_dev->ifs_lock));

    usb_if->prev = NULL;
    usb_if->next = usb_dev->ifs_head;
    if(usb_dev->ifs_head) usb_dev->ifs_head->prev = usb_if;
    usb_dev->ifs_head = usb_if;

    cant_fail_ret(pthread_mutex_unlock(&usb_dev->ifs_lock));

    return &usb_if->object;
}
WDFFUNC(WdfUsbTargetDeviceGetInterface, 236)

__winfnc BYTE WdfUsbInterfaceGetConfiguredSettingIndex(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_if_obj) {
    return 0;
}
WDFFUNC(WdfUsbInterfaceGetConfiguredSettingIndex, 237)

__winfnc void WdfUsbInterfaceGetDescriptor(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_if_obj, UCHAR setting_idx, USB_INTERFACE_DESCRIPTOR *descriptor) {
    struct wdf_usb_interface *usb_if = (struct wdf_usb_interface*) usb_if_obj;

    *descriptor = (USB_INTERFACE_DESCRIPTOR) {0};
    if(setting_idx < usb_if->libusb_if->num_altsetting) memcpy(descriptor, &usb_if->libusb_if->altsetting[setting_idx], sizeof(USB_INTERFACE_DESCRIPTOR));
}
WDFFUNC(WdfUsbInterfaceGetDescriptor, 232)

__winfnc BYTE WdfUsbInterfaceGetNumEndpoints(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_if_obj, UCHAR setting_idx) {
    struct wdf_usb_interface *usb_if = (struct wdf_usb_interface*) usb_if_obj;
    if(setting_idx < usb_if->libusb_if->num_altsetting) return usb_if->libusb_if->altsetting[setting_idx].bNumEndpoints;
    else return 0;
}
WDFFUNC(WdfUsbInterfaceGetNumEndpoints, 231)

__winfnc WDFOBJECT WdfUsbInterfaceGetConfiguredPipe(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_if_obj, UCHAR idx, WDF_USB_PIPE_INFORMATION *info) {
    struct wdf_usb_interface *usb_if = (struct wdf_usb_interface*) usb_if_obj;

    //Get the pipe
    if(idx >= usb_if->num_pipes) return NULL;
    struct wdf_usb_pipe *usb_pipe = &usb_if->pipes[idx];

    //Fill in info
    if(info) {
        info->Size = sizeof(WDF_USB_PIPE_INFORMATION);
        info->MaximumPacketSize = usb_pipe->libusb_ep->wMaxPacketSize;
        info->EndpointAddress = usb_pipe->libusb_ep->bEndpointAddress;
        info->Interval = usb_pipe->libusb_ep->bInterval;
        info->SettingIndex = 0;
        switch(usb_pipe->libusb_ep->bmAttributes & 0b11) {
            case LIBUSB_ENDPOINT_TRANSFER_TYPE_CONTROL: info->PipeType = WdfUsbPipeTypeControl;
            case LIBUSB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS: info->PipeType = WdfUsbPipeTypeIsochronous;
            case LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK: info->PipeType = WdfUsbPipeTypeBulk;
            case LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT: info->PipeType = WdfUsbPipeTypeInterrupt;
        }
        info->MaximumTransferSize = 0;
    }

    return &usb_pipe->object;
}
WDFFUNC(WdfUsbInterfaceGetConfiguredPipe, 239)

__winfnc void WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_pipe_obj) {}
WDFFUNC(WdfUsbTargetPipeSetNoMaximumPacketSizeCheck, 220)

__winfnc NTSTATUS WdfUsbTargetPipeAbortSynchronously(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_pipe_obj, WDFOBJECT request, WDF_REQUEST_SEND_OPTIONS *req_opts) { return STATUS_SUCCESS; }
WDFFUNC(WdfUsbTargetPipeAbortSynchronously, 226)

__winfnc NTSTATUS WdfUsbTargetPipeResetSynchronously(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_pipe_obj, WDFOBJECT request, WDF_REQUEST_SEND_OPTIONS *req_opts) {
    struct wdf_usb_pipe *usb_pipe = (struct wdf_usb_pipe*) usb_pipe_obj;

    int usb_err;
    if((usb_err = libusb_clear_halt(usb_pipe->usb_if->usb_device->libusb_dev, usb_pipe->libusb_ep->bEndpointAddress)) != 0) {
        log_error("libusb_clear_halt failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetPipeResetSynchronously, 228)

struct transfer_req_ctx {
    struct wdf_memory *mem;
    WDFMEMORY_OFFSET mem_off;
    struct winwdf_request *request;
    void *ctrl_buf;
    void *padded_buf;  /* Padded buffer for interrupt IN (prevent overflow) */

    struct libusb_transfer *transfer;
};

#define USBD_STATUS_SUCCESS 0x00000000
#define USBD_STATUS_ERROR 0xc0000000

#define USBD_STATUS_ENDPOINT_HALTED 0xc0000030
#define USBD_STATUS_INTERNAL_HC_ERROR 0x80000900
#define USBD_STATUS_BUFFER_TOO_SMALL 0xc0003000
#define USBD_STATUS_TIMEOUT 0xc0006000
#define USBD_STATUS_DEVICE_GONE 0xc0007000

static void pipe_transfer_callback(struct libusb_transfer *transfer) {
    struct transfer_req_ctx *ctx = (struct transfer_req_ctx*) transfer->user_data;

    //Handle status
    if(transfer->status == LIBUSB_TRANSFER_CANCELLED) {
        wdf_complete_request(ctx->request, STATUS_CANCELLED, NULL);
        return;
    }

    log_info("USB CB: ep=0x%02x status=%d actual=%d/%d",
        transfer->endpoint, transfer->status, transfer->actual_length, transfer->length);

    /* If we used a padded buffer, copy received data back to the original buffer */
    if(ctx->padded_buf) {
        int copy_len = transfer->actual_length;
        if(copy_len > (int)ctx->mem_off.BufferLength) copy_len = (int)ctx->mem_off.BufferLength;
        if(copy_len > 0) memcpy(ctx->mem->data + ctx->mem_off.BufferOffset, ctx->padded_buf, copy_len);
        transfer->actual_length = copy_len;
        /* Don't free here — cleanup function handles it */
    }

    NTSTATUS status = STATUS_SUCCESS;
    if(transfer->status != LIBUSB_TRANSFER_COMPLETED) {
        log_warn("libusb transfer failed: %d [%s]", transfer->status, libusb_error_name(transfer->status));
        status = WINERR_SET_CODE;
    }

    //Complete the request
    WDF_USB_REQUEST_COMPLETION_PARAMS usb_params;

    usb_params.UsbdStatus = (status == STATUS_SUCCESS) ? USBD_STATUS_SUCCESS : USBD_STATUS_ERROR;
    switch(transfer->status) {
        case LIBUSB_TRANSFER_TIMED_OUT: usb_params.UsbdStatus |= USBD_STATUS_TIMEOUT; break;
        case LIBUSB_TRANSFER_STALL: usb_params.UsbdStatus |= USBD_STATUS_ENDPOINT_HALTED; break;
        case LIBUSB_TRANSFER_NO_DEVICE: usb_params.UsbdStatus |= USBD_STATUS_DEVICE_GONE; break;
        case LIBUSB_TRANSFER_OVERFLOW: usb_params.UsbdStatus |= USBD_STATUS_BUFFER_TOO_SMALL; break;
        case LIBUSB_TRANSFER_ERROR:
        default: usb_params.UsbdStatus |= USBD_STATUS_INTERNAL_HC_ERROR; break;
    }

    if(((transfer->endpoint & 0b10000000) == LIBUSB_ENDPOINT_IN)) {
        usb_params.Type = WdfUsbRequestTypePipeRead;
        usb_params.Parameters.PipeRead.Buffer = &ctx->mem->object;
        usb_params.Parameters.PipeRead.Length = transfer->actual_length;
        usb_params.Parameters.PipeRead.Offset = ctx->mem_off.BufferOffset;
    } else {
        usb_params.Type = WdfUsbRequestTypePipeWrite;
        usb_params.Parameters.PipeWrite.Buffer = &ctx->mem->object;
        usb_params.Parameters.PipeWrite.Length = transfer->actual_length;
        usb_params.Parameters.PipeWrite.Offset = ctx->mem_off.BufferOffset;
    }

    WDF_REQUEST_COMPLETION_PARAMS params;
    params.Size = sizeof(WDF_REQUEST_COMPLETION_PARAMS);
    params.Type = WdfRequestTypeUsb;
    params.IoStatus.Status = status;
    params.IoStatus.Information = transfer->actual_length;
    params.Parameters.Usb.Completion = &usb_params;

    if(ctx->ctrl_buf) {
        memcpy(ctx->mem->data + ctx->mem_off.BufferOffset, (BYTE*) ctx->ctrl_buf + sizeof(struct libusb_control_setup), ctx->mem_off.BufferLength);
    }

    wdf_complete_request(ctx->request, status, &params);
}

static NTSTATUS usb_transfer_start(struct winwdf_request *req, struct transfer_req_ctx *ctx, WDFOBJECT target, int timeout, void **data) {
    *data = NULL;

    //Initialize remaining transfer fields
    ctx->transfer->timeout = (timeout > 0) ? (unsigned int) timeout : 0;
    ctx->transfer->callback = (libusb_transfer_cb_fn) pipe_transfer_callback;
    ctx->transfer->user_data = ctx;

    ctx->transfer->buffer = ctx->ctrl_buf ? ctx->ctrl_buf : (ctx->mem->data + ctx->mem_off.BufferOffset);
    ctx->transfer->length = (ctx->ctrl_buf ? sizeof(struct libusb_control_setup) : 0) + (int) ctx->mem_off.BufferLength;

    /* Pad interrupt IN buffers to 64 bytes to prevent OVERFLOW.
     * The Kensington VeriMark sends 60-byte packets but the v104 driver
     * may request smaller buffers. We allocate a larger buffer for the USB
     * transfer and truncate on completion. */
    /* Pad interrupt IN buffers to prevent OVERFLOW from 60-byte sensor packets */
    if((ctx->transfer->endpoint & 0x80) && ctx->transfer->type == LIBUSB_TRANSFER_TYPE_INTERRUPT
       && ctx->transfer->length < 64 && !ctx->ctrl_buf) {
        ctx->padded_buf = malloc(64);
        if(ctx->padded_buf) {
            memset(ctx->padded_buf, 0, 64);
            ctx->transfer->buffer = ctx->padded_buf;
            ctx->transfer->length = 64;
        }
    }

    if(ctx->ctrl_buf) {
        memcpy((BYTE*) ctx->ctrl_buf + sizeof(struct libusb_control_setup), ctx->mem->data + ctx->mem_off.BufferOffset, ctx->mem_off.BufferLength);
    }

    //Log transfer details
    log_info("USB XFER: ep=0x%02x type=%d len=%d timeout=%d",
        ctx->transfer->endpoint, ctx->transfer->type,
        ctx->transfer->length, ctx->transfer->timeout);
    if(ctx->transfer->length > 0 && ctx->transfer->buffer && (ctx->transfer->endpoint & 0x80) == 0) {
        BYTE *b = ctx->transfer->buffer;
        log_info("USB XFER OUT data: [%02x %02x %02x %02x %02x %02x %02x %02x]",
            b[0], ctx->transfer->length>1?b[1]:0, ctx->transfer->length>2?b[2]:0,
            ctx->transfer->length>3?b[3]:0, ctx->transfer->length>4?b[4]:0,
            ctx->transfer->length>5?b[5]:0, ctx->transfer->length>6?b[6]:0,
            ctx->transfer->length>7?b[7]:0);
    }

    //Submit the transfer
    int usb_err;
    if((usb_err = libusb_submit_transfer(ctx->transfer)) != 0) {
        log_warn("libusb_submit_transfer failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return WINERR_SET_CODE;
    }

    return STATUS_SUCCESS;
}

static void usb_transfer_cancel(struct winwdf_request *req, struct transfer_req_ctx *ctx, void *data) {
    //Cancel the transfer
    int usb_err;
    if((usb_err = libusb_cancel_transfer(ctx->transfer)) != 0) {
        log_warn("libusb_cancel_transfer failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        abort();
    }
}

static void usb_transfer_cleanup(struct winwdf_request *req, struct transfer_req_ctx *ctx, void *data) {
    //Free memory
    libusb_free_transfer(ctx->transfer);
    free(ctx->ctrl_buf);
    free(ctx->padded_buf);
    free(ctx);
}

__winfnc NTSTATUS WdfUsbTargetDeviceFormatRequestForControlTransfer(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_dev_obj, WDFOBJECT req_obj, WDF_USB_CONTROL_SETUP_PACKET *packet, WDFOBJECT mem_obj, WDFMEMORY_OFFSET *mem_off) {
    struct wdf_usb_device *usb_dev = (struct wdf_usb_device*) usb_dev_obj;
    struct wdf_memory *mem = (struct wdf_memory*) mem_obj;
    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    //Create the context and transfer
    struct transfer_req_ctx *ctx = (struct transfer_req_ctx*) malloc(sizeof(struct transfer_req_ctx));
    if(!ctx) return winerr_from_errno();

    ctx->mem = mem;
    ctx->mem_off = mem_off ? *mem_off : ((WDFMEMORY_OFFSET) { .BufferOffset = 0, .BufferLength = mem->data_size });
    ctx->request = req;
    ctx->padded_buf = NULL;

    ctx->ctrl_buf = malloc(sizeof(struct libusb_control_setup) + ctx->mem_off.BufferLength);
    if(!ctx->ctrl_buf) { free(ctx); return winerr_from_errno(); }
    memcpy(ctx->ctrl_buf, packet, sizeof(struct libusb_control_setup));
    ((struct libusb_control_setup*) ctx->ctrl_buf)->wLength = (uint16_t) ctx->mem_off.BufferLength;
    
    ctx->transfer = libusb_alloc_transfer(0);
    if(!ctx->transfer) { return winerr_from_errno(); }
    ctx->transfer->dev_handle = usb_dev->libusb_dev;
    ctx->transfer->endpoint = 0;
    ctx->transfer->type = LIBUSB_TRANSFER_TYPE_CONTROL;
    ctx->transfer->flags = 0;

    //Configure the request
    wdf_configure_request(req, NULL, ctx, 0, 0, (wdf_request_start_fnc*) usb_transfer_start, (wdf_request_cancel_fnc*) usb_transfer_cancel, (wdf_request_cleanup_fnc*) usb_transfer_cleanup, NULL);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetDeviceFormatRequestForControlTransfer, 213)

__winfnc NTSTATUS WdfUsbTargetPipeFormatRequestForRead(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_pipe_obj, WDFOBJECT req_obj, WDFOBJECT mem_obj, WDFMEMORY_OFFSET *mem_off) {
    struct wdf_usb_pipe *usb_pipe = (struct wdf_usb_pipe*) usb_pipe_obj;
    struct wdf_memory *mem = (struct wdf_memory*) mem_obj;
    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    //Check the endpoint direction
    if((usb_pipe->libusb_ep->bEndpointAddress & 0b10000000) != LIBUSB_ENDPOINT_IN) {
        log_warn("Attempted USB read request for non-IN endpoint 0x%02x!", usb_pipe->libusb_ep->bEndpointAddress);
        return WINERR_SET_CODE;
    }

    //Determine the transfer type
    unsigned char transfer_type;
    switch(usb_pipe->libusb_ep->bmAttributes & 0b11) {
        case LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK: transfer_type = LIBUSB_TRANSFER_TYPE_BULK; break;
        case LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT: transfer_type = LIBUSB_TRANSFER_TYPE_INTERRUPT; break;
        default: {
            log_warn("Attempted USB read request for unsupported endpoint transfer type 0x%x!", usb_pipe->libusb_ep->bmAttributes & 0b11);
            return WINERR_SET_CODE;
        }
    }

    //Create the context and transfer
    struct transfer_req_ctx *ctx = (struct transfer_req_ctx*) malloc(sizeof(struct transfer_req_ctx));
    if(!ctx) return winerr_from_errno();

    ctx->mem = mem;
    ctx->mem_off = mem_off ? *mem_off : ((WDFMEMORY_OFFSET) { .BufferOffset = 0, .BufferLength = mem->data_size });
    ctx->request = req;
    ctx->ctrl_buf = NULL;
    ctx->padded_buf = NULL;

    ctx->transfer = libusb_alloc_transfer(0);
    if(!ctx->transfer) { return winerr_from_errno(); }

    ctx->transfer->dev_handle = usb_pipe->usb_if->usb_device->libusb_dev;
    ctx->transfer->endpoint = usb_pipe->libusb_ep->bEndpointAddress;
    ctx->transfer->type = transfer_type;
    ctx->transfer->flags = 0;

    //Configure the request
    wdf_configure_request(req, NULL, ctx, 0, 0, (wdf_request_start_fnc*) usb_transfer_start, (wdf_request_cancel_fnc*) usb_transfer_cancel, (wdf_request_cleanup_fnc*) usb_transfer_cleanup, NULL);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetPipeFormatRequestForRead, 224)

__winfnc NTSTATUS WdfUsbTargetPipeFormatRequestForWrite(WDF_DRIVER_GLOBALS *globals, WDFOBJECT usb_pipe_obj, WDFOBJECT req_obj, WDFOBJECT mem_obj, WDFMEMORY_OFFSET *mem_off) {
    struct wdf_usb_pipe *usb_pipe = (struct wdf_usb_pipe*) usb_pipe_obj;
    struct wdf_memory *mem = (struct wdf_memory*) mem_obj;
    struct winwdf_request *req = (struct winwdf_request*) req_obj;

    //Check the endpoint direction
    if((usb_pipe->libusb_ep->bEndpointAddress & 0b10000000) != LIBUSB_ENDPOINT_OUT) {
        log_warn("Attempted USB write request for non-OUT endpoint 0x%02x!", usb_pipe->libusb_ep->bEndpointAddress);
        return WINERR_SET_CODE;
    }

    //Determine the transfer type
    unsigned char transfer_type;
    switch(usb_pipe->libusb_ep->bmAttributes & 0b11) {
        case LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK: transfer_type = LIBUSB_TRANSFER_TYPE_BULK; break;
        case LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT: transfer_type = LIBUSB_TRANSFER_TYPE_INTERRUPT; break;
        default: {
            log_warn("Attempted USB write request for unsupported endpoint transfer type 0x%x!", usb_pipe->libusb_ep->bmAttributes & 0b11);
            return WINERR_SET_CODE;
        }
    }

    //Create the context and transfer
    struct transfer_req_ctx *ctx = (struct transfer_req_ctx*) malloc(sizeof(struct transfer_req_ctx));
    if(!ctx) return winerr_from_errno();

    ctx->mem = mem;
    ctx->mem_off = mem_off ? *mem_off : ((WDFMEMORY_OFFSET) { .BufferOffset = 0, .BufferLength = mem->data_size });
    ctx->request = req;
    ctx->ctrl_buf = NULL;
    ctx->padded_buf = NULL;

    ctx->transfer = libusb_alloc_transfer(0);
    if(!ctx->transfer) { return winerr_from_errno(); }

    ctx->transfer->dev_handle = usb_pipe->usb_if->usb_device->libusb_dev;
    ctx->transfer->endpoint = usb_pipe->libusb_ep->bEndpointAddress;
    ctx->transfer->type = transfer_type;
    ctx->transfer->flags = 0;

    //Configure the request
    wdf_configure_request(req, NULL, ctx, 0, 0, (wdf_request_start_fnc*) usb_transfer_start, (wdf_request_cancel_fnc*) usb_transfer_cancel, (wdf_request_cleanup_fnc*) usb_transfer_cleanup, NULL);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfUsbTargetPipeFormatRequestForWrite, 222)