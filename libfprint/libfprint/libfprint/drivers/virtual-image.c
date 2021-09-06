/*
 * Virtual driver for image device debugging
 *
 * Copyright (C) 2019 Benjamin Berg <bberg@redhat.com>
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
 * This is a virtual driver to debug the image based drivers. A small
 * python script is provided to connect to it via a socket, allowing
 * prints to be sent to this device programmatically.
 * Using this it is possible to test libfprint and fprintd.
 */

#define FP_COMPONENT "virtual_image"

#include "fpi-log.h"

#include "virtual-device-private.h"

#include "../fpi-image.h"
#include "../fpi-image-device.h"

struct _FpDeviceVirtualImage
{
  FpImageDevice             parent;

  FpiDeviceVirtualListener *listener;
  GCancellable             *cancellable;

  gboolean                  automatic_finger;
  FpImage                  *recv_img;
  gint                      recv_img_hdr[2];
};

G_DECLARE_FINAL_TYPE (FpDeviceVirtualImage, fpi_device_virtual_image, FPI, DEVICE_VIRTUAL_IMAGE, FpImageDevice)
G_DEFINE_TYPE (FpDeviceVirtualImage, fpi_device_virtual_image, FP_TYPE_IMAGE_DEVICE)

static void recv_image (FpDeviceVirtualImage *self);

static void
recv_image_img_recv_cb (GObject      *source_object,
                        GAsyncResult *res,
                        gpointer      user_data)
{
  g_autoptr(GError) error = NULL;
  FpiDeviceVirtualListener *listener = FPI_DEVICE_VIRTUAL_LISTENER (source_object);
  FpDeviceVirtualImage *self;
  FpImageDevice *device;
  gsize bytes;

  bytes = fpi_device_virtual_listener_read_finish (listener, res, &error);

  if (!bytes || g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED) ||
      g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CONNECTION_CLOSED))
    return;

  self = FPI_DEVICE_VIRTUAL_IMAGE (user_data);
  device = FP_IMAGE_DEVICE (self);

  if (self->automatic_finger)
    fpi_image_device_report_finger_status (device, TRUE);
  fpi_image_device_image_captured (device, g_steal_pointer (&self->recv_img));
  if (self->automatic_finger)
    fpi_image_device_report_finger_status (device, FALSE);

  /* And, listen for more images from the same client. */
  recv_image (self);
}

static void
recv_image_hdr_recv_cb (GObject      *source_object,
                        GAsyncResult *res,
                        gpointer      user_data)
{
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualImage *self;
  FpiDeviceVirtualListener *listener = FPI_DEVICE_VIRTUAL_LISTENER (source_object);
  gsize bytes;

  bytes = fpi_device_virtual_listener_read_finish (listener, res, &error);

  if (error)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED) ||
          g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CONNECTION_CLOSED) ||
          g_error_matches (error, G_IO_ERROR, G_IO_ERROR_PENDING))
        return;

      g_warning ("Error receiving header for image data: %s", error->message);
      return;
    }

  if (!bytes)
    return;

  self = FPI_DEVICE_VIRTUAL_IMAGE (user_data);
  if (self->recv_img_hdr[0] > 5000 || self->recv_img_hdr[1] > 5000)
    {
      g_warning ("Image header suggests an unrealistically large image, disconnecting client.");
      fpi_device_virtual_listener_connection_close (listener);
    }

  if (self->recv_img_hdr[0] < 0 || self->recv_img_hdr[1] < 0)
    {
      switch (self->recv_img_hdr[0])
        {
        case -1:
          /* -1 is a retry error, just pass it through */
          fpi_image_device_retry_scan (FP_IMAGE_DEVICE (self), self->recv_img_hdr[1]);
          break;

        case -2:
          /* -2 is a fatal error, just pass it through*/
          fpi_image_device_session_error (FP_IMAGE_DEVICE (self),
                                          fpi_device_error_new (self->recv_img_hdr[1]));
          break;

        case -3:
          /* -3 sets/clears automatic finger detection for images */
          self->automatic_finger = !!self->recv_img_hdr[1];
          break;

        case -4:
          /* -4 submits a finger detection report */
          fpi_image_device_report_finger_status (FP_IMAGE_DEVICE (self),
                                                 !!self->recv_img_hdr[1]);
          break;

        case -5:
          /* -5 causes the device to disappear (no further data) */
          fpi_device_remove (FP_DEVICE (self));
          break;

        default:
          /* disconnect client, it didn't play fair */
          fpi_device_virtual_listener_connection_close (listener);
        }

      /* And, listen for more images from the same client. */
      recv_image (self);
      return;
    }

  self->recv_img = fp_image_new (self->recv_img_hdr[0], self->recv_img_hdr[1]);
  g_debug ("image data: %p", self->recv_img->data);
  fpi_device_virtual_listener_read (listener,
                                    TRUE,
                                    (guint8 *) self->recv_img->data,
                                    self->recv_img->width * self->recv_img->height,
                                    recv_image_img_recv_cb,
                                    self);
}

