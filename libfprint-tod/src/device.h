#ifndef LIBFPRINT_TOD_DEVICE_H
#define LIBFPRINT_TOD_DEVICE_H

#include <stdbool.h>
#include "drivers_api.h"

typedef struct _IPCMessageBuf IPCMessageBuf;

struct _FpiDeviceTudor {
    FpDevice parent;

    GDBusConnection *dbus_con;

    guint host_id;
    bool host_has_id, host_dead;

    GSocket *ipc_socket;
    GCancellable *ipc_cancel;
    IPCMessageBuf *send_msg;
    bool in_shutdown;

    GPtrArray *db_records;
};

#define FPI_TYPE_DEVICE_TUDOR (fpi_device_tudor_get_type())
G_DECLARE_FINAL_TYPE(FpiDeviceTudor, fpi_device_tudor, FPI, DEVICE_TUDOR, FpDevice)

#endif