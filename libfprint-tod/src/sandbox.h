#ifndef LIBFPRINT_TOD_SANDBOX_H
#define LIBFPRINT_TOD_SANDBOX_H

#include "device.h"
#include "ipc.h"

int sandbox_open_file(FpiDeviceTudor *tdev, const char *path, int flags, GError **error);
int sandbox_ls_dir(FpiDeviceTudor *tdev, const char *path, struct ipc_sbox_dir_ent **entries, GError **error);

#endif