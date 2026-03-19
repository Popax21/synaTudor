/*
 * Minimal COM infrastructure for UMDF v1 driver hosting.
 * Implements just enough to bootstrap a Synaptics Tudor fingerprint driver.
 */

#ifndef LIBTUDOR_WINAPI_COM_H
#define LIBTUDOR_WINAPI_COM_H

#include <libusb.h>
#include "winapi/api.h"
#include "loader.h"

/* Standard COM result codes */
#define S_OK          0x00000000
#define S_FALSE       0x00000001
#define E_NOINTERFACE 0x80004002
#define E_POINTER     0x80004003
#define E_FAIL        0x80004005
#define CLASS_E_CLASSNOTAVAILABLE 0x80040111

/* COM GUID comparison */
static inline bool guid_eq(const GUID *a, const GUID *b) {
    return memcmp(a, b, sizeof(GUID)) == 0;
}

/*
 * COM_GUID: construct a GUID from the standard Windows GUID format.
 * {AAAAAAAA-BBBB-CCCC-DDEE-FFFFFFFFFFFF}
 * where D,E are the two bytes of PartD and F..F are the 6 bytes of PartE.
 * All parameters are plain integers (no 0x prefix needed in the macro).
 */
#define COM_GUID(a, b, c, d0,d1, e0,e1,e2,e3,e4,e5) ((GUID){ \
    .PartA = (a), .PartB = (b), .PartC = (c), \
    .PartD = ((d0) << 0) | ((d1) << 8), \
    .PartE = ((uint64_t)(e0) << 0) | ((uint64_t)(e1) << 8) | ((uint64_t)(e2) << 16) | ((uint64_t)(e3) << 24) | ((uint64_t)(e4) << 32) | ((uint64_t)(e5) << 40) \
})

/* Well-known IIDs */
#define IID_IUNKNOWN      COM_GUID(0x00000000,0x0000,0x0000, 0xC0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46)
#define IID_ICLASSFACTORY COM_GUID(0x00000001,0x0000,0x0000, 0xC0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46)

