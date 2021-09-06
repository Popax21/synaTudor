/*
 * UPEK TouchChip driver for libfprint
 * Copyright (C) 2007 Jan-Michael Brummer <buzz2@gmx.de>
 * Copyright (C) 2012 Vasily Khoruzhick <anarsoul@gmail.com>
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

#define FP_COMPONENT "upektc"

#include "drivers_api.h"
#include "upektc.h"

#define UPEKTC_EP_IN (2 | FPI_USB_ENDPOINT_IN)
#define UPEKTC_EP_OUT (3 | FPI_USB_ENDPOINT_OUT)
#define UPEKET_EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define UPEKET_EP_OUT (2 | FPI_USB_ENDPOINT_OUT)
#define BULK_TIMEOUT 4000

struct _FpiDeviceUpektc
{
  FpImageDevice           parent;

  gboolean                deactivating;
  const struct setup_cmd *setup_commands;
  size_t                  setup_commands_len;
  int                     ep_in;
  int                     ep_out;
  int                     init_idx;
  int                     sum_threshold;
};
G_DECLARE_FINAL_TYPE (FpiDeviceUpektc, fpi_device_upektc, FPI, DEVICE_UPEKTC,
                      FpImageDevice);
G_DEFINE_TYPE (FpiDeviceUpektc, fpi_device_upektc, FP_TYPE_IMAGE_DEVICE);

enum upektc_driver_data {
  UPEKTC_2015,
  UPEKTC_3001,
};

static void start_capture (FpImageDevice *dev);
static void complete_deactivation (FpImageDevice *dev,
                                   GError        *error);
static void start_finger_detection (FpImageDevice *dev);

/****** INITIALIZATION/DEINITIALIZATION ******/

enum activate_states {
  WRITE_INIT,
  READ_DATA,
  ACTIVATE_NUM_STATES,
};

static void
upektc_next_init_cmd (FpiSsm        *ssm,
                      FpImageDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  self->init_idx += 1;
  if (self->init_idx == self->setup_commands_len)
    fpi_ssm_mark_completed (ssm);
  else
    fpi_ssm_jump_to_state (ssm, WRITE_INIT);
}

static void
write_init_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  if (!error)
    {
      if (self->setup_commands[self->init_idx].response_len)
        fpi_ssm_next_state (transfer->ssm);
      else
        upektc_next_init_cmd (transfer->ssm, dev);
    }
  else
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
    }
}

static void
read_init_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                   gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);

  if (!error)
    upektc_next_init_cmd (transfer->ssm, dev);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

static void
activate_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case WRITE_INIT:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk_full (transfer,
                                         self->ep_out,
                                         (unsigned char *) self->setup_commands[self->init_idx].cmd,
                                         UPEKTC_CMD_LEN,
                                         NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 write_init_cb, NULL);
      }
      break;

    case READ_DATA:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk (transfer,
                                    self->ep_in,
                                    self->setup_commands[self->init_idx].response_len);
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 read_init_data_cb, NULL);
      }
      break;
    }
}

static void
activate_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);

  fpi_image_device_activate_complete (dev, error);

  if (!error)
    start_finger_detection (dev);
}


/****** FINGER PRESENCE DETECTION ******/

static int
finger_present (unsigned char *img, size_t len, int sum_threshold)
{
  int i, sum;

  sum = 0;

  for (i = 0; i < len; i++)
    if (img[i] < 160)
      sum++;

  fp_dbg ("finger_present: sum is %d", sum);
  return sum < sum_threshold ? 0 : 1;
}

static void
finger_det_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  if (error)
    {
      fp_dbg ("data transfer status %s", error->message);
      fpi_image_device_session_error (dev, error);
      return;
    }

  if (finger_present (transfer->buffer, IMAGE_SIZE, self->sum_threshold))
    {
      /* finger present, start capturing */
      fpi_image_device_report_finger_status (dev, TRUE);
      start_capture (dev);
    }
  else
    {
      /* no finger, poll for a new histogram */
      start_finger_detection (dev);
    }
}

static void
finger_det_cmd_cb (FpiUsbTransfer *t, FpDevice *device,
                   gpointer user_data, GError *error)
{
  FpiUsbTransfer *transfer;
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  if (error)
    {
      fp_dbg ("req transfer status %s", error->message);
      fpi_image_device_session_error (dev, error);
      return;
    }

  transfer = fpi_usb_transfer_new (device);
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer, self->ep_in,
                              IMAGE_SIZE);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_data_cb, NULL);
}

