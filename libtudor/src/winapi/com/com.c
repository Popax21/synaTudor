/*
 * UMDF v1 COM host — minimal implementation for Synaptics Tudor drivers.
 *
 * Implements mock IWDFDriver, IWDFDevice, IWDFDeviceInitialize, and
 * IWDFUsbTargetFactory objects for the COM bootstrap path.
 * USB I/O flows through the existing DeviceIoControl/ReadFile/WriteFile shims.
 */

#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <time.h>
/* Note: mprotect must NOT be called from this file — causes mysterious crashes */
#include "com.h"
#include "winapi/api.h"
#include "loader.h"
#include "tudor/internal.h"

com_object *com_driver_callback = NULL;
void *com_usb_device_obj = NULL;
com_object *com_pnp_hw_callback = NULL;
com_object *com_pnp_hw2_callback = NULL;
com_object *com_pnp_callback = NULL;
com_object *com_ioctl_callback = NULL;

static libusb_device_handle *com_libusb_dev = NULL;

/* Reference to the loaded driver DLL module (for winmodule_set_cur) */
extern struct windrv_dll *tudor_driver_dll;

void com_set_usb_device(libusb_device_handle *dev) {
    com_libusb_dev = dev;
}

/* ─── Generic helpers ─────────────────────────────────────────────── */

static __winfnc ULONG stub_addref(com_object *self) { return ++self->ref_count; }
static __winfnc ULONG stub_release(com_object *self) {
    if(self->ref_count > 0) self->ref_count--;
    return self->ref_count;
}

/* Catch-all for any COM method that shouldn't be called but might be.
   Uses __builtin_return_address to help identify which vtable slot was called. */
static __winfnc HRESULT com_method_stub() {
    void *caller = __builtin_return_address(0);
    log_warn("COM: Unimplemented COM method called! (caller %p)", caller);
    return E_NOTIMPL;
}
static __winfnc void com_void_stub() {
    void *caller = __builtin_return_address(0);
    log_warn("COM: Unimplemented void COM method called! (caller %p)", caller);
}

static __winfnc HRESULT wdfobj_delete(com_object *self) { return S_OK; }
static __winfnc HRESULT wdfobj_assign_ctx(com_object *self, void *cleanup, void *ctx) {
    self->impl_data = ctx;
    return S_OK;
}
static __winfnc HRESULT wdfobj_retrieve_ctx(com_object *self, void **ctx) {
    *ctx = self->impl_data;
    return S_OK;
}
static __winfnc void wdfobj_noop(com_object *self) {}

/* Log a GUID for debugging */
static void log_guid(const char *label, const GUID *g) {
    log_debug("COM: %s {%08x-%04x-%04x-...}", label, g->PartA, g->PartB, g->PartC);
}

/* ─── IWDFDeviceInitialize ────────────────────────────────────────── */

static __winfnc HRESULT devinit_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID iid_unk = IID_IUNKNOWN;
    GUID iid_devinit = IID_IWDFDEVICEINITIALIZE;
    if(guid_eq(riid, &iid_unk) || guid_eq(riid, &iid_devinit)) {
        *ppv = self; self->ref_count++; return S_OK;
    }
    log_guid("DeviceInit::QI unknown", riid);
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc void devinit_set_filter(com_object *self) { log_debug("COM: DeviceInit::SetFilter"); }
static __winfnc void devinit_set_locking(com_object *self, DWORD c) { log_debug("COM: DeviceInit::SetLockingConstraint(%u)", c); }
/* Forward declaration */
static com_object g_propstore;

static __winfnc HRESULT devinit_prop_store(com_object *self, const char16_t *s, DWORD f, void **store, DWORD *d) {
    log_debug("COM: DeviceInit::RetrieveDevicePropertyStore");
    if(store) *store = &g_propstore;
    if(d) *d = 1;
    g_propstore.ref_count++;
    return S_OK;
}
static __winfnc void devinit_power_ownership(com_object *self, BOOL o) { log_debug("COM: DeviceInit::SetPowerPolicyOwnership(%d)", o); }
static __winfnc void devinit_auto_forward(com_object *self, DWORD s) {}
static __winfnc HRESULT devinit_instance_id(com_object *self, char16_t *buf, DWORD *size) {
    const char *id = "USB\\VID_047D&PID_00F2\\TUDOR0";
    if(!buf) { *size = strlen(id) + 1; return S_OK; }
    char16_t *wid = winstr_from_str(id);
    int len = winstr_len(wid) + 1;
    if(*size < (DWORD)len) { free(wid); *size = len; return E_INVALIDARG; }
    memcpy(buf, wid, len * sizeof(char16_t));
    *size = len;
    free(wid);
    return S_OK;
}
static __winfnc void devinit_set_pnp_cap(com_object *self, DWORD c, DWORD v) {}
static __winfnc DWORD devinit_get_pnp_cap(com_object *self, DWORD c) { return 0; }

static void *g_devinit_vtbl[] = {
    devinit_qi, stub_addref, stub_release,
    devinit_set_filter, devinit_set_locking, devinit_prop_store,
    devinit_power_ownership, devinit_auto_forward, devinit_instance_id,
    devinit_set_pnp_cap, devinit_get_pnp_cap,
};

/* ─── IWDFUsbTargetFactory (separate object from device) ──────────── */

static com_object g_usb_target_factory;

static __winfnc HRESULT usb_factory_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID iid_unk = IID_IUNKNOWN;
    GUID iid_factory = IID_IWDFUSBTARGETFACTORY;
    if(guid_eq(riid, &iid_unk) || guid_eq(riid, &iid_factory)) {
        *ppv = self; self->ref_count++; return S_OK;
    }
    log_guid("UsbTargetFactory::QI unknown", riid);
    *ppv = NULL;
    return E_NOINTERFACE;
}

/* ─── IWDFUsbTargetDevice (minimal, backed by libusb) ─────────────── */

static com_object g_usb_target_device;

static __winfnc HRESULT usb_target_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFUSBTARGETDEVICE, IID_IWDFIOTARGET };
    for(int i = 0; i < 4; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    log_guid("UsbTarget::QI unknown", riid);
    *ppv = NULL;
    return E_NOINTERFACE;
}

/* IWDFIoTarget methods (slots 8-12 in IWDFUsbTargetDevice, inherited from IWDFIoTarget) */
static __winfnc void usb_target_get_file(com_object *self, void **f) { *f = NULL; }
static __winfnc void usb_target_cancel_sent(com_object *self, void *f) {}

