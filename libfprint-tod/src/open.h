#ifndef LIBFPRINT_TOD_OPEN_H
#define LIBFPRINT_TOD_OPEN_H

#include "device.h"

void open_device(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);
void close_device(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);

void fpi_device_tudor_probe(FpDevice *dev);
void fpi_device_tudor_open(FpDevice *dev);
void fpi_device_tudor_close(FpDevice *dev);

#endif