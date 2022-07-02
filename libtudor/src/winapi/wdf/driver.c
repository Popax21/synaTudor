#include <stdlib.h>
#include "internal.h"

typedef NTSTATUS __winfnc WDF_DRIVER_DEVICE_ADD(WDFOBJECT Driver, void *DeviceInit);
typedef void __winfnc WDF_DRIVER_UNLOAD(WDFOBJECT Driver);

typedef struct {
    ULONG Size;
    WDF_DRIVER_DEVICE_ADD *EvtDriverDeviceAdd;
    WDF_DRIVER_UNLOAD *EvtDriverUnload;
    ULONG DriverInitFlags;
    ULONG DriverPoolTag;
} WDF_DRIVER_CONFIG;

struct winwdf_driver {
    struct wdf_object object;

    WDF_DRIVER_GLOBALS *globals;
    WDF_DRIVER_CONFIG cfg;
    HANDLE reg_key;
};

struct winwdf_driver *winwdf_get_driver(WDF_DRIVER_GLOBALS *globals) {
    return (struct winwdf_driver*) globals->Driver;    
}

void winwdf_unload_driver(struct winwdf_driver *driver) {
    winwdf_destroy_object(&driver->object);
}

NTSTATUS winwdf_add_device(struct winwdf_driver *driver, HANDLE reg_key, libusb_device_handle *usb_dev, struct winwdf_device **dev) {
    if(dev) *dev = NULL;

    if(!driver->cfg.EvtDriverDeviceAdd) return EXIT_SUCCESS;

    //Add device
    struct wdf_device_init *dev_init = wdf_create_device_init(driver, reg_key, usb_dev, dev);
    return driver->cfg.EvtDriverDeviceAdd(&driver->object, dev_init);
}

static void driver_destr(struct winwdf_driver *driver) {
    //Call callbacks
    if(driver->cfg.EvtDriverUnload) driver->cfg.EvtDriverUnload(&driver->object);
    driver->globals->Driver = NULL;

    //Free memory
    wdf_cleanup_obj(&driver->object);
    winhandle_destroy(driver->reg_key);
    free(driver);
}

HANDLE wdf_get_driver_reg_key(struct winwdf_driver *driver) { return driver->reg_key; }

__winfnc NTSTATUS WdfDriverCreate(WDF_DRIVER_GLOBALS *globals, DRIVER_OBJECT *driver_obj, UNICODE_STRING *reg_path, WDF_OBJECT_ATTRIBUTES *attrs, WDF_DRIVER_CONFIG *cfg, WDFOBJECT *out) {
    if(cfg->Size < sizeof(*cfg)) { return ERROR_INSUFFICIENT_BUFFER; }

    //Allocate the driver object
    struct winwdf_driver *driver = (struct winwdf_driver*) malloc(sizeof(struct winwdf_driver));
    if(!driver) { return winerr_from_errno(); }

    wdf_create_obj(NULL, &driver->object, (wdf_obj_destr_fnc*) driver_destr, attrs);

    driver->globals = globals;
    driver->cfg = *cfg;
    driver->reg_key = winreg_open_key(driver, winstr_to_str(reg_path->Buffer));

    globals->Driver = driver;
    if(out) *out = &driver->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfDriverCreate, 57)