/*
 * AuthenTec AES3500/AES4000 common routines
 *
 * The AES3500 and AES4000 sensors are press-typed, and could capture
 * fingerprint images in 128x128 and 96x96 pixels respectively. They
 * share a same communication interface: a number of frames are
 * transferred and captured, from which a final image could be
 * assembled. Each frame has fixed height of 16 pixels.
 *
 * As the imaging area is a bit small, only a part of finger could be
 * captured, the detected minutiae are not so many that the NBIS
 * matching works not so good. The verification rate is very low at the
 * moment.
 *
 * This work is derived from Daniel Drake's AES4000 driver.
 *
 * Copyright (C) 2013 Juvenn Woo <machese@gmail.com>
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#define FP_COMPONENT "aes3k"

#include "drivers_api.h"
#include "aeslib.h"
#include "aes3k.h"

typedef struct
{
  /* This is used both as a flag that we are in a capture operation
   * and for cancellation.
   */
  GCancellable *img_capture_cancel;
} FpiDeviceAes3kPrivate;

#define CTRL_TIMEOUT 1000
#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)

static void do_capture (FpImageDevice *dev);

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (FpiDeviceAes3k, fpi_device_aes3k, FP_TYPE_IMAGE_DEVICE);

static void
aes3k_assemble_image (unsigned char *input, size_t width, size_t height,
                      unsigned char *output)
{
  size_t row, column;

  for (column = 0; column < width; column++)
    {
      for (row = 0; row < height; row += 2)
        {
          output[width * row + column] = (*input & 0x0f) * 17;
          output[width * (row + 1) + column] = ((*input & 0xf0) >> 4) * 17;
          input++;
        }
    }
}

static void
img_cb (FpiUsbTransfer *transfer, FpDevice *device,
        gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (device);
  FpiDeviceAes3kPrivate *priv = fpi_device_aes3k_get_instance_private (self);
  FpiDeviceAes3kClass *cls = FPI_DEVICE_AES3K_GET_CLASS (self);
  unsigned char *ptr = transfer->buffer;
  FpImage *tmp;
  FpImage *img;
  int i;

  /* Image capture operation is finished (error/completed) */
  g_clear_object (&priv->img_capture_cancel);

  if (error)
    {
      if (g_error_matches (error,
                           G_IO_ERROR,
                           G_IO_ERROR_CANCELLED))
        {
          /* Cancellation implies we are deactivating. */
          g_error_free (error);
          fpi_image_device_deactivate_complete (dev, NULL);
          return;
        }

      fpi_image_device_session_error (dev, error);
      return;
    }

  fpi_image_device_report_finger_status (dev, TRUE);

  tmp = fp_image_new (cls->frame_width, cls->frame_width);
  tmp->width = cls->frame_width;
  tmp->height = cls->frame_width;
  tmp->flags = FPI_IMAGE_COLORS_INVERTED | FPI_IMAGE_V_FLIPPED | FPI_IMAGE_H_FLIPPED;
  for (i = 0; i < cls->frame_number; i++)
    {
      fp_dbg ("frame header byte %02x", *ptr);
      ptr++;
      aes3k_assemble_image (ptr, cls->frame_width, AES3K_FRAME_HEIGHT, tmp->data + (i * cls->frame_width * AES3K_FRAME_HEIGHT));
      ptr += cls->frame_size;
    }

  /* FIXME: this is an ugly hack to make the image big enough for NBIS
   * to process reliably */
  img = fpi_image_resize (tmp, cls->enlarge_factor, cls->enlarge_factor);
  g_object_unref (tmp);
  fpi_image_device_image_captured (dev, img);

  /* FIXME: rather than assuming finger has gone, we should poll regs until
   * it really has. */
  fpi_image_device_report_finger_status (dev, FALSE);

  /* Note: The transfer is re-started when we switch to the AWAIT_FINGER_ON state. */
}

static void
do_capture (FpImageDevice *dev)
{
  g_autoptr(FpiUsbTransfer) img_trf = NULL;
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kPrivate *priv = fpi_device_aes3k_get_instance_private (self);
  FpiDeviceAes3kClass *cls = FPI_DEVICE_AES3K_GET_CLASS (self);

  img_trf = fpi_usb_transfer_new (FP_DEVICE (dev));
  fpi_usb_transfer_fill_bulk (img_trf, EP_IN, cls->data_buflen);
  img_trf->short_is_error = TRUE;
  fpi_usb_transfer_submit (g_steal_pointer (&img_trf), 0,
                           priv->img_capture_cancel,
                           img_cb, NULL);
}

static void
capture_reqs_cb (FpImageDevice *dev, GError *result, void *user_data)
{
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kPrivate *priv = fpi_device_aes3k_get_instance_private (self);

  if (result)
    {
      g_clear_object (&priv->img_capture_cancel);
      fpi_image_device_session_error (dev, result);
      return;
    }

  /* FIXME: we never cancel a pending capture. So we are likely leaving the
   * hardware in a bad state should we abort the capture operation and the
   * user does not touch the device.
   * But, we don't know how we might cancel, so just leave it as is. */
  do_capture (dev);
}

static void
do_capture_start (FpImageDevice *dev)
{
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kClass *cls = FPI_DEVICE_AES3K_GET_CLASS (self);

  aes_write_regv (dev, cls->capture_reqs, cls->capture_reqs_len, capture_reqs_cb, NULL);
}

static void
init_reqs_cb (FpImageDevice *dev, GError *result, void *user_data)
{
  fpi_image_device_activate_complete (dev, result);
}

static void
aes3k_dev_activate (FpImageDevice *dev)
{
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kClass *cls = FPI_DEVICE_AES3K_GET_CLASS (self);

  aes_write_regv (dev, cls->init_reqs, cls->init_reqs_len, init_reqs_cb, NULL);
}

static void
aes3k_dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kPrivate *priv = fpi_device_aes3k_get_instance_private (self);

  /* If a capture is running, then deactivation finishes from the cancellation handler */
  if (priv->img_capture_cancel)
    g_cancellable_cancel (priv->img_capture_cancel);
  else
    fpi_image_device_deactivate_complete (dev, NULL);
}

static void
aes3k_dev_change_state (FpImageDevice *dev, FpiImageDeviceState state)
{
  FpiDeviceAes3k *self = FPI_DEVICE_AES3K (dev);
  FpiDeviceAes3kPrivate *priv = fpi_device_aes3k_get_instance_private (self);

  if (state == FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON)
    {
      g_assert (!priv->img_capture_cancel);
      priv->img_capture_cancel = g_cancellable_new ();

      do_capture_start (dev);
    }
}

static void
fpi_device_aes3k_init (FpiDeviceAes3k *self)
{
}

static void
aes3k_dev_init (FpImageDevice *dev)
{
  GError *error = NULL;

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  fpi_image_device_open_complete (dev, NULL);
}

static void
aes3k_dev_deinit (FpImageDevice *dev)
{
  GError *error = NULL;

  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);
  fpi_image_device_close_complete (dev, error);
}


static void
fpi_device_aes3k_class_init (FpiDeviceAes3kClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->scan_type = FP_SCAN_TYPE_PRESS;

  img_class->img_open = aes3k_dev_init;
  img_class->img_close = aes3k_dev_deinit;
  img_class->activate = aes3k_dev_activate;
  img_class->change_state = aes3k_dev_change_state;
  img_class->deactivate = aes3k_dev_deactivate;

  /* Extremely low due to low image quality. */
  img_class->bz3_threshold = 9;

  /* Everything else is set by the subclasses. */
}
