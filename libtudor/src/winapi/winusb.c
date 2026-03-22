/*
 * WinUSB API shims backed by libusb.
 * The driver dynamically loads winusb.dll and calls these via GetProcAddress.
 * Since GetProcAddress resolves through resolve_windows_api(), registering
 * these with WINAPI() makes them discoverable automatically.
 */

#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include "internal.h"

/* The global libusb device handle, set from COM path */
extern libusb_device_handle *tudor_com_usb_dev;

/* Sentinel value used by COM GetWinUsbHandle (must be 32-bit safe) */
#define WINUSB_HANDLE_SENTINEL ((void*)(uintptr_t)0x57494E55)

/*
 * Fake Synaptics USB COM interface.
 * The DLL's palWinUsbQueryInterfaceExternal creates a wrapper that calls
 * through an interface object at wrapper[0xB8]. This interface is obtained
 * via QueryInterface(IID_SynapticsUsb) which fails on our framework.
 * We provide a fake interface whose vtable methods do the actual USB I/O
 * through libusb.
 *
 * Wrapper vtable calls → interface vtable offsets:
 *   wrapper[0x10] → interface[0x18]  (CreateFile / device open)
 *   wrapper[0x18] → interface[0x20]  (CloseHandle / device close)
 *   wrapper[0x20] → interface[0x30]  (WritePipe)
 *   wrapper[0x78] → interface[0x80]  (Initialize / pipe query)
 */

/* All methods use __winfnc (Windows x64 CC: rcx, rdx, r8, r9) */
static __winfnc int64_t fake_iface_queryinterface(void *self, void *riid, void **ppv) {
    if(ppv) *ppv = self;
    return 0; /* S_OK */
}
static __winfnc uint32_t fake_iface_addref(void *self) { return 1; }
static __winfnc uint32_t fake_iface_release(void *self) { return 1; }

static int _last_write_done = 0;  /* Track write→read sequencing for fake interface */

/* Numbered debug wrappers to identify which vtable offset is called */
#define MAKE_NUMBERED_IFACE(N) \
static __winfnc int64_t fake_iface_##N(void *p1, void *p2, void *p3, void *p4, void *p5, void *p6) { \
    log_info("fake_iface[0x%02x]: p1=%p p2=%p p3=%p p4=%p p5=%p p6=%p", N*8, p1, p2, p3, p4, p5, p6); \
    return 1; \
}
MAKE_NUMBERED_IFACE(4)
MAKE_NUMBERED_IFACE(5)
MAKE_NUMBERED_IFACE(6)
MAKE_NUMBERED_IFACE(7)
/* vtable[7]=0x38 and [8]=0x40: called by wrapper[0x48] and wrapper[0x50].
   These are the actual WinUsb_WritePipe/ReadPipe via wrapper layer.
   Params from wrapper: (interface, pipe_byte, data_len, data_len2, stack_5th_arg)
   The wrapper repackages: edx=pipe_byte, r8d=len1, r9d=len2, [rsp+0x20]=5th */
static __winfnc int64_t fake_iface_wrapper_io(void *self, uint8_t pipe, uint32_t param1, uint32_t param2, void *extra) {
    log_info("fake_iface WRAPPER_IO: pipe=0x%02x p1=%u p2=%u extra=%p", pipe, param1, param2, extra);
    /* This is called via the wrapper path for BulkWrite/BulkRead.
       For now return success — the actual I/O goes through direct [0x48]/[0x50] calls. */
    return 1;
}

MAKE_NUMBERED_IFACE(11)
MAKE_NUMBERED_IFACE(12)
MAKE_NUMBERED_IFACE(13)
MAKE_NUMBERED_IFACE(14)
MAKE_NUMBERED_IFACE(15)
MAKE_NUMBERED_IFACE(17)
MAKE_NUMBERED_IFACE(18)
MAKE_NUMBERED_IFACE(19)
#undef MAKE_NUMBERED_IFACE

/* vtable[9]=0x48: BulkWrite (38-byte Tudor frames)
   vtable[10]=0x50: BulkWrite (1-byte GET_VERSION)
   Params: (self, pipe_idx(nil), buffer, length, &transferred) */
