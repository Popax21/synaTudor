#ifndef TUDOR_WINAPI_WDF_WDF_H
#define TUDOR_WINAPI_WDF_WDF_H

#include <libusb.h>
#include "winwdf.h"

//Functions
#define NUM_WDF_FUNCS 261
extern void *wdf_functions[NUM_WDF_FUNCS];
#define WDFFUNC(fnc, idx) __constr static void __init_wdf_fnc##idx() { wdf_functions[idx] = &fnc; }

//Event queue
void winwdf_event_queue_flush();

//Objects
void winwdf_destroy_object(WDFOBJECT obj);

//Drivers
struct winwdf_driver;
struct winwdf_driver *winwdf_get_driver(WDF_DRIVER_GLOBALS *globals);
void winwdf_unload_driver(struct winwdf_driver *driver);

//Devices
struct winwdf_device;
NTSTATUS winwdf_add_device(struct winwdf_driver *driver, HANDLE reg_key, libusb_device_handle *usb_dev, struct winwdf_device **dev);
void winwdf_remove_device(struct winwdf_device *device);

//Files
struct winwdf_file;
NTSTATUS winwdf_open_device(struct winwdf_device *device, struct winwdf_file **file);
void winwdf_close_file(struct winwdf_file *file);

//TODO NTSTATUS winwdf_read_file(struct winwdf_file *file, void *buf, size_t *size);
//TODO NTSTATUS winwdf_write_file(struct winwdf_file *file, const void *buf, size_t size);
NTSTATUS winwdf_devctrl_file(struct winwdf_file *file, ULONG code, const void *in_buf, size_t in_size, void *out_buf, size_t *out_size);

#endif