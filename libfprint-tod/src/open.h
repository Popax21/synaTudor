#ifndef LIBFPRINT_TOD_OPEN_H
#define LIBFPRINT_TOD_OPEN_H

#include <stdint.h>
#include "device.h"

#define SHUTDOWN_TIMEOUT_SECS 5

void register_host_process_monitor(FpiDeviceTudor *tdev);

static inline int get_usb_device_fd(GUsbDevice *usb_dev) {
    int usb_fd;
    g_assert_no_errno(usb_fd = dup(((int**) usb_dev->priv)[3][10 + 2 + 4 + 2 + 1 + 1])); //Cursed offset magic
    return usb_fd;
}

void open_device(FpiDeviceTudor *tdev, int usb_fd, uint8_t usb_bus, uint8_t usb_addr, GAsyncReadyCallback callback, gpointer user_data);
void close_device(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);

void fpi_device_tudor_probe(FpDevice *dev);
void fpi_device_tudor_open(FpDevice *dev);
void fpi_device_tudor_close(FpDevice *dev);

#endif