static __winfnc int64_t fake_iface_bulkwrite(void *self, void *pipe, uint8_t *buf, uint32_t len, void *p5, void *p6) {
    log_info("fake_iface WRITE: buf=%p len=%u p5=%p p6=%p (first: %02x %02x %02x %02x)",
        buf, len, p5, p6,
        buf ? buf[0] : 0, (buf && len > 1) ? buf[1] : 0,
        (buf && len > 2) ? buf[2] : 0, (buf && len > 3) ? buf[3] : 0);
    uint32_t *transferred = (uint32_t*)p5;  /* p5 might be &transferred */
    if(!tudor_com_usb_dev || !buf) return 0;
    /* Skip all-zeros frames (TLS not initialized → empty encrypted frame) */
    int all_zero = 1;
    for(uint32_t i = 0; i < len && i < 64; i++) if(buf[i]) { all_zero = 0; break; }
    if(all_zero && len > 1) {
        log_info("fake_iface WRITE: skipping %u-byte all-zeros frame", len);
        if(transferred) *transferred = len; /* pretend success */
        return 1;
    }
    int xfr = 0;
    int err = libusb_bulk_transfer(tudor_com_usb_dev, 0x01, buf, len, &xfr, 5000);
    if(err != 0) {
        log_warn("fake_iface WRITE: error %s", libusb_error_name(err));
        return 0;
    }
    if(transferred) *transferred = xfr;
    log_info("fake_iface WRITE: sent %d/%d bytes OK", xfr, len);
    _last_write_done = 1;

    /* The response should go to the transferred_ptr area or a separate read buffer.
       p5 (transferred ptr) might point to a larger struct with response buffer.
       For now, just do the read so the endpoint is drained for the next [0x30] call. */
    return 1;
}

/* vtable[6]=0x30: QueryPipe / BulkRead
   During PrepareHardware: pipe query (p3=pipe_idx, p4=info_ptr)
   During init loop: bulk read (p3=length?, p4=output?)
   For now: if p3 looks like a buffer ptr → read, else query */
/* [0x30] is called for BOTH pipe queries AND bulk reads.
   During PrepareHardware: pipe query (returns pipe info)
   During init loop after a write: bulk read (returns sensor response)
   Detect: if there's pending data in the endpoint, read it. */

static __winfnc int64_t fake_iface_querypipe_or_read(void *self, void *pipe, void *p3, void *p4, void *p5, void *p6) {
    uint64_t p3v = (uint64_t)p3;
    uint64_t p4v = (uint64_t)p4;

    /* If p3 is a small value AND p4 is a valid pointer → pipe query or read */
    log_info("fake_iface[0x30]: pipe=%p p3=0x%lx p4=%p p5=%p (last_write=%d)",
        pipe, (long)p3v, p4, p5, _last_write_done);

    /* Dump the transfer info structure at p4 to understand the I/O layout */
    if(p4v > 0x10000) {
        uint64_t *info = (uint64_t*)p4;
        log_info("fake_iface[0x30] info@%p: [0]=0x%lx [1]=0x%lx [2]=0x%lx",
            p4, info[0], info[1], info[2]);
    }

    /* If we just did a write and this is pipe 1 (IN), do a bulk read */
    if(_last_write_done && p3v == 1 && p4v > 0x10000 && tudor_com_usb_dev) {
        _last_write_done = 0;
        uint8_t resp[64] = {0};
        int resp_len = 0;
        int err = libusb_bulk_transfer(tudor_com_usb_dev, 0x81, resp, sizeof(resp), &resp_len, 2000);
        if(err == 0 && resp_len > 0) {
            char hex[200] = {0};
            int pos = 0;
            for(int i = 0; i < resp_len && i < 32 && pos < 190; i++)
                pos += snprintf(hex+pos, sizeof(hex)-pos, "%02x ", resp[i]);
            log_info("fake_iface[0x30] READ: %d bytes: %s", resp_len, hex);
            /* Also try writing response to p5 or p4 — the DLL's expected output */
            uint64_t p5v = (uint64_t)p5;
            if(p5v > 0x10000) {
                /* p5 might be a transfer result struct — write actual_length there */
                *(uint32_t*)p5 = resp_len;
                log_info("  → wrote resp_len=%d to p5=%p", resp_len, p5);
            }
        } else {
            log_info("fake_iface[0x30] READ: %s", err ? libusb_error_name(err) : "0 bytes");
        }
    }

    /* Always fill in pipe info (the DLL checks this during setup) */
    if(p4v > 0x10000) {
        uint8_t *info = (uint8_t*)p4;
        if(p3v == 0) {
            info[0] = 3; info[1] = 0x01; *(uint16_t*)(info + 2) = 60; /* OUT pipe */
        } else if(p3v == 1) {
            info[0] = 3; info[1] = 0x81; *(uint16_t*)(info + 2) = 60; /* IN pipe */
        }
    }
    return 1;
}