static void
start_finger_detection (FpImageDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);
  FpiUsbTransfer *transfer;

  G_DEBUG_HERE ();

  if (self->deactivating)
    {
      complete_deactivation (dev, NULL);
      return;
    }

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk_full (transfer, self->ep_out,
                                   (unsigned char *) scan_cmd,
                                   UPEKTC_CMD_LEN, NULL);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_cmd_cb, NULL);
}

/****** CAPTURE ******/

enum capture_states {
  CAPTURE_WRITE_CMD,
  CAPTURE_READ_DATA,
  CAPTURE_NUM_STATES,
};

static void
capture_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                      gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpImage *img;

  if (error)
    {
      fp_dbg ("request is not completed, %s", error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  img = fp_image_new (IMAGE_WIDTH, IMAGE_HEIGHT);
  memcpy (img->data, transfer->buffer, IMAGE_SIZE);
  fpi_image_device_image_captured (dev, img);
  fpi_image_device_report_finger_status (dev, FALSE);
  fpi_ssm_mark_completed (transfer->ssm);
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_WRITE_CMD:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (_dev);

        fpi_usb_transfer_fill_bulk_full (transfer, self->ep_out,
                                         (unsigned char *) scan_cmd,
                                         UPEKTC_CMD_LEN, NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case CAPTURE_READ_DATA:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (_dev);

        fpi_usb_transfer_fill_bulk (transfer, self->ep_in,
                                    IMAGE_SIZE);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 capture_read_data_cb, NULL);
      }
      break;
    }
  ;
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (_dev);

  fp_dbg ("Capture completed");
  if (self->deactivating)
    complete_deactivation (dev, error);
  else if (error)
    fpi_image_device_session_error (dev, error);
  else
    start_finger_detection (dev);

}

static void
start_capture (FpImageDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);
  FpiSsm *ssm;

  if (self->deactivating)
    {
      complete_deactivation (dev, NULL);
      return;
    }

  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state, CAPTURE_NUM_STATES);
  G_DEBUG_HERE ();
  fpi_ssm_start (ssm, capture_sm_complete);
}

static void
dev_activate (FpImageDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  self->init_idx = 0;
  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  self->deactivating = TRUE;
}

static void
complete_deactivation (FpImageDevice *dev, GError *error)
{
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);

  G_DEBUG_HERE ();

  self->deactivating = FALSE;
  fpi_image_device_deactivate_complete (dev, error);
}

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;
  FpiDeviceUpektc *self = FPI_DEVICE_UPEKTC (dev);
  guint64 driver_data = fpi_device_get_driver_data (FP_DEVICE (dev));

  /* TODO check that device has endpoints we're using */

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  switch (driver_data)
    {
    case UPEKTC_2015:
      self->ep_in = UPEKTC_EP_IN;
      self->ep_out = UPEKTC_EP_OUT;
      self->setup_commands = upektc_setup_commands;
      self->setup_commands_len = G_N_ELEMENTS (upektc_setup_commands);
      self->sum_threshold = UPEKTC_SUM_THRESHOLD;
      break;

    case UPEKTC_3001:
      self->ep_in = UPEKET_EP_IN;
      self->ep_out = UPEKET_EP_OUT;
      self->setup_commands = upeket_setup_commands;
      self->setup_commands_len = G_N_ELEMENTS (upeket_setup_commands);
      self->sum_threshold = UPEKET_SUM_THRESHOLD;
      break;

    default:
      fp_err ("Device variant %lu is not known", driver_data);
      g_assert_not_reached ();
      fpi_image_device_open_complete (dev, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
      return;
    }
  fpi_image_device_open_complete (dev, NULL);
}

static void
dev_deinit (FpImageDevice *dev)
{
  GError *error = NULL;

  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);
  fpi_image_device_close_complete (dev, error);
}

static const FpIdEntry id_table[] = {
  { .vid = 0x0483,  .pid = 0x2015, .driver_data = UPEKTC_2015 },
  { .vid = 0x147e,  .pid = 0x3001, .driver_data = UPEKTC_3001 },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_upektc_init (FpiDeviceUpektc *self)
{
}
static void
fpi_device_upektc_class_init (FpiDeviceUpektcClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "upektc";
  dev_class->full_name = "UPEK TouchChip/Eikon Touch 300";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_PRESS;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 30;

  img_class->img_width = IMAGE_WIDTH;
  img_class->img_height = IMAGE_HEIGHT;
}
