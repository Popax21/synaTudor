#ifndef LIBFPRINT_TOD_SANDBOX_H
#define LIBFPRINT_TOD_SANDBOX_H

#include "device.h"

int sandbox_open_file(FpiDeviceTudor *tdev, const char *filename, int flags, GError **error);

#endif