/* vtable[3] = offset 0x18: device open (CreateFile equivalent)
   Returns a HANDLE. Must be non-zero/non-INVALID for success. */
static __winfnc int64_t fake_iface_open(void *p1, void *p2, void *p3, void *p4) {
    log_info("fake_iface[0x18] open: p1=%p p2=%p p3=%p p4=%p", p1, p2, p3, p4);
    return (int64_t)WINUSB_HANDLE_SENTINEL;
}

/* vtable[4] = offset 0x20: device close — noop */
static __winfnc int fake_iface_close(void *self) {
    log_info("fake_iface: close (noop)");
    return 0;
}

/* vtable[5] = offset 0x28: SetPipePolicy equivalent */
static __winfnc int fake_iface_setpolicy(void *self, uint8_t pipe, uint8_t policy, uint32_t len, void *val) {
    log_info("fake_iface: setPipePolicy(pipe=0x%02x, policy=%d)", pipe, policy);
    return 1; /* TRUE = success */
}

/* vtable[6] = offset 0x30: WritePipe — the critical one! */
static __winfnc int fake_iface_write(void *self, uint8_t pipe, uint8_t *buf, uint32_t len, uint32_t *transferred) {
    log_info("fake_iface: WritePipe(pipe=0x%02x, len=%u)", pipe, len);
    if(!tudor_com_usb_dev) { log_error("fake_iface: no USB device!"); return 0; }
    int xfr = 0;
    int err = libusb_bulk_transfer(tudor_com_usb_dev, pipe, buf, len, &xfr, 5000);
    if(err != 0) {
        log_warn("fake_iface: WritePipe error: %s", libusb_error_name(err));
        return 0; /* FALSE */
    }
    if(transferred) *transferred = xfr;
    log_info("fake_iface: WritePipe sent %d bytes", xfr);
    return 1; /* TRUE = success */
}

/* vtable[7-15] = misc stubs */
static __winfnc int fake_iface_read(void *self, uint8_t pipe, uint8_t *buf, uint32_t len, uint32_t *transferred) {
    log_info("fake_iface: ReadPipe(pipe=0x%02x, len=%u)", pipe, len);
    if(!tudor_com_usb_dev) { log_error("fake_iface: no USB device!"); return 0; }
    int xfr = 0;
    int err = libusb_bulk_transfer(tudor_com_usb_dev, pipe, buf, len, &xfr, 5000);
    if(err != 0 && err != LIBUSB_ERROR_TIMEOUT) {
        log_warn("fake_iface: ReadPipe error: %s", libusb_error_name(err));
        return 0;
    }
    if(transferred) *transferred = xfr;
    log_info("fake_iface: ReadPipe got %d bytes", xfr);
    return 1;
}

static __winfnc int fake_iface_stub(void *self) {
    log_info("fake_iface: unhandled method called");
    return 0;
}

/* vtable[16] = offset 0x80: WinUsb_Initialize equivalent */
static __winfnc int64_t fake_iface_initialize(void *p1, void *p2, void *p3, void *p4) {
    log_info("fake_iface[0x80] init: p1=%p p2=%p p3=%p p4=%p", p1, p2, p3, p4);
    /* Try writing sentinel to various output params */
    if(p2 && (uint64_t)p2 > 0x10000) *(void**)p2 = WINUSB_HANDLE_SENTINEL;
    if(p3 && (uint64_t)p3 > 0x10000) *(void**)p3 = WINUSB_HANDLE_SENTINEL;
    return 1;
}