/* UMDF v1 interface IIDs (from wudfddi.h / Microsoft Learn) */
#define IID_IDRIVERENTRY           COM_GUID(0x1BEC7499,0x8881,0x4F2B, 0xB0,0x1C, 0xA1,0xA9,0x07,0x30,0x4A,0xFC)
#define IID_IWDFOBJECT             COM_GUID(0x64275C66,0x2E71,0x4060, 0xB5,0xF4, 0x3A,0x76,0xDF,0x96,0xED,0x3C)
#define IID_IWDFDRIVER             COM_GUID(0xAD368EBE,0x4139,0x43e3, 0xA8,0x75, 0x69,0xB2,0x66,0xA9,0x13,0x9C)
#define IID_IWDFDEVICEINITIALIZE   COM_GUID(0x74CC381C,0x0871,0x43c1, 0x87,0x8B, 0x3F,0x7C,0x9D,0x16,0x93,0x3D)
#define IID_IWDFDEVICE             COM_GUID(0xD657FE45,0x460A,0x49c3, 0x82,0x19, 0x76,0x6A,0xE8,0x03,0x2A,0x80)
#define IID_IWDFDEVICE2            COM_GUID(0x1B0D07B1,0xF596,0x4A76, 0x99,0xB3, 0xB9,0x49,0x2A,0xD8,0xDF,0x9F)
#define IID_IWDFDEVICE3            COM_GUID(0x863D943A,0xC9CD,0x4655, 0xA8,0xD6, 0xC8,0x4E,0xF4,0x11,0xCC,0x0D)
#define IID_IWDFIOTARGET           COM_GUID(0xb7615d26,0x494b,0x47a6, 0xb4,0xcf, 0x02,0x71,0xbc,0xc3,0xda,0x4b)
#define IID_IWDFIOQUEUE            COM_GUID(0xAE1162B9,0x8B11,0x4714, 0x99,0x3D, 0x93,0xDC,0x48,0xCC,0x9E,0x8A)
#define IID_IWDFIOREQUEST          COM_GUID(0x896DF312,0x22B4,0x4a9d, 0x95,0xDD, 0xA3,0x64,0xAA,0xF5,0x97,0x69)
#define IID_IWDFMEMORY             COM_GUID(0xCC060D79,0xC0C2,0x407f, 0x8B,0x10, 0xA5,0xE9,0x00,0xFC,0x34,0x74)
#define IID_IWDFFILE               COM_GUID(0xCC8FE04B,0xFE8B,0x4245, 0xAF,0xD6, 0xC3,0x1B,0xC8,0x30,0xC7,0x91)
#define IID_IPNPCALLBACK           COM_GUID(0x27C32374,0xCC45,0x4840, 0x85,0x7E, 0x8E,0x5E,0xF7,0xC0,0xEB,0xFF)
#define IID_IPNPCALLBACKHARDWARE   COM_GUID(0x51433BD3,0xC7C1,0x4bd8, 0xB4,0xC1, 0xAB,0x1E,0x03,0x46,0x26,0xCC)
#define IID_IPNPCALLBACKHARDWARE2  COM_GUID(0x652DBC55,0xC7EF,0x45E2, 0x9B,0x4D, 0x3A,0x07,0xC0,0x10,0xCB,0xBB)
#define IID_IQUEUECALLBACK_DEVIOCTL COM_GUID(0xC5411408,0x0F1E,0x4ed6, 0xA4,0x12, 0x36,0xDD,0x15,0xEE,0xE7,0x07)
#define IID_IOBJECTCLEANUP         COM_GUID(0x244ABE3A,0xABBC,0x43b1, 0xA8,0x77, 0xF0,0x00,0x77,0x55,0x0E,0x6A)
#define IID_IWDFUSBTARGETFACTORY   COM_GUID(0x3F7BECF9,0x3A65,0x4348, 0xA4,0xF3, 0x33,0x9D,0x57,0x34,0xA9,0xC6)
#define IID_IWDFUSBTARGETDEVICE    COM_GUID(0x4CD12E96,0x900A,0x44C3, 0xA1,0xB7, 0x05,0xB8,0x95,0x4D,0xAB,0x76)
#define IID_IWDFUSBINTERFACE       COM_GUID(0xA6714A4D,0xC764,0x498F, 0xB6,0xD2, 0x03,0x2F,0x10,0xCA,0x4E,0x36)
#define IID_IWDFUSBTARGETPIPE      COM_GUID(0xF373216E,0x8C42,0x4C5C, 0x95,0xB4, 0xD2,0xA4,0x0B,0x99,0xC0,0xFA)
#define IID_IWDFNAMEDPROPERTYSTORE COM_GUID(0x394B48C9,0xBCA0,0x498F, 0x8E,0x2C, 0x01,0x22,0x54,0x64,0xA9,0x32)
#define IID_IWDFPROPERTYSTOREFACTORY COM_GUID(0x45BE7E06,0x9B65,0x434d, 0xA7,0xD6, 0x95,0x72,0xD7,0xF7,0x3D,0x53)

/* Kensington/Synaptics driver CLSID */
#define CLSID_SYNATUDOR DEFINE_GUID(96710705, B080, 4B29, A3EC, B16935AE663A)

/*
 * COM vtable base pattern. Every COM object starts with a pointer to its vtable.
 * The vtable always begins with QueryInterface, AddRef, Release (IUnknown).
 */

/* Forward declarations */
typedef struct com_object com_object;
typedef struct com_vtable com_vtable;

/* IUnknown vtable (base for all COM interfaces) */
typedef HRESULT __winfnc (*com_QueryInterface_fn)(com_object *self, const GUID *riid, void **ppv);
typedef ULONG   __winfnc (*com_AddRef_fn)(com_object *self);
typedef ULONG   __winfnc (*com_Release_fn)(com_object *self);

struct com_vtable {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    /* Derived interfaces add more function pointers after these */
};

struct com_object {
    com_vtable *vtbl;
    ULONG ref_count;
    void *impl_data; /* implementation-specific data */
};