static void
recv_image (FpDeviceVirtualImage *self)
{
  fpi_device_virtual_listener_read (self->listener,
                                    TRUE,
                                    self->recv_img_hdr,
                                    sizeof (self->recv_img_hdr),
                                    recv_image_hdr_recv_cb,
                                    self);
}

static void
on_listener_connected (FpiDeviceVirtualListener *listener,
                       gpointer                  user_data)
{
  FpDeviceVirtualImage *self = FPI_DEVICE_VIRTUAL_IMAGE (user_data);
  FpiImageDeviceState state;

  self->automatic_finger = TRUE;

  g_object_get (self,
                "fpi-image-device-state", &state,
                NULL);

  switch (state)
    {
    case FPI_IMAGE_DEVICE_STATE_IDLE:
    case FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON:
    case FPI_IMAGE_DEVICE_STATE_CAPTURE:
    case FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_OFF:
      recv_image (self);

    default:
      break;
    }
}

static void
dev_init (FpImageDevice *dev)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FpiDeviceVirtualListener) listener = NULL;
  g_autoptr(GCancellable) cancellable = NULL;
  FpDeviceVirtualImage *self = FPI_DEVICE_VIRTUAL_IMAGE (dev);

  G_DEBUG_HERE ();

  listener = fpi_device_virtual_listener_new ();
  cancellable = g_cancellable_new ();

  if (!fpi_device_virtual_listener_start (listener,
                                          fpi_device_get_virtual_env (FP_DEVICE (self)),
                                          cancellable,
                                          on_listener_connected,
                                          self,
                                          &error))
    {
      fpi_image_device_open_complete (dev, g_steal_pointer (&error));
      return;
    }

  self->listener = g_steal_pointer (&listener);
  self->cancellable = g_steal_pointer (&cancellable);

  /* Delay result to open up the possibility of testing race conditions. */
  fpi_device_add_timeout (FP_DEVICE (dev), 100, (FpTimeoutFunc) fpi_image_device_open_complete, NULL, NULL);
}

static void
dev_deinit (FpImageDevice *dev)
{
  FpDeviceVirtualImage *self = FPI_DEVICE_VIRTUAL_IMAGE (dev);

  G_DEBUG_HERE ();

  g_cancellable_cancel (self->cancellable);
  g_clear_object (&self->cancellable);
  g_clear_object (&self->listener);

  /* Delay result to open up the possibility of testing race conditions. */
  fpi_device_add_timeout (FP_DEVICE (dev), 100, (FpTimeoutFunc) fpi_image_device_close_complete, NULL, NULL);
}

static void
dev_activate (FpImageDevice *dev)
{
  FpDeviceVirtualImage *self = FPI_DEVICE_VIRTUAL_IMAGE (dev);

  /* Start reading (again). */
  recv_image (self);

  fpi_image_device_activate_complete (dev, NULL);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
dev_notify_removed_cb (FpDevice *dev)
{
  FpiImageDeviceState state;
  gboolean removed;

  g_object_get (dev,
                "fpi-image-device-state", &state,
                "removed", &removed,
                NULL);

  if (!removed || state == FPI_IMAGE_DEVICE_STATE_INACTIVE)
    return;

  /* This error will be converted to an FP_DEVICE_ERROR_REMOVED by the
   * surrounding layers. */
  fpi_image_device_session_error (FP_IMAGE_DEVICE (dev),
                                  fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
}

static void
fpi_device_virtual_image_init (FpDeviceVirtualImage *self)
{
  /* NOTE: This is not nice, but we can generally rely on the underlying
   *       system to throw errors on the transport layer.
   */
  g_signal_connect (self,
                    "notify::removed",
                    G_CALLBACK (dev_notify_removed_cb),
                    NULL);
}

static const FpIdEntry driver_ids[] = {
  { .virtual_envvar = "FP_VIRTUAL_IMAGE" },
  { .virtual_envvar = NULL }
};

static void
fpi_device_virtual_image_class_init (FpDeviceVirtualImageClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = FP_COMPONENT;
  dev_class->full_name = "Virtual image device for debugging";
  dev_class->type = FP_DEVICE_TYPE_VIRTUAL;
  dev_class->id_table = driver_ids;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;

  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;
}
