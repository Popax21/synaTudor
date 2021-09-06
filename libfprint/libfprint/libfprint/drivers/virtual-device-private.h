/*
 * Virtual driver for "simple" device debugging
 *
 * Copyright (C) 2019 Benjamin Berg <bberg@redhat.com>
 * Copyright (C) 2020 Bastien Nocera <hadess@hadess.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * This is a virtual driver to debug the non-image based drivers. A small
 * python script is provided to connect to it via a socket, allowing
 * prints to registered programmatically.
 * Using this, it is possible to test libfprint and fprintd.
 */

#include <gio/gio.h>

#include "fpi-device.h"

#define MAX_LINE_LEN 1024

G_DECLARE_FINAL_TYPE (FpiDeviceVirtualListener, fpi_device_virtual_listener, FPI, DEVICE_VIRTUAL_LISTENER, GSocketListener)

typedef void (*FpiDeviceVirtualListenerConnectionCb) (FpiDeviceVirtualListener *listener,
                                                      gpointer                  user_data);

FpiDeviceVirtualListener * fpi_device_virtual_listener_new (void);

gboolean fpi_device_virtual_listener_start (FpiDeviceVirtualListener            *listener,
                                            const char                          *address,
                                            GCancellable                        *cancellable,
                                            FpiDeviceVirtualListenerConnectionCb cb,
                                            gpointer                             user_data,
                                            GError                             **error);

gboolean fpi_device_virtual_listener_connection_close (FpiDeviceVirtualListener *listener);

void fpi_device_virtual_listener_read (FpiDeviceVirtualListener *listener,
                                       gboolean                  all,
                                       void                     *buffer,
                                       gsize                     count,
                                       GAsyncReadyCallback       callback,
                                       gpointer                  user_data);
gsize fpi_device_virtual_listener_read_finish (FpiDeviceVirtualListener *listener,
                                               GAsyncResult             *result,
                                               GError                  **error);

gboolean fpi_device_virtual_listener_write_sync (FpiDeviceVirtualListener *self,
                                                 const char               *buffer,
                                                 gsize                     count,
                                                 GError                  **error);


struct _FpDeviceVirtualDevice
{
  FpDevice                  parent;

  FpiDeviceVirtualListener *listener;
  GCancellable             *cancellable;

  char                      recv_buf[MAX_LINE_LEN];

  GPtrArray                *pending_commands;

  GHashTable               *prints_storage;

  guint                     wait_command_id;
  guint                     sleep_timeout_id;
  guint                     enroll_stages_passed;
  gboolean                  match_reported;
  gboolean                  supports_cancellation;
  gboolean                  injected_synthetic_cmd;
  gboolean                  ignore_wait;
  gboolean                  keep_alive;
};

/* Not really final here, but we can do this to share the FpDeviceVirtualDevice
 * contents without having to use a shared private struct instead. */
G_DECLARE_FINAL_TYPE (FpDeviceVirtualDevice, fpi_device_virtual_device, FP, DEVICE_VIRTUAL_DEVICE, FpDevice)

struct _FpDeviceVirtualDeviceStorage
{
  FpDeviceVirtualDevice parent;
};

G_DECLARE_FINAL_TYPE (FpDeviceVirtualDeviceStorage, fpi_device_virtual_device_storage, FP, DEVICE_VIRTUAL_DEVICE_STORAGE, FpDeviceVirtualDevice)


char * process_cmds (FpDeviceVirtualDevice * self, gboolean scan, GError **error);
char * start_scan_command (FpDeviceVirtualDevice *self,
                           GError               **error);
gboolean should_wait_for_command (FpDeviceVirtualDevice *self,
                                  GError                *error);
gboolean should_wait_to_sleep (FpDeviceVirtualDevice *self,
                               const char            *scan_id,
                               GError                *error);
