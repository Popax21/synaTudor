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

/* WinUSB types */
typedef void *WINUSB_INTERFACE_HANDLE;

/* Sentinel value used by COM GetWinUsbHandle (must be 32-bit safe) */
#define WINUSB_HANDLE_SENTINEL ((void*)(uintptr_t)0x57494E55)

/* Map WinUSB handle to libusb device */
static inline libusb_device_handle *winusb_to_libusb(WINUSB_INTERFACE_HANDLE h) {
    if(h == WINUSB_HANDLE_SENTINEL) return tudor_com_usb_dev;
    return (libusb_device_handle*)h;
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

    log_debug("WinUsb_QueryInterfaceSettings: iface %d → class=%d endpoints=%d",
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

    log_debug("WinUsb_QueryPipe: iface %d pipe %d → addr=0x%02x type=%d maxpkt=%d",
        AltSettingIndex, PipeIndex, ep->bEndpointAddress, PipeInfo->PipeType, PipeInfo->MaximumPacketSize);
    return TRUE;
}
WINAPI(WinUsb_QueryPipe)

__winfnc BOOL WinUsb_ReadPipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred, OVERLAPPED *Overlapped) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    int transferred = 0;

    int err = libusb_interrupt_transfer(dev, PipeID, Buffer, BufferLength, &transferred, 5000);
    if(err != 0 && err != LIBUSB_ERROR_TIMEOUT) {
        log_warn("WinUsb_ReadPipe(0x%02x): %s", PipeID, libusb_error_name(err));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = transferred;
    log_debug("WinUsb_ReadPipe(0x%02x): %d bytes", PipeID, transferred);
    return TRUE;
}
WINAPI(WinUsb_ReadPipe)

__winfnc BOOL WinUsb_WritePipe(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, BYTE *Buffer, ULONG BufferLength, ULONG *LengthTransferred, OVERLAPPED *Overlapped) {
    libusb_device_handle *dev = winusb_to_libusb(InterfaceHandle);
    int transferred = 0;

    int err = libusb_interrupt_transfer(dev, PipeID, Buffer, BufferLength, &transferred, 5000);
    if(err != 0) {
        log_warn("WinUsb_WritePipe(0x%02x): %s", PipeID, libusb_error_name(err));
        return FALSE;
    }

    if(LengthTransferred) *LengthTransferred = transferred;
    log_debug("WinUsb_WritePipe(0x%02x): %d bytes", PipeID, transferred);
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
    log_debug("WinUsb_GetPipePolicy(pipe=0x%02x, policy=%u) — stub", PipeID, PolicyType);
    if(Value && ValueLength && *ValueLength >= 4) {
        *(ULONG*)Value = 0;
    }
    return TRUE;
}
WINAPI(WinUsb_GetPipePolicy)

__winfnc BOOL WinUsb_SetPipePolicy(WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, ULONG PolicyType, ULONG ValueLength, void *Value) {
    log_debug("WinUsb_SetPipePolicy(pipe=0x%02x, policy=%u) — stub", PipeID, PolicyType);
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