/* IWDFUsbTargetDevice-specific methods */
/* WinUSB handles must be 32-bit-safe (driver checks with test eax,eax).
   Use a small sentinel value and map it to the real libusb handle in WinUsb_* functions. */
#define WINUSB_HANDLE_SENTINEL ((void*)(uintptr_t)0x57494E55) /* "WINU" */

static __winfnc void *usb_target_get_winusb_handle(com_object *self) {
    log_debug("COM: IWDFUsbTargetDevice::GetWinUsbHandle → 0x%x", (unsigned)(uintptr_t)WINUSB_HANDLE_SENTINEL);
    return WINUSB_HANDLE_SENTINEL;
}

static __winfnc UCHAR usb_target_get_num_interfaces(com_object *self) {
    if(!com_libusb_dev) return 0;
    struct libusb_config_descriptor *cfg;
    if(libusb_get_config_descriptor(libusb_get_device(com_libusb_dev), 0, &cfg) != 0) return 0;
    UCHAR n = cfg->bNumInterfaces;
    libusb_free_config_descriptor(cfg);
    log_debug("COM: IWDFUsbTargetDevice::GetNumInterfaces → %d", n);
    return n;
}

/* ─── IWDFUsbInterface (minimal) ───────────────────────────────── */

static com_object g_usb_interface_objs[4]; /* up to 4 interfaces */

