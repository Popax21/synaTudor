#ifndef LIBFPRINT_TOD_OPEN_H
#define LIBFPRINT_TOD_OPEN_H

#include <stdint.h>
#include "device.h"

void open_device(FpiDeviceTudor *tdev, int usb_fd, uint8_t usb_bus, uint8_t usb_addr, GAsyncReadyCallback callback, gpointer user_data);
void close_device(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);

void fpi_device_tudor_probe(FpDevice *dev);
void fpi_device_tudor_open(FpDevice *dev);
void fpi_device_tudor_close(FpDevice *dev);

#endif