/* Build the vtable — identified from runtime tracing */
static void *fake_synusb_vtable[20] = {
    /* [0]  0x00 */ (void*)fake_iface_queryinterface,
    /* [1]  0x08 */ (void*)fake_iface_addref,
    /* [2]  0x10 */ (void*)fake_iface_release,
    /* [3]  0x18 */ (void*)fake_iface_open,          /* CreateFile equiv */
    /* [4]  0x20 */ (void*)fake_iface_4,             /* Close? */
    /* [5]  0x28 */ (void*)fake_iface_5,             /* ? */
    /* [6]  0x30 */ (void*)fake_iface_querypipe_or_read, /* QueryPipe + pipe info */
    /* [7]  0x38 */ (void*)fake_iface_wrapper_io,     /* wrapper[0x48] → write/read */
    /* [8]  0x40 */ (void*)fake_iface_wrapper_io,     /* wrapper[0x50] → write/read */
    /* [9]  0x48 */ (void*)fake_iface_bulkwrite,     /* BulkWrite (38-byte frames) */
    /* [10] 0x50 */ (void*)fake_iface_bulkwrite,     /* BulkWrite (1-byte cmds) */
    /* [11] 0x58 */ (void*)fake_iface_11,
    /* [12] 0x60 */ (void*)fake_iface_12,
    /* [13] 0x68 */ (void*)fake_iface_13,
    /* [14] 0x70 */ (void*)fake_iface_14,
    /* [15] 0x78 */ (void*)fake_iface_15,
    /* [16] 0x80 */ (void*)fake_iface_initialize,    /* WinUsb_Initialize */
    /* [17] 0x88 */ (void*)fake_iface_17,            /* Reset/reopen */
    /* [18] 0x90 */ (void*)fake_iface_18,
    /* [19] 0x98 */ (void*)fake_iface_19,
};

/* The fake COM interface object — just a vtable pointer */
static void *fake_synusb_interface_storage = (void*)fake_synusb_vtable;

/* Exported for the runtime hook to inject into wrapper[0xB8] */
void *synusb_get_fake_interface(void) {
    return &fake_synusb_interface_storage;
}

/* WinUSB types */
typedef void *WINUSB_INTERFACE_HANDLE;

/* Map WinUSB handle to libusb device.
   Always use tudor_com_usb_dev — the DLL's PAL layer creates custom handles
   that aren't valid libusb pointers. Our libusb device is the only real one. */
static inline libusb_device_handle *winusb_to_libusb(WINUSB_INTERFACE_HANDLE h) {
    (void)h;
    return tudor_com_usb_dev;
}

typedef struct {
    UCHAR PipeType;
    UCHAR PipeId;
    USHORT MaximumPacketSize;
    UCHAR Interval;
} WINUSB_PIPE_INFORMATION;

typedef struct {
    UCHAR RequestType;
    UCHAR Request;
    USHORT Value;
    USHORT Index;
    USHORT Length;
} WINUSB_SETUP_PACKET;

/* Track claimed interfaces */
static bool usb_initialized = false;
static struct libusb_config_descriptor *usb_cfg = NULL;