static __winfnc HRESULT usb_iface_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFUSBINTERFACE };
    for(int i = 0; i < 3; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    log_guid("UsbInterface::QI unknown", riid);
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc UCHAR usb_iface_get_iface_number(com_object *self) {
    UCHAR idx = (UCHAR)(uintptr_t)self->impl_data;
    log_debug("COM: IWDFUsbInterface::GetInterfaceNumber → %d", idx);
    return idx;
}

static __winfnc UCHAR usb_iface_get_num_endpoints(com_object *self) {
    UCHAR idx = (UCHAR)(uintptr_t)self->impl_data;
    if(!com_libusb_dev) return 0;
    struct libusb_config_descriptor *cfg;
    if(libusb_get_config_descriptor(libusb_get_device(com_libusb_dev), 0, &cfg) != 0) return 0;
    UCHAR n = 0;
    if(idx < cfg->bNumInterfaces && cfg->interface[idx].num_altsetting > 0)
        n = cfg->interface[idx].altsetting[0].bNumEndpoints;
    libusb_free_config_descriptor(cfg);
    log_debug("COM: IWDFUsbInterface::GetNumEndPoints(%d) → %d", idx, n);
    return n;
}

static __winfnc void *usb_iface_get_winusb_handle(com_object *self) {
    log_debug("COM: IWDFUsbInterface::GetWinUsbHandle → 0x%x", (unsigned)(uintptr_t)WINUSB_HANDLE_SENTINEL);
    return WINUSB_HANDLE_SENTINEL;
}

static __winfnc HRESULT usb_iface_get_descriptor(com_object *self, void *desc, void *len) {
    UCHAR idx = (UCHAR)(uintptr_t)self->impl_data;
    if(!com_libusb_dev) return E_FAIL;
    struct libusb_config_descriptor *cfg;
    if(libusb_get_config_descriptor(libusb_get_device(com_libusb_dev), 0, &cfg) != 0) return E_FAIL;
    if(idx < cfg->bNumInterfaces && cfg->interface[idx].num_altsetting > 0) {
        memcpy(desc, &cfg->interface[idx].altsetting[0], 9);
    }
    libusb_free_config_descriptor(cfg);
    return S_OK;
}

static __winfnc HRESULT usb_iface_retrieve_pipe(com_object *self, UCHAR pipe_idx, void **ppPipe) {
    log_debug("COM: IWDFUsbInterface::RetrieveUsbPipeObject(%d) — stub", pipe_idx);
    *ppPipe = NULL;
    return E_NOTIMPL;
}

static __winfnc HRESULT usb_iface_select_setting(com_object *self, UCHAR setting) {
    log_debug("COM: IWDFUsbInterface::SelectSetting(%d)", setting);
    return S_OK;
}

static __winfnc HRESULT usb_iface_noop_target() {
    log_debug("COM: IWDFUsbInterface::IoTarget method (stub)");
    return S_OK;
}
static __winfnc UCHAR usb_iface_get_configured_setting(com_object *self) {
    return 0;
}

/* USB interface vtable — IUnknown + IWDFObject + IWDFUsbInterface */
static void *g_usb_iface_vtbl[] = {
    /* IUnknown (0-2) */
    [0]  = usb_iface_qi,
    [1]  = stub_addref,
    [2]  = stub_release,
    /* IWDFObject (3-7) */
    [3]  = wdfobj_delete,
    [4]  = wdfobj_assign_ctx,
    [5]  = wdfobj_retrieve_ctx,
    [6]  = wdfobj_noop,
    [7]  = wdfobj_noop,
    /* IWDFUsbInterface (8+) — no IWDFIoTarget for interfaces */
    [8]  = usb_iface_get_descriptor,       /* GetInterfaceDescriptor */
    [9]  = usb_iface_get_iface_number,     /* GetInterfaceNumber */
    [10] = usb_iface_get_num_endpoints,    /* GetNumEndPoints */
    [11] = usb_iface_retrieve_pipe,        /* RetrieveUsbPipeObject */
    [12] = usb_iface_get_winusb_handle,    /* GetWinUsbHandle */
    [13] = usb_iface_select_setting,       /* SelectSetting */
    [14] = usb_iface_get_configured_setting, /* GetConfiguredSettingIndex */
    /* Padding */
    [15] = usb_iface_noop_target,
    [16] = usb_iface_noop_target,
    [17] = usb_iface_noop_target,
    [18] = usb_iface_noop_target,
};

static __winfnc HRESULT usb_target_retrieve_interface(com_object *self, UCHAR idx, void **ppIface) {
    if(!com_libusb_dev) { *ppIface = NULL; return E_FAIL; }
    struct libusb_config_descriptor *cfg;
    if(libusb_get_config_descriptor(libusb_get_device(com_libusb_dev), 0, &cfg) != 0) {
        *ppIface = NULL; return E_FAIL;
    }
    if(idx >= cfg->bNumInterfaces) {
        libusb_free_config_descriptor(cfg);
        *ppIface = NULL; return E_INVALIDARG;
    }
    libusb_free_config_descriptor(cfg);

    /* Return a USB interface object */
    if(idx < 4) {
        g_usb_interface_objs[idx].vtbl = (com_vtable*)g_usb_iface_vtbl;
        g_usb_interface_objs[idx].ref_count = 1;
        g_usb_interface_objs[idx].impl_data = (void*)(uintptr_t)idx;
        *ppIface = &g_usb_interface_objs[idx];
        log_info("COM: IWDFUsbTargetDevice::RetrieveUsbInterface(%d) → OK", idx);
        return S_OK;
    }
    *ppIface = NULL;
    return E_INVALIDARG;
}

/* USB target device methods that need real implementations */
static __winfnc HRESULT usb_target_format_ctrl(com_object *self, void *request, void *setup, void *memory, void *offset) {
    log_debug("COM: IWDFUsbTargetDevice::FormatRequestForControlTransfer (stub)");
    return S_OK;
}

static __winfnc HRESULT usb_target_retrieve_dev_info(com_object *self, ULONG info_type, ULONG *value) {
    log_debug("COM: IWDFUsbTargetDevice::RetrieveDeviceInformation(type=%u)", info_type);
    /* DEVICE_SPEED=1: 1=low, 2=full, 3=high */
    if(value) *value = 2; /* Full speed (matching lsusb: 12Mbps) */
    return S_OK;
}

static __winfnc HRESULT usb_target_retrieve_descriptor(com_object *self, void *desc_params, void *memory, void *offset) {
    log_debug("COM: IWDFUsbTargetDevice::RetrieveDescriptor (stub)");
    return S_OK;
}

static __winfnc HRESULT usb_target_retrieve_power_policy(com_object *self, ULONG policy, ULONG *len, void *value) {
    log_debug("COM: IWDFUsbTargetDevice::RetrievePowerPolicy(%u)", policy);
    if(value && len && *len >= 4) *(ULONG*)value = 0;
    return S_OK;
}

static __winfnc HRESULT usb_target_set_power_policy(com_object *self, ULONG policy, ULONG len, void *value) {
    log_debug("COM: IWDFUsbTargetDevice::SetPowerPolicy(%u)", policy);
    return S_OK;
}

/* IWDFIoTarget::GetTargetFile — the driver calls this to get the underlying file handle. */
static com_object g_wdf_device; /* forward decl — defined below */
static __winfnc void usb_target_get_target_file(com_object *self, com_object **ppFile) {
    log_info("COM: IWDFIoTarget::GetTargetFile called on USB target");
    if(ppFile) *ppFile = &g_wdf_device;
}

static __winfnc void usb_target_cancel_for_file(com_object *self, com_object *file) {
    log_debug("COM: IWDFIoTarget::CancelSentRequestsForFile");
}

/* USB target device vtable — IUnknown + IWDFObject + IWDFIoTarget + IWDFUsbTargetDevice.
   Confirmed by disassembly: slot 17 (offset 0x88) = RetrieveDeviceInformation. */
static void *g_usb_target_vtbl[] = {
    /* IUnknown (0-2) */
    [0]  = usb_target_qi,
    [1]  = stub_addref,
    [2]  = stub_release,
    /* IWDFObject (3-7) */
    [3]  = wdfobj_delete,
    [4]  = wdfobj_assign_ctx,
    [5]  = wdfobj_retrieve_ctx,
    [6]  = wdfobj_noop,           /* AcquireLock */
    [7]  = wdfobj_noop,           /* ReleaseLock */
    /* IWDFIoTarget (8-12) */
    [8]  = usb_target_get_target_file,  /* GetTargetFile(self, &ppFile) */
    [9]  = usb_target_cancel_for_file,  /* CancelSentRequestsForFile */
    [10] = usb_target_format_ctrl,      /* FormatRequestForRead */
    [11] = usb_target_format_ctrl,      /* FormatRequestForWrite */
    [12] = usb_target_format_ctrl,      /* FormatRequestForIoctl */
    /* IWDFUsbTargetDevice (13-20) */
    [13] = usb_target_get_winusb_handle,       /* GetWinUsbHandle */
    [14] = usb_target_get_num_interfaces,      /* GetNumInterfaces */
    [15] = usb_target_retrieve_interface,      /* RetrieveUsbInterface */
    [16] = usb_target_format_ctrl,             /* FormatRequestForControlTransfer */
    [17] = usb_target_retrieve_dev_info,       /* RetrieveDeviceInformation */
    [18] = usb_target_retrieve_descriptor,     /* RetrieveDescriptor */
    [19] = usb_target_retrieve_power_policy,   /* RetrievePowerPolicy */
    [20] = usb_target_set_power_policy,        /* SetPowerPolicy */
    /* Extra padding (21+) */
    [21] = usb_target_set_power_policy,
    [22] = usb_target_set_power_policy,
    [23] = usb_target_set_power_policy,
    [24] = usb_target_set_power_policy,
};

static com_object g_usb_target_device = {
    .vtbl = (com_vtable*)g_usb_target_vtbl,
    .ref_count = 1,
    .impl_data = NULL,
};

static __winfnc HRESULT usb_factory_create_target(com_object *self, com_object **ppDevice) {
    log_info("COM: IWDFUsbTargetFactory::CreateUsbTargetDevice called");
    if(!com_libusb_dev) {
        log_error("COM: No USB device handle set — call com_set_usb_device() first");
        *ppDevice = NULL;
        return E_FAIL;
    }

    /* Detach kernel drivers and claim interfaces */
    struct libusb_config_descriptor *cfg;
    int usb_err;
    if((usb_err = libusb_get_config_descriptor(libusb_get_device(com_libusb_dev), 0, &cfg)) == 0) {
        log_info("COM: USB config has %d interfaces", cfg->bNumInterfaces);
        for(int i = 0; i < cfg->bNumInterfaces; i++) {
            int active = libusb_kernel_driver_active(com_libusb_dev, i);
            if(active == 1) {
                log_info("COM: Detaching kernel driver from interface %d", i);
                int det_err = libusb_detach_kernel_driver(com_libusb_dev, i);
                if(det_err) log_warn("COM: detach failed: %s", libusb_error_name(det_err));
            }
            usb_err = libusb_claim_interface(com_libusb_dev, i);
            if(usb_err != 0) {
                log_error("COM: libusb_claim_interface(%d) FAILED: %s", i, libusb_error_name(usb_err));
            } else {
                log_info("COM: Claimed USB interface %d", i);
            }
        }
        libusb_free_config_descriptor(cfg);
    } else {
        log_error("COM: libusb_get_config_descriptor failed: %s", libusb_error_name(usb_err));
    }

    *ppDevice = &g_usb_target_device;
    g_usb_target_device.ref_count++;
    log_info("COM: USB target device created (backed by libusb)");
    return S_OK;
}

static void *g_usb_factory_vtbl[] = {
    usb_factory_qi, stub_addref, stub_release,
    usb_factory_create_target,
};

static com_object g_usb_target_factory = {
    .vtbl = (com_vtable*)g_usb_factory_vtbl,
    .ref_count = 1,
    .impl_data = NULL,
};

/* ─── IWDFDevice ──────────────────────────────────────────────────── */

static com_object g_wdf_device;
static com_object g_wdf_driver;

static __winfnc HRESULT device_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID checks[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFDEVICE, IID_IWDFDEVICE2, IID_IWDFDEVICE3 };
    for(int i = 0; i < 5; i++) {
        if(guid_eq(riid, &checks[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    /* IWDFUsbTargetFactory → return separate object with correct vtable */
    GUID iid_usbfactory = IID_IWDFUSBTARGETFACTORY;
    if(guid_eq(riid, &iid_usbfactory)) {
        *ppv = &g_usb_target_factory;
        g_usb_target_factory.ref_count++;
        log_info("COM: Device::QI → IWDFUsbTargetFactory");
        return S_OK;
    }
    /* IWDFUsbTargetDevice → return USB target directly (some drivers skip the factory) */
    GUID iid_usbtarget = IID_IWDFUSBTARGETDEVICE;
    if(guid_eq(riid, &iid_usbtarget)) {
        if(com_libusb_dev && g_usb_target_device.ref_count > 0) {
            *ppv = &g_usb_target_device;
            g_usb_target_device.ref_count++;
            log_info("COM: Device::QI → IWDFUsbTargetDevice (direct)");
            return S_OK;
        }
        /* Auto-create USB target if we have a libusb handle */
        if(com_libusb_dev) {
            log_info("COM: Device::QI → Auto-creating USB target for IWDFUsbTargetDevice");
            com_object *target = NULL;
            if(usb_factory_create_target(&g_usb_target_factory, &target) == S_OK) {
                *ppv = target;
                return S_OK;
            }
        }
        log_warn("COM: Device::QI → IWDFUsbTargetDevice requested but no USB device");
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    /* IWDFPropertyStoreFactory */
    GUID iid_propfactory = IID_IWDFPROPERTYSTOREFACTORY;
    if(guid_eq(riid, &iid_propfactory)) {
        log_debug("COM: Device::QI for IWDFPropertyStoreFactory (not implemented)");
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    /* IWDFIoTarget — return self as default target */
    GUID iid_iotarget = IID_IWDFIOTARGET;
    if(guid_eq(riid, &iid_iotarget)) {
        *ppv = self;
        self->ref_count++;
        return S_OK;
    }
    log_guid("Device::QI MISS", riid);
    log_debug("COM: Device::QI full GUID: {%08x-%04x-%04x-%02x%02x-%016llx}",
        riid->PartA, riid->PartB, riid->PartC,
        (unsigned)(riid->PartD & 0xff), (unsigned)((riid->PartD >> 8) & 0xff),
        (unsigned long long)riid->PartE);
    *ppv = NULL;
    return E_NOINTERFACE;
}

/* ─── IWDFNamedPropertyStore (minimal) ────────────────────────────── */

static __winfnc HRESULT propstore_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFNAMEDPROPERTYSTORE };
    for(int i = 0; i < 2; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc HRESULT propstore_get_named_value(com_object *self, const char16_t *name, void *value) {
    char *cname = name ? winstr_to_str(name) : NULL;
    log_debug("COM: PropertyStore::GetNamedValue('%s')", cname ? cname : "");

    if(value) {
        /* PROPVARIANT layout: USHORT vt, padding, then value */
        memset(value, 0, 24);
        /* Return VT_UI4 (DWORD = 0) for known counter properties */
        if(cname && (strstr(cname, "Failure") || strstr(cname, "Count"))) {
            ((USHORT*)value)[0] = 19; /* VT_UI4 */
            /* Value at offset 8 = 0 (DWORD zero) */
            log_debug("COM: → returning VT_UI4(0)");
        }
        /* else VT_EMPTY (already zeroed) */
    }
    free(cname);
    return S_OK;
}

static __winfnc HRESULT propstore_set_named_value(com_object *self, const char16_t *name, void *value) {
    log_debug("COM: PropertyStore::SetNamedValue (stub)");
    return S_OK;
}

static __winfnc ULONG propstore_get_count(com_object *self) { return 0; }
static __winfnc HRESULT propstore_get_at(com_object *self, ULONG idx, void *name, void *value) { return E_INVALIDARG; }

static void *g_propstore_vtbl[] = {
    propstore_qi, stub_addref, stub_release,
    propstore_get_named_value,   /* [3] GetNamedValue */
    propstore_set_named_value,   /* [4] SetNamedValue */
    propstore_get_count,         /* [5] GetNamedValueCount */
    propstore_get_at,            /* [6] GetNamedValueAt */
    com_method_stub, com_method_stub, com_method_stub, com_method_stub,
};

static com_object g_propstore = {
    .vtbl = (com_vtable*)g_propstore_vtbl,
    .ref_count = 1,
    .impl_data = NULL,
};

static __winfnc HRESULT device_prop_store(com_object *self, const char16_t *s, DWORD f, void **store, DWORD *d) {
    char *svc = s ? winstr_to_str(s) : NULL;
    log_debug("COM: Device::RetrieveDevicePropertyStore('%s', flags=%u)", svc ? svc : "", f);
    free(svc);
    /* Return the property store for all callers. The driver stores this at
       its internal field_4D0, which controls the PrepareHardware code path. */
    if(store) *store = &g_propstore;
    if(d) *d = 0; /* WdfPropertyStoreExistingStore (not newly created) */
    g_propstore.ref_count++;
    return S_OK;
}
static __winfnc void device_get_driver(com_object *self, com_object **drv) { *drv = &g_wdf_driver; }
static __winfnc HRESULT device_instance_id(com_object *self, char16_t *buf, DWORD *size) {
    return devinit_instance_id(self, buf, size);
}
static __winfnc void device_get_default_target(com_object *self, void **t) { *t = self; }
static __winfnc HRESULT device_create_file(com_object *self, const char16_t *n, void **f) {
    char *name = n ? winstr_to_str(n) : NULL;
    log_info("COM: Device::CreateWdfFile('%s')", name ? name : "(null)");
    free(name);
    /* Return a dummy file object — WinUsb_Initialize ignores the handle anyway */
    *f = self; /* Return device as file — enough to pass pointer checks */
    self->ref_count++;
    return S_OK;
}
static __winfnc HRESULT device_get_default_queue(com_object *self, void **q) {
    log_debug("COM: Device::GetDefaultIoQueue called");
    if(q) *q = self; /* return device as queue */
    return S_OK;
}

static __winfnc HRESULT device_create_io_queue(com_object *self, void *callback, BOOL isDefault, DWORD dispType, BOOL powerManaged, BOOL allowZeroLen, void **queue) {
    log_debug("COM: Device::CreateIoQueue(default=%d, dispatch=%u)", isDefault, dispType);
    *queue = self;
    self->ref_count++;
    if(callback) {
        com_object *cb = (com_object*)callback;
        void *ioctl_iface = NULL;
        GUID iid = IID_IQUEUECALLBACK_DEVIOCTL;
        if(cb->vtbl->QueryInterface(cb, &iid, &ioctl_iface) == S_OK && ioctl_iface) {
            com_ioctl_callback = (com_object*)ioctl_iface;
            log_info("COM: Captured IQueueCallbackDeviceIoControl from driver");
        }
    }
    return S_OK;
}

static __winfnc HRESULT device_create_iface(com_object *self, const GUID *g, const char16_t *r) { return S_OK; }
static __winfnc HRESULT device_assign_iface_state(com_object *self, const GUID *g, const char16_t *r, BOOL e) { return S_OK; }
static __winfnc HRESULT device_retrieve_name(com_object *self, char16_t *n, DWORD *l) { return E_NOTIMPL; }
static __winfnc HRESULT device_post_event(com_object *self, const GUID *g, DWORD t, BYTE *d, DWORD s) { return S_OK; }
static __winfnc HRESULT device_config_dispatching(com_object *self, void *q, DWORD t, BOOL f) { return S_OK; }
static __winfnc HRESULT device_create_request(com_object *self, void *cb, void *p, void **r) { return E_NOTIMPL; }
static __winfnc HRESULT device_create_symlink(com_object *self, const char16_t *l) { return S_OK; }

/* IWDFDevice2 */
static __winfnc HRESULT device_s0_idle(com_object *self, DWORD a, DWORD b, ULONG c, DWORD d, DWORD e) {
    log_debug("COM: Device::AssignS0IdleSettings");
    return S_OK;
}
static __winfnc HRESULT device_stop_idle(com_object *self, BOOL w) { return S_OK; }

/*
 * Full device vtable — every slot filled, no NULLs.
 * Array of function pointers indexed by vtable slot number.
 */
static void *g_device_vtbl[] = {
    /* IUnknown (0-2) */
    [0]  = device_qi,
    [1]  = stub_addref,
    [2]  = stub_release,
    /* IWDFObject (3-7) */
    [3]  = wdfobj_delete,
    [4]  = wdfobj_assign_ctx,
    [5]  = wdfobj_retrieve_ctx,
    [6]  = wdfobj_noop,           /* AcquireLock */
    [7]  = wdfobj_noop,           /* ReleaseLock */
    /* IWDFDevice (8-24) */
    [8]  = device_prop_store,     /* RetrieveDevicePropertyStore */
    [9]  = device_get_driver,     /* GetDriver */
    [10] = device_instance_id,    /* RetrieveDeviceInstanceId */
    [11] = device_get_default_target, /* GetDefaultIoTarget */
    [12] = device_create_file,    /* CreateWdfFile */
    [13] = device_get_default_queue,  /* GetDefaultIoQueue */
    [14] = device_create_io_queue,    /* CreateIoQueue */
    [15] = device_create_iface,       /* CreateDeviceInterface */
    [16] = device_assign_iface_state, /* AssignDeviceInterfaceState */
    [17] = device_retrieve_name,      /* RetrieveDeviceName */
    [18] = device_post_event,         /* PostEvent */
    [19] = device_config_dispatching, /* ConfigureRequestDispatching */
    [20] = com_void_stub,         /* SetPnpState */
    [21] = com_method_stub,       /* GetPnpState */
    [22] = com_void_stub,         /* CommitPnpState */
    [23] = device_create_request, /* CreateRequest */
    [24] = device_create_symlink, /* CreateSymbolicLink */
    /* IWDFDevice2 (25-34) */
    [25] = device_s0_idle,        /* AssignS0IdleSettings */
    [26] = device_stop_idle,      /* StopIdle */
    [27] = com_void_stub,         /* ResumeIdle */
    [28] = com_method_stub,       /* CreateSymbolicLinkWithReferenceString */
    [29] = com_method_stub,       /* RegisterRemoteInterfaceNotification */
    [30] = com_method_stub,       /* CreateRemoteInterface */
    [31] = com_method_stub,       /* CreateRemoteTarget */
    [32] = com_void_stub,         /* GetDeviceStackIoTypePreference */
    [33] = com_method_stub,       /* AssignSxWakeSettings */
    [34] = com_method_stub,       /* GetSystemPowerAction */
    /* IWDFDevice3 (35-42) */
    [35] = com_method_stub,       /* MapIoSpace */
    [36] = com_void_stub,         /* UnmapIoSpace */
    [37] = com_method_stub,       /* GetHardwareRegisterMappedAddress */
    [38] = com_method_stub,       /* ReadFromHardware */
    [39] = com_void_stub,         /* WriteToHardware */
    [40] = com_method_stub,       /* CreateInterrupt */
    [41] = com_method_stub,       /* CreateWorkItem */
    [42] = com_method_stub,       /* AssignS0IdleSettingsEx */
    /* Extra padding in case there are more methods */
    [43] = com_method_stub,
    [44] = com_method_stub,
    [45] = com_method_stub,
    [46] = com_method_stub,
    [47] = com_method_stub,
};

static com_object g_wdf_device = {
    .vtbl = (com_vtable*)g_device_vtbl,
    .ref_count = 1,
    .impl_data = NULL,
};

/* ─── IWDFDriver ──────────────────────────────────────────────────── */

static __winfnc HRESULT driver_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFDRIVER };
    for(int i = 0; i < 3; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    log_guid("Driver::QI unknown", riid);
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc HRESULT driver_create_device(com_object *self, com_object *pDevInit, void *pCallback, com_object **ppDevice) {
    log_info("COM: IWDFDriver::CreateDevice called");
    *ppDevice = &g_wdf_device;
    g_wdf_device.ref_count++;

    if(pCallback) {
        com_object *cb = (com_object*)pCallback;
        void *iface = NULL;

        GUID iid_hw = IID_IPNPCALLBACKHARDWARE;
        if(cb->vtbl->QueryInterface(cb, &iid_hw, &iface) == S_OK) {
            com_pnp_hw_callback = (com_object*)iface;
            log_info("COM: Captured IPnpCallbackHardware");
        }

        iface = NULL;
        GUID iid_hw2 = IID_IPNPCALLBACKHARDWARE2;
        if(cb->vtbl->QueryInterface(cb, &iid_hw2, &iface) == S_OK) {
            com_pnp_hw2_callback = (com_object*)iface;
            log_info("COM: Captured IPnpCallbackHardware2");
        }

        /* Probe unknown GUIDs found near IPnpCallback in the driver binary.
           One of these is likely IPnpCallbackHardware with a non-standard IID. */
        GUID probe_guids[] = {
            COM_GUID(0x4e9f1a77,0x4587,0x4235, 0x81,0xc4, 0xe6,0xd2,0x45,0x45,0xa6,0x56),
            COM_GUID(0x7ee9f0fa,0x5a1a,0x48df, 0xa3,0x5e, 0x8d,0xb4,0x2f,0x51,0x9b,0x66),
            COM_GUID(0x3ab1426d,0x689c,0x4220, 0x90,0x1e, 0x03,0xc6,0xd9,0x09,0xb5,0xf5),
            COM_GUID(0xa49a0bb4,0x9545,0x4aaf, 0x87,0x2e, 0x0d,0xef,0x0b,0xbb,0x58,0x34),
            COM_GUID(0x1493cd1b,0xc546,0x46bb, 0xbf,0x47, 0xb2,0x74,0x65,0x09,0x33,0x93),
            COM_GUID(0x3dad1a1c,0x7d52,0x4891, 0x8c,0xa3, 0xee,0x9a,0xa3,0x90,0x20,0x90),
        };
        for(int g = 0; g < 6; g++) {
            iface = NULL;
            if(cb->vtbl->QueryInterface(cb, &probe_guids[g], &iface) == S_OK && iface) {
                log_info("COM: Probe hit! GUID {%08x-%04x-%04x-...} → interface at %p",
                    probe_guids[g].PartA, probe_guids[g].PartB, probe_guids[g].PartC, iface);
                /* Assume it's the hardware callback if we don't have one yet */
                if(!com_pnp_hw_callback) {
                    com_pnp_hw_callback = (com_object*)iface;
                    log_info("COM: Using probed interface as IPnpCallbackHardware");
                }
            }
        }

        iface = NULL;
        GUID iid_pnp = IID_IPNPCALLBACK;
        if(cb->vtbl->QueryInterface(cb, &iid_pnp, &iface) == S_OK) {
            com_pnp_callback = (com_object*)iface;
            log_info("COM: Captured IPnpCallback");
        }

        iface = NULL;
        GUID iid_ioctl = IID_IQUEUECALLBACK_DEVIOCTL;
        if(cb->vtbl->QueryInterface(cb, &iid_ioctl, &iface) == S_OK) {
            com_ioctl_callback = (com_object*)iface;
            log_info("COM: Captured IQueueCallbackDeviceIoControl");
        }
    }
    return S_OK;
}

static __winfnc HRESULT driver_create_prealloc_mem(com_object *self, BYTE *buf, SIZE_T sz, void *cb, void *p, void **mem) {
    log_debug("COM: Driver::CreatePreallocatedWdfMemory(size=%zu)", sz);
    return E_NOTIMPL;
}
static __winfnc HRESULT driver_create_mem(com_object *self, SIZE_T sz, void *cb, void *p, void **mem) {
    log_debug("COM: Driver::CreateWdfMemory(size=%zu)", sz);
    return E_NOTIMPL;
}
static __winfnc BOOL driver_is_version_avail(com_object *self, void *ver) { return TRUE; }

static void *g_driver_vtbl[] = {
    driver_qi, stub_addref, stub_release,
    wdfobj_delete, wdfobj_assign_ctx, wdfobj_retrieve_ctx, wdfobj_noop, wdfobj_noop,
    driver_create_device,
    com_method_stub,       /* CreateWdfObject */
    driver_create_prealloc_mem,
    driver_create_mem,
    driver_is_version_avail,
    com_method_stub,       /* RetrieveVersionString */
};

static com_object g_wdf_driver = {
    .vtbl = (com_vtable*)g_driver_vtbl,
    .ref_count = 1,
    .impl_data = NULL,
};

/* ─── COM Host bootstrap ─────────────────────────────────────────── */

/* ─── IWDFMemory (buffer wrapper) ─────────────────────────────────── */

typedef struct {
    com_object obj;
    void *buffer;
    SIZE_T size;
} com_memory;

static __winfnc HRESULT mem_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFMEMORY };
    for(int i = 0; i < 3; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc HRESULT mem_copy_to(com_object *self, ULONG_PTR off, void *buf, SIZE_T n) {
    com_memory *m = (com_memory*)self;
    if(off + n > m->size) return E_INVALIDARG;
    memcpy(buf, (BYTE*)m->buffer + off, n);
    return S_OK;
}
static __winfnc HRESULT mem_copy_from(com_object *self, ULONG_PTR off, void *buf, SIZE_T n) {
    com_memory *m = (com_memory*)self;
    if(off + n > m->size) return E_INVALIDARG;
    memcpy((BYTE*)m->buffer + off, buf, n);
    return S_OK;
}
static __winfnc SIZE_T mem_get_size(com_object *self) {
    return ((com_memory*)self)->size;
}
static __winfnc void *mem_get_data(com_object *self, SIZE_T *size) {
    com_memory *m = (com_memory*)self;
    if(size) *size = m->size;
    return m->buffer;
}
static __winfnc void mem_set_buffer(com_object *self, void *buf, SIZE_T size) {
    com_memory *m = (com_memory*)self;
    m->buffer = buf;
    m->size = size;
}

static void *g_mem_vtbl[] = {
    mem_qi, stub_addref, stub_release,
    wdfobj_delete, wdfobj_assign_ctx, wdfobj_retrieve_ctx, wdfobj_noop, wdfobj_noop,
    com_method_stub,  /* CopyFromMemory */
    mem_copy_to,      /* CopyToBuffer */
    mem_copy_from,    /* CopyFromBuffer */
    mem_get_size,     /* GetSize */
    mem_get_data,     /* GetDataBuffer */
    mem_set_buffer,   /* SetBuffer */
};

static com_memory *com_memory_create(void *buf, SIZE_T size) {
    com_memory *m = (com_memory*)calloc(1, sizeof(com_memory));
    m->obj.vtbl = (com_vtable*)g_mem_vtbl;
    m->obj.ref_count = 1;
    m->buffer = buf;
    m->size = size;
    return m;
}

/* ─── IWDFIoRequest (for IOCTL routing) ──────────────────────────── */

typedef struct {
    com_object obj;
    ULONG ioctl_code;
    com_memory *in_mem;
    com_memory *out_mem;
    SIZE_T in_size;
    SIZE_T out_size;
    /* Completion state */
    volatile bool completed;
    HRESULT completion_status;
    SIZE_T completion_info;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} com_request;

static __winfnc HRESULT req_qi(com_object *self, const GUID *riid, void **ppv) {
    GUID ids[] = { IID_IUNKNOWN, IID_IWDFOBJECT, IID_IWDFIOREQUEST };
    for(int i = 0; i < 3; i++) {
        if(guid_eq(riid, &ids[i])) { *ppv = self; self->ref_count++; return S_OK; }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

static __winfnc void req_complete_with_info(com_object *self, HRESULT status, SIZE_T info) {
    com_request *r = (com_request*)self;
    pthread_mutex_lock(&r->lock);
    r->completion_status = status;
    r->completion_info = info;
    r->completed = true;
    pthread_cond_signal(&r->cond);
    pthread_mutex_unlock(&r->lock);
    log_debug("COM: Request completed: status=0x%x info=%zu", status, info);
}

static __winfnc void req_set_info(com_object *self, ULONG_PTR info) {
    ((com_request*)self)->completion_info = info;
}

static __winfnc void req_complete(com_object *self, HRESULT status) {
    req_complete_with_info(self, status, 0);
}

static __winfnc void req_get_devioctl_params(com_object *self, ULONG *code, SIZE_T *in_size, SIZE_T *out_size) {
    com_request *r = (com_request*)self;
    if(code) *code = r->ioctl_code;
    if(in_size) *in_size = r->in_size;
    if(out_size) *out_size = r->out_size;
}

static __winfnc void req_get_output_memory(com_object *self, com_object **mem) {
    *mem = (com_object*)((com_request*)self)->out_mem;
    if(*mem) (*mem)->ref_count++;
}

static __winfnc void req_get_input_memory(com_object *self, com_object **mem) {
    *mem = (com_object*)((com_request*)self)->in_mem;
    if(*mem) (*mem)->ref_count++;
}

static void *g_request_vtbl[] = {
    /* IUnknown (0-2) */
    req_qi, stub_addref, stub_release,
    /* IWDFObject (3-7) */
    wdfobj_delete, wdfobj_assign_ctx, wdfobj_retrieve_ctx, wdfobj_noop, wdfobj_noop,
    /* IWDFIoRequest (8-30) */
    req_complete_with_info,   /* [8]  CompleteWithInformation */
    req_set_info,             /* [9]  SetInformation */
    req_complete,             /* [10] Complete */
    com_void_stub,            /* [11] SetCompletionCallback */
    com_method_stub,          /* [12] GetType */
    com_void_stub,            /* [13] GetCreateParameters */
    com_void_stub,            /* [14] GetReadParameters */
    com_void_stub,            /* [15] GetWriteParameters */
    req_get_devioctl_params,  /* [16] GetDeviceIoControlParameters */
    req_get_output_memory,    /* [17] GetOutputMemory */
    req_get_input_memory,     /* [18] GetInputMemory */
    com_void_stub,            /* [19] MarkCancelable */
    com_method_stub,          /* [20] UnmarkCancelable */
    com_method_stub,          /* [21] CancelSentRequest */
    com_method_stub,          /* [22] ForwardToIoQueue */
    com_method_stub,          /* [23] Send */
    com_void_stub,            /* [24] GetFileObject */
    com_void_stub,            /* [25] FormatUsingCurrentType */
    com_method_stub,          /* [26] GetRequestorProcessId */
    com_void_stub,            /* [27] GetIoQueue */
    com_method_stub,          /* [28] Impersonate */
    com_method_stub,          /* [29] IsFrom32BitProcess */
    com_void_stub,            /* [30] GetCompletionParams */
    /* Padding */
    com_method_stub, com_method_stub, com_method_stub, com_method_stub,
};

/* Send an IOCTL through the COM IQueueCallbackDeviceIoControl path */
NTSTATUS com_send_ioctl(ULONG code, const void *in_buf, size_t in_size, void *out_buf, size_t out_size, size_t *bytes_returned) {
    if(!com_ioctl_callback) {
        log_error("COM: No IQueueCallbackDeviceIoControl captured");
        return STATUS_CANCELLED;
    }

    /* Create memory wrappers */
    com_memory *in_mem = in_buf ? com_memory_create((void*)in_buf, in_size) : NULL;
    com_memory *out_mem = out_buf ? com_memory_create(out_buf, out_size) : NULL;

    /* Create request */
    com_request req = {
        .obj = { .vtbl = (com_vtable*)g_request_vtbl, .ref_count = 1, .impl_data = NULL },
        .ioctl_code = code,
        .in_mem = in_mem,
        .out_mem = out_mem,
        .in_size = in_size,
        .out_size = out_size,
        .completed = false,
        .completion_status = E_FAIL,
        .completion_info = 0,
    };
    pthread_mutex_init(&req.lock, NULL);
    pthread_cond_init(&req.cond, NULL);

    /* Call the driver's OnDeviceIoControl */
    IQueueCallbackDeviceIoControlVtbl *vtbl = (IQueueCallbackDeviceIoControlVtbl*)com_ioctl_callback->vtbl;

    struct winmodule *mod = winmodule_get_cur();
    winmodule_set_cur(&tudor_driver_dll->module);
    vtbl->OnDeviceIoControl(com_ioctl_callback, &g_wdf_device, (com_object*)&req, code, in_size, out_size);
    winmodule_set_cur(mod);

    /* Wait for completion (with timeout) */
    if(!req.completed) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 10; /* 10 second timeout */
        pthread_mutex_lock(&req.lock);
        while(!req.completed) {
            if(pthread_cond_timedwait(&req.cond, &req.lock, &ts) != 0) {
                log_error("COM: IOCTL 0x%x timed out", code);
                pthread_mutex_unlock(&req.lock);
                pthread_mutex_destroy(&req.lock);
                pthread_cond_destroy(&req.cond);
                free(in_mem);
                free(out_mem);
                return STATUS_CANCELLED;
            }
        }
        pthread_mutex_unlock(&req.lock);
    }

    NTSTATUS status = (req.completion_status == S_OK) ? STATUS_SUCCESS : STATUS_CANCELLED;
    if(bytes_returned) *bytes_returned = req.completion_info;

    pthread_mutex_destroy(&req.lock);
    pthread_cond_destroy(&req.cond);
    free(in_mem);
    free(out_mem);

    return status;
}

/* ─── COM Host bootstrap ─────────────────────────────────────────── */

bool com_init_driver(struct dll_image *driver_dll) {
    GUID clsid = CLSID_SYNATUDOR;
    GUID iid_cf = IID_ICLASSFACTORY;
    GUID iid_de = IID_IDRIVERENTRY;
    GUID iid_unk = IID_IUNKNOWN;

    /* Step 1: DllGetClassObject → IClassFactory */
    log_info("COM: Calling DllGetClassObject...");
    api_DllGetClassObject get_class_obj = (api_DllGetClassObject)find_dll_export(driver_dll, "DllGetClassObject");

    com_object *factory = NULL;
    HRESULT hr = get_class_obj(&clsid, &iid_cf, (void**)&factory);
    if(hr != S_OK || !factory) {
        hr = get_class_obj(&clsid, &iid_unk, (void**)&factory);
        if(hr != S_OK || !factory) {
            log_error("COM: DllGetClassObject failed: 0x%x", hr);
            return false;
        }
    }
    log_info("COM: Got IClassFactory at %p", factory);

    /* Step 2: CreateInstance → IDriverEntry */
    IClassFactoryVtbl *cf_vtbl = (IClassFactoryVtbl*)factory->vtbl;
    com_object *driver_entry = NULL;
    hr = cf_vtbl->CreateInstance(factory, NULL, &iid_de, (void**)&driver_entry);
    if(hr != S_OK || !driver_entry) {
        log_error("COM: CreateInstance(IDriverEntry) failed: 0x%x", hr);
        return false;
    }
    com_driver_callback = driver_entry;
    log_info("COM: Got IDriverEntry at %p", driver_entry);

    /* Step 3: OnInitialize */
    IDriverEntryVtbl *de_vtbl = (IDriverEntryVtbl*)driver_entry->vtbl;
    log_info("COM: Calling OnInitialize...");
    hr = de_vtbl->OnInitialize(driver_entry, &g_wdf_driver);
    if(hr != S_OK) {
        log_error("COM: OnInitialize failed: 0x%x", hr);
        return false;
    }
    log_info("COM: OnInitialize OK");

    /* Step 4: OnDeviceAdd */
    com_object devinit_obj = {
        .vtbl = (com_vtable*)g_devinit_vtbl,
        .ref_count = 1,
        .impl_data = NULL,
    };
    log_info("COM: Calling OnDeviceAdd...");
    hr = de_vtbl->OnDeviceAdd(driver_entry, &g_wdf_driver, &devinit_obj);
    if(hr != S_OK) {
        log_error("COM: OnDeviceAdd failed: 0x%x", hr);
        return false;
    }
    log_info("COM: OnDeviceAdd OK");

    /* Save {1493cd1b...} interface pointer for external access */
    {
        GUID iid_hw_real = COM_GUID(0x1493cd1b,0xc546,0x46bb, 0xbf,0x47, 0xb2,0x74,0x65,0x09,0x33,0x93);
        com_object *base_obj = com_pnp_callback;
        void *hw_iface = NULL;
        if(base_obj && base_obj->vtbl->QueryInterface(base_obj, &iid_hw_real, &hw_iface) == S_OK && hw_iface) {
            com_usb_device_obj = hw_iface;
        }
    }

    /* Step 4b: Call WBFUsbInitialize.
       field_0x80 is zeroed by tudor_init after this function returns and before
       it's called again... but we only have one call. Instead, the zeroing happens
       in tudor_init between com_init_driver and the WINBIO pipeline init.
       WBFUsbInitialize returns S_FALSE if field_0x80 is non-zero, which is OK —
       the PrepareHardware binary patch bypasses the internal check anyway. */
    if(com_usb_device_obj) {
        typedef HRESULT __winfnc (*wbf_usb_init_fn)(void *self);
        wbf_usb_init_fn wbf_init = (wbf_usb_init_fn)(driver_dll->base_addr + 0x16160);

        struct winmodule *mod = winmodule_get_cur();
        winmodule_set_cur(&tudor_driver_dll->module);
        log_info("COM: Calling WBFUsbInitialize...");
        hr = wbf_init(com_usb_device_obj);
        winmodule_set_cur(mod);
        log_info("COM: WBFUsbInitialize returned hr=0x%x", hr);
    }

    /* Step 5: OnPrepareHardware */
    if(com_usb_device_obj) {
        void **vtbl = *(void***)com_usb_device_obj;
        log_info("COM: Calling OnPrepareHardware...");

        typedef HRESULT __winfnc (*prep_hw_fn)(com_object *self, com_object *pDevice);
        hr = ((prep_hw_fn)vtbl[3])((com_object*)com_usb_device_obj, &g_wdf_device);

        if((int)hr < 0) {
            log_error("COM: OnPrepareHardware failed: 0x%x", hr);
        } else {
            log_info("COM: OnPrepareHardware OK (hr=0x%x)", hr);
        }
    }

    /* Step 6: OnD0Entry */
    if(com_pnp_callback) {
        IPnpCallbackVtbl *pnp = (IPnpCallbackVtbl*)com_pnp_callback->vtbl;
        log_info("COM: Calling OnD0Entry (D3→D0)...");
        hr = pnp->OnD0Entry(com_pnp_callback, &g_wdf_device, 4);
        if((int)hr < 0) {
            log_error("COM: OnD0Entry failed: 0x%x", hr);
            return false;
        }
        log_info("COM: OnD0Entry OK (hr=0x%x) — driver operational", hr);
    }

    return true;
}

void com_shutdown_driver(void) {
    if(com_pnp_callback) {
        IPnpCallbackVtbl *pnp = (IPnpCallbackVtbl*)com_pnp_callback->vtbl;
        pnp->OnD0Exit(com_pnp_callback, &g_wdf_device, 4);
    }
    if(com_pnp_hw2_callback) {
        IPnpCallbackHardware2Vtbl *hw2 = (IPnpCallbackHardware2Vtbl*)com_pnp_hw2_callback->vtbl;
        hw2->OnReleaseHardware(com_pnp_hw2_callback, &g_wdf_device, NULL);
    } else if(com_pnp_hw_callback) {
        IPnpCallbackHardwareVtbl *hw = (IPnpCallbackHardwareVtbl*)com_pnp_hw_callback->vtbl;
        hw->OnReleaseHardware(com_pnp_hw_callback, &g_wdf_device);
    }
    if(com_driver_callback) {
        IDriverEntryVtbl *de = (IDriverEntryVtbl*)com_driver_callback->vtbl;
        de->OnDeinitialize(com_driver_callback, &g_wdf_driver);
    }
}
