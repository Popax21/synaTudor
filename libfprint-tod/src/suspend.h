#ifndef LIBFPRINT_TOD_SUSPEND_H
#define LIBFPRINT_TOD_SUSPEND_H

#include "device.h"

#define SUSPEND_CANCEL_TIMEOUT_SECS 10

void register_suspend_monitor(FpiDeviceTudor *tdev);
bool create_sleep_inhibitor(FpiDeviceTudor *tdev, GError **error);

static inline void close_sleep_inhibitor(gint inhib) {
    g_debug("Clearing sleep inhibitor %d for tudor device...", inhib);
    g_assert_no_errno(close(inhib));
}

#endif