#ifndef LIBTUDOR_WINAPI_WDF_WINWDF_H
#define LIBTUDOR_WINAPI_WDF_WINWDF_H

#include "winumdf.h"

typedef struct wdf_object *WDFOBJECT;

typedef struct _WDF_VERSION {
    ULONG Major, Minor, Build;
} WDF_VERSION;

typedef struct {
    ULONG Size;
    char16_t *Component;
    WDF_VERSION Version;
    ULONG FuncCount;
    void ***FuncTable;
    void *Module;
} WDF_BIND_INFO;

typedef NTSTATUS __winfnc WUDF_VERSION_BIND(void *Context, WDF_BIND_INFO *BindInfo, void **ComponentGlobals);

typedef struct {
    ULONG Size;
    WUDF_VERSION_BIND *VersionBind;
    void *Pad1, *Pad2, *Pad3, *Pad4;
    DWORD Flags, Pad5;
} WUDF_LOADER_FX_INTERFACE;

typedef struct {
    void *Driver;
    ULONG DriverFlags;
    ULONG DriverTag;
    CHAR DriverName[32];
    BOOL DisplaceDriverUnload;
} WDF_DRIVER_GLOBALS;

typedef struct _WDF_OBJECT_CONTEXT_TYPE_INFO WDF_OBJECT_CONTEXT_TYPE_INFO;

typedef WDF_OBJECT_CONTEXT_TYPE_INFO* __cdecl WDF_GET_UNIQUE_CONTEXT_TYPE();

typedef void __winfnc WDF_OBJECT_CONTEXT_CLEANUP(WDFOBJECT Object);
typedef void __winfnc WDF_OBJECT_CONTEXT_DESTROY(WDFOBJECT Object);

typedef struct _WDF_OBJECT_CONTEXT_TYPE_INFO {
    ULONG Size;
    const char *ContextName;
    size_t ContextSize;
    WDF_OBJECT_CONTEXT_TYPE_INFO *UniqueType;
    WDF_GET_UNIQUE_CONTEXT_TYPE *EvtDriverGetUniqueContextType;
} WDF_OBJECT_CONTEXT_TYPE_INFO;

typedef struct _WDF_OBJECT_ATTRIBUTES {
    ULONG Size;
    WDF_OBJECT_CONTEXT_CLEANUP *EvtCleanupCallback;
    WDF_OBJECT_CONTEXT_DESTROY *EvtDestroyCallback;
    ULONG ExecutionLevel;
    ULONG SynchronizationScope;
    WDFOBJECT ParentObject;
    size_t ContextSizeOverride;
    WDF_OBJECT_CONTEXT_TYPE_INFO *ContextTypeInfo;
} WDF_OBJECT_ATTRIBUTES;

typedef struct {
    size_t BufferOffset;
    size_t BufferLength;
} WDFMEMORY_OFFSET;

typedef union {
    struct {
        union {
            struct {
                BYTE Recipient : 2;
                BYTE Reserved : 3;
                BYTE Type : 2;
                BYTE Dir : 1;
            } Request;
            BYTE Byte;
        } bm;
        BYTE bRequest;
        union {
            struct {
                BYTE LowByte;
                BYTE HiByte;
            } Bytes;
            USHORT Value;
        } wValue;
        union {
            struct {
                BYTE LowByte;
                BYTE HiByte;
            } Bytes;
            USHORT Value;
        } wIndex;
        USHORT wLength;
    } Packet;
    struct {
        BYTE Bytes[8];
    } Generic;
} WDF_USB_CONTROL_SETUP_PACKET;

typedef enum {
    WdfUsbRequestTypeInvalid,
    WdfUsbRequestTypeNoFormat,
    WdfUsbRequestTypeDeviceString,
    WdfUsbRequestTypeDeviceControlTransfer,
    WdfUsbRequestTypeDeviceUrb,
    WdfUsbRequestTypePipeWrite,
    WdfUsbRequestTypePipeRead,
    WdfUsbRequestTypePipeAbort,
    WdfUsbRequestTypePipeReset,
    WdfUsbRequestTypePipeUrb
} WDF_USB_REQUEST_TYPE;

typedef struct {
    ULONG UsbdStatus;
    ULONG Type;
    union {
        struct {
            WDFOBJECT Buffer;
            USHORT LangID;
            UCHAR StringIndex;
            UCHAR RequiredSize;
        } DeviceString;
        struct {
            WDFOBJECT Buffer;
            WDF_USB_CONTROL_SETUP_PACKET SetupPacket;
            ULONG Length;
        } DeviceControlTransfer;
        struct {
            WDFOBJECT Buffer;
        } DeviceUrb;
        struct {
            WDFOBJECT Buffer;
            size_t Length;
            size_t Offset;
        } PipeWrite;
        struct {
            WDFOBJECT Buffer;
            size_t Length;
            size_t Offset;
        } PipeRead;
        struct {
            WDFOBJECT Buffer;
        } PipeUrb;
    } Parameters;
} WDF_USB_REQUEST_COMPLETION_PARAMS;