/* DllGetClassObject typedef */
typedef HRESULT __winfnc (*api_DllGetClassObject)(const GUID *rclsid, const GUID *riid, void **ppv);

/* IClassFactory vtable */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    HRESULT __winfnc (*CreateInstance)(com_object *self, void *pUnkOuter, const GUID *riid, void **ppv);
    HRESULT __winfnc (*LockServer)(com_object *self, BOOL fLock);
} IClassFactoryVtbl;

/* IDriverEntry vtable */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    HRESULT __winfnc (*OnInitialize)(com_object *self, com_object *pWdfDriver);
    HRESULT __winfnc (*OnDeviceAdd)(com_object *self, com_object *pWdfDriver, com_object *pDeviceInit);
    void    __winfnc (*OnDeinitialize)(com_object *self, com_object *pWdfDriver);
} IDriverEntryVtbl;

/* IPnpCallbackHardware vtable */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    HRESULT __winfnc (*OnPrepareHardware)(com_object *self, com_object *pDevice);
    HRESULT __winfnc (*OnReleaseHardware)(com_object *self, com_object *pDevice);
} IPnpCallbackHardwareVtbl;

/* IPnpCallbackHardware2 vtable (UMDF 1.11 — takes resource lists) */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    HRESULT __winfnc (*OnPrepareHardware)(com_object *self, com_object *pDevice, com_object *pRawResources, com_object *pTranslatedResources);
    HRESULT __winfnc (*OnReleaseHardware)(com_object *self, com_object *pDevice, com_object *pTranslatedResources);
} IPnpCallbackHardware2Vtbl;

extern com_object *com_pnp_hw2_callback; /* Driver's IPnpCallbackHardware2 */

/* IPnpCallback vtable */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    HRESULT __winfnc (*OnD0Entry)(com_object *self, com_object *pDevice, DWORD previousState);
    HRESULT __winfnc (*OnD0Exit)(com_object *self, com_object *pDevice, DWORD newState);
    void    __winfnc (*OnSurpriseRemoval)(com_object *self, com_object *pDevice);
    HRESULT __winfnc (*OnQueryRemove)(com_object *self, com_object *pDevice);
    HRESULT __winfnc (*OnQueryStop)(com_object *self, com_object *pDevice);
} IPnpCallbackVtbl;

/* IQueueCallbackDeviceIoControl vtable */
typedef struct {
    com_QueryInterface_fn QueryInterface;
    com_AddRef_fn AddRef;
    com_Release_fn Release;
    void __winfnc (*OnDeviceIoControl)(com_object *self, com_object *pQueue, com_object *pRequest, ULONG ControlCode, SIZE_T InBufSize, SIZE_T OutBufSize);
} IQueueCallbackDeviceIoControlVtbl;

/* Set the libusb handle for the COM USB target factory.
   Must be called before com_init_driver(). */
void com_set_usb_device(libusb_device_handle *dev);

/* Phase 1: COM bootstrap through OnDeviceAdd (creates driver object). */
bool com_init_driver(struct dll_image *driver_dll);

/* Phase 2: WBFUsbInitialize + OnPrepareHardware + OnD0Entry.
   Call after field_0x80 has been zeroed externally. */
bool com_finish_init(struct dll_image *driver_dll);

void com_shutdown_driver(void);

/* Route an IOCTL through the COM IQueueCallbackDeviceIoControl path */
NTSTATUS com_send_ioctl(ULONG code, const void *in_buf, size_t in_size, void *out_buf, size_t out_size, size_t *bytes_returned);

/* The {1493cd1b...} interface = CBiometricDeviceUSB base pointer */
extern void *com_usb_device_obj;

/* Access to driver-provided callback interfaces (set during OnDeviceAdd) */
extern com_object *com_driver_callback;  /* The driver's IDriverEntry */
extern com_object *com_pnp_hw_callback;  /* Driver's IPnpCallbackHardware */
extern com_object *com_pnp_callback;     /* Driver's IPnpCallback */
extern com_object *com_ioctl_callback;   /* Driver's IQueueCallbackDeviceIoControl */

#endif