__winfnc BOOL WinUsb_Initialize(HANDLE DeviceHandle, WINUSB_INTERFACE_HANDLE *InterfaceHandle) {
    log_info("WinUsb_Initialize called!");

    if(!tudor_com_usb_dev) {
        log_error("WinUsb_Initialize: No USB device available");
        return FALSE;
    }

    if(!usb_initialized) {
        int err;

        /* Get config descriptor */
        if((err = libusb_get_config_descriptor(libusb_get_device(tudor_com_usb_dev), 0, &usb_cfg)) != 0) {
            log_error("WinUsb_Initialize: libusb_get_config_descriptor failed: %s", libusb_error_name(err));
            return FALSE;
        }

        /* Detach kernel drivers and claim interfaces */
        for(int i = 0; i < usb_cfg->bNumInterfaces; i++) {
            if(libusb_kernel_driver_active(tudor_com_usb_dev, i) == 1) {
                log_debug("WinUsb_Initialize: Detaching kernel driver from interface %d", i);
                libusb_detach_kernel_driver(tudor_com_usb_dev, i);
            }
            if((err = libusb_claim_interface(tudor_com_usb_dev, i)) != 0) {
                log_warn("WinUsb_Initialize: libusb_claim_interface(%d) failed: %s", i, libusb_error_name(err));
            }
        }
        usb_initialized = true;
    }

    /* Return the libusb handle as the "WinUSB handle" */
    *InterfaceHandle = tudor_com_usb_dev;
    log_info("WinUsb_Initialize: Success (interfaces: %d)", usb_cfg->bNumInterfaces);
    return TRUE;
}
WINAPI(WinUsb_Initialize)

__winfnc BOOL WinUsb_Free(WINUSB_INTERFACE_HANDLE InterfaceHandle) {
    log_debug("WinUsb_Free called");
    /* Don't actually free — we manage the libusb handle lifecycle elsewhere */
    return TRUE;
}
WINAPI(WinUsb_Free)

__winfnc BOOL WinUsb_QueryInterfaceSettings(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR AltSettingIndex, void *UsbAltInterfaceDescriptor) {
    if(!usb_cfg || AltSettingIndex >= usb_cfg->bNumInterfaces) return FALSE;

    const struct libusb_interface *iface = &usb_cfg->interface[AltSettingIndex];
    if(iface->num_altsetting < 1) return FALSE;

    const struct libusb_interface_descriptor *desc = &iface->altsetting[0];
    /* Copy the interface descriptor (same layout as USB_INTERFACE_DESCRIPTOR) */
    memcpy(UsbAltInterfaceDescriptor, desc, 9); /* bLength through iInterface */

    log_info("WinUsb_QueryInterfaceSettings: iface %d → class=%d endpoints=%d",
        AltSettingIndex, desc->bInterfaceClass, desc->bNumEndpoints);
    return TRUE;
}
WINAPI(WinUsb_QueryInterfaceSettings)

__winfnc BOOL WinUsb_QueryPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR AltSettingIndex, UCHAR PipeIndex, WINUSB_PIPE_INFORMATION *PipeInfo) {
    if(!usb_cfg || AltSettingIndex >= usb_cfg->bNumInterfaces) return FALSE;

    const struct libusb_interface *iface = &usb_cfg->interface[AltSettingIndex];
    if(iface->num_altsetting < 1) return FALSE;

    const struct libusb_interface_descriptor *desc = &iface->altsetting[0];
    if(PipeIndex >= desc->bNumEndpoints) return FALSE;

    const struct libusb_endpoint_descriptor *ep = &desc->endpoint[PipeIndex];
    PipeInfo->PipeType = (ep->bmAttributes & 0x03) + 1; /* libusb→WinUSB type mapping */
    PipeInfo->PipeId = ep->bEndpointAddress;
    PipeInfo->MaximumPacketSize = ep->wMaxPacketSize;
    PipeInfo->Interval = ep->bInterval;

    log_info("WinUsb_QueryPipe: iface %d pipe %d → addr=0x%02x type=%d maxpkt=%d",
        AltSettingIndex, PipeIndex, ep->bEndpointAddress, PipeInfo->PipeType, PipeInfo->MaximumPacketSize);
    return TRUE;
}
WINAPI(WinUsb_QueryPipe)

__winfnc BOOL WinUsb_ReadPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred, OVERLAPPED *Overlapped) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    log_info("WinUsb_ReadPipe(pipe=0x%02x, len=%u, dev=%p)", PipeID, BufferLength, dev);
    if(!dev) { log_error("WinUsb_ReadPipe: NULL device!"); return FALSE; }
    int transferred = 0;

    int err = libusb_bulk_transfer(dev, PipeID, Buffer, BufferLength, &transferred, 5000);
    if(err != 0 && err != LIBUSB_ERROR_TIMEOUT) {
        log_warn("WinUsb_ReadPipe(0x%02x): %s", PipeID, libusb_error_name(err));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = transferred;
    log_info("WinUsb_ReadPipe(0x%02x): got %d bytes", PipeID, transferred);
    return TRUE;
}
WINAPI(WinUsb_ReadPipe)

