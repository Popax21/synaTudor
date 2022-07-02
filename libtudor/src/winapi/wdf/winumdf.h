#ifndef LIBTUDOR_WINAPI_WDF_WINUMDF_H
#define LIBTUDOR_WINAPI_WDF_WINUMDF_H

#include "winapi/windows.h"

typedef struct _DRIVER_OBJECT DRIVER_OBJECT;
typedef struct _DEVICE_OBJECT DEVICE_OBJECT;
typedef struct _IRP IRP;

typedef void __winfnc DRIVER_INITIALIZE(DRIVER_OBJECT *DriverObject, UNICODE_STRING *reg_path);
typedef void __winfnc DRIVER_UNLOAD(DRIVER_OBJECT *DriverObject);
typedef void __winfnc DRIVER_STARTIO(DEVICE_OBJECT *DeviceObject, IRP *Irp);
typedef NTSTATUS __winfnc DRIVER_DISPATCH(DEVICE_OBJECT *DeviceObject, IRP *Irp);

typedef struct _DRIVER_OBJECT {
    SHORT Type;
    SHORT Size;
    DEVICE_OBJECT *DeviceObject;
    ULONG Flags;
    void *DriverStart;
    ULONG DriverSize;
    void *DriverSection;
    void *DriverExtension;
    UNICODE_STRING DriverName;
    UNICODE_STRING *HardwareDatabase;
    void *FastIoDispatch;
    DRIVER_INITIALIZE *DriverInit;
    DRIVER_STARTIO *DriverStartIo;
    DRIVER_UNLOAD *DriverUnload;
    DRIVER_DISPATCH *MajorFunction[0x1b + 1];
} DRIVER_OBJECT;

#endif