typedef struct {
    union {
        NTSTATUS Status;
        void *Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK;

typedef enum {
    WdfRequestTypeCreate,
    WdfRequestTypeCreateNamedPipe,
    WdfRequestTypeClose,
    WdfRequestTypeRead,
    WdfRequestTypeWrite,
    WdfRequestTypeQueryInformation,
    WdfRequestTypeSetInformation,
    WdfRequestTypeQueryEA,
    WdfRequestTypeSetEA,
    WdfRequestTypeFlushBuffers,
    WdfRequestTypeQueryVolumeInformation,
    WdfRequestTypeSetVolumeInformation,
    WdfRequestTypeDirectoryControl,
    WdfRequestTypeFileSystemControl,
    WdfRequestTypeDeviceControl,
    WdfRequestTypeDeviceControlInternal,
    WdfRequestTypeShutdown,
    WdfRequestTypeLockControl,
    WdfRequestTypeCleanup,
    WdfRequestTypeCreateMailSlot,
    WdfRequestTypeQuerySecurity,
    WdfRequestTypeSetSecurity,
    WdfRequestTypePower,
    WdfRequestTypeSystemControl,
    WdfRequestTypeDeviceChange,
    WdfRequestTypeQueryQuota,
    WdfRequestTypeSetQuota,
    WdfRequestTypePnp,
    WdfRequestTypeOther,
    WdfRequestTypeUsb,
    WdfRequestTypeNoFormat,
    WdfRequestTypeMax
} WDF_REQUEST_TYPE;

typedef struct {
    USHORT Size;
    UCHAR MinorFunction;
    WDF_REQUEST_TYPE Type;
    union {
        struct {
            void *SecurityContext;
            ULONG Options;
            USHORT __align(8) FileAttributes;
            USHORT ShareAccess;
            ULONG __align(8) EaLength;
        } Create;
        struct {
            size_t Length;
            ULONG __align(8) Key;
            LONGLONG DeviceOffset;
        } Read;
        struct {
            size_t Length;
            ULONG __align(8) Key;
            LONGLONG DeviceOffset;
        } Write;
        struct {
            size_t OutputBufferLength;
            size_t __align(8) InputBufferLength;
            ULONG __align(8) IoControlCode;
            void *Type3InputBuffer;
        } DeviceIoControl;
        struct {
            void *Arg1;
            void *Arg2;
            ULONG __align(8) IoControlCode;
            void *Arg4;
        } Others;
    } Parameters;
} WDF_REQUEST_PARAMETERS;

typedef struct  {
    ULONG Size;
    WDF_REQUEST_TYPE Type;
    IO_STATUS_BLOCK IoStatus;
    union {
        struct {
            WDFOBJECT Buffer;
            size_t Length;
            size_t Offset;
        } Write;
        struct {
            WDFOBJECT Buffer;
            size_t Length;
            size_t Offset;
        } Read;
        struct {
            ULONG IoControlCode;
            struct {
                WDFOBJECT Buffer;
                size_t Offset;
            } Input;
            struct {
                WDFOBJECT Buffer;
                size_t Offset;
                size_t Length;
            } Output;
        } Ioctl;
        struct {
            union {
                void *Ptr;
                ULONG_PTR Value;
            } Argument1;
            union {
                void *Ptr;
                ULONG_PTR Value;
            } Argument2;
            union {
                void *Ptr;
                ULONG_PTR Value;
            } Argument3;
            union {
                void *Ptr;
                ULONG_PTR Value;
            } Argument4;
        } Others;
        struct {
            WDF_USB_REQUEST_COMPLETION_PARAMS *Completion;
        } Usb;
    } Parameters;
} WDF_REQUEST_COMPLETION_PARAMS;

typedef void __winfnc WDF_DEVICE_FILE_CREATE(WDFOBJECT Device, WDFOBJECT Request, WDFOBJECT FileObject);
typedef void __winfnc WDF_FILE_CLOSE(WDFOBJECT FileObject);
typedef void __winfnc WDF_FILE_CLEANUP(WDFOBJECT FileObject);

typedef struct {
    ULONG Size;
    WDF_DEVICE_FILE_CREATE *EvtDeviceFileCreate;
    WDF_FILE_CLOSE *EvtFileClose;
    WDF_FILE_CLEANUP *EvtFileCleanup;
    ULONG AutoForwardCleanupClose;
    ULONG FileObjectClass;
} WDF_FILEOBJECT_CONFIG;

typedef NTSTATUS __winfnc (*api_FxDriverEntryUm)(WUDF_LOADER_FX_INTERFACE *loader, void *contex, DRIVER_OBJECT *driver, UNICODE_STRING *reg_path);

#endif