__winfnc BOOL WinUsb_WritePipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred, OVERLAPPED *Overlapped) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    log_info("WinUsb_WritePipe(pipe=0x%02x, len=%u, dev=%p)", PipeID, BufferLength, dev);
    if(!dev) { log_error("WinUsb_WritePipe: NULL device!"); return FALSE; }
    int transferred = 0;

    int err = libusb_bulk_transfer(dev, PipeID, Buffer, BufferLength, &transferred, 5000);
    if(err != 0) {
        log_warn("WinUsb_WritePipe(0x%02x): %s", PipeID, libusb_error_name(err));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = transferred;
    log_info("WinUsb_WritePipe(0x%02x): sent %d bytes", PipeID, transferred);
    return TRUE;
}
WINAPI(WinUsb_WritePipe)

__winfnc BOOL WinUsb_ControlTransfer(WINUSB_INTERFACE_HANDLE InterfaceHandle, WINUSB_SETUP_PACKET SetupPacket, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred, OVERLAPPED *Overlapped) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);

    int ret = libusb_control_transfer(dev,
        SetupPacket.RequestType, SetupPacket.Request,
        SetupPacket.Value, SetupPacket.Index,
        Buffer, BufferLength, 5000);

    if(ret < 0) {
        log_warn("WinUsb_ControlTransfer: %s", libusb_error_name(ret));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = ret;
    log_debug("WinUsb_ControlTransfer: %d bytes", ret);
    return TRUE;
}
WINAPI(WinUsb_ControlTransfer)

__winfnc BOOL WinUsb_GetPipePolicy(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, ULONG PolicyType, ULONG *ValueLength, void *Value) {
    log_info("WinUsb_GetPipePolicy(pipe=0x%02x, policy=%u) — stub", PipeID, PolicyType);
    if(Value && ValueLength && *ValueLength >= 4) {
        *(ULONG*)Value = 0;
    }
    return TRUE;
}
WINAPI(WinUsb_GetPipePolicy)

__winfnc BOOL WinUsb_SetPipePolicy(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, ULONG PolicyType, ULONG ValueLength, void *Value) {
    log_info("WinUsb_SetPipePolicy(pipe=0x%02x, policy=%u) — stub", PipeID, PolicyType);
    return TRUE;
}
WINAPI(WinUsb_SetPipePolicy)

__winfnc BOOL WinUsb_FlushPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID) {
    log_debug("WinUsb_FlushPipe(0x%02x)", PipeID);
    return TRUE;
}
WINAPI(WinUsb_FlushPipe)

__winfnc BOOL WinUsb_AbortPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID) {
    log_debug("WinUsb_AbortPipe(0x%02x)", PipeID);
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    libusb_clear_halt(dev, PipeID);
    return TRUE;
}
WINAPI(WinUsb_AbortPipe)

__winfnc BOOL WinUsb_ResetPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID) {
    log_debug("WinUsb_ResetPipe(0x%02x)", PipeID);
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    libusb_clear_halt(dev, PipeID);
    return TRUE;
}
WINAPI(WinUsb_ResetPipe)

__winfnc BOOL WinUsb_GetDescriptor(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR DescriptorType, UCHAR Index, USHORT LanguageID, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);

    int ret = libusb_get_descriptor(dev, DescriptorType, Index, Buffer, BufferLength);
    if(ret < 0) {
        log_warn("WinUsb_GetDescriptor(type=%d, idx=%d): %s", DescriptorType, Index, libusb_error_name(ret));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = ret;
    log_debug("WinUsb_GetDescriptor(type=%d, idx=%d): %d bytes", DescriptorType, Index, ret);
    return TRUE;
}
WINAPI(WinUsb_GetDescriptor)
