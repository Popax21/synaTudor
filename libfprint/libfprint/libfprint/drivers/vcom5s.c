/*
 * Veridicom 5thSense driver for libfprint
 * Copyright (C) 2008 Daniel Drake <dsd@gentoo.org>
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

#define FP_COMPONENT "vcom5s"

#include "drivers_api.h"

/* TODO:
 * calibration?
 * image size: windows gets 300x300 through vpas enrollment util?
 * (probably just increase bulk read size?)
 * powerdown? does windows do anything special on exit?
 */

#define CTRL_TIMEOUT 1000
#define EP_IN (1 | FPI_USB_ENDPOINT_IN)

#define IMG_WIDTH 300
#define IMG_HEIGHT 288
#define ROWS_PER_RQ 12
#define NR_REQS (IMG_HEIGHT / ROWS_PER_RQ)
#define RQ_SIZE (IMG_WIDTH * ROWS_PER_RQ)
#define IMG_SIZE (IMG_WIDTH * IMG_HEIGHT)

struct _FpDeviceVcom5s
{
  FpImageDevice parent;

  int           capture_iteration;
  FpImage      *capture_img;
  gboolean      loop_running;
  gboolean      deactivating;
};
G_DECLARE_FINAL_TYPE (FpDeviceVcom5s, fpi_device_vcom5s, FPI, DEVICE_VCOM5S,
                      FpImageDevice);
G_DEFINE_TYPE (FpDeviceVcom5s, fpi_device_vcom5s, FP_TYPE_IMAGE_DEVICE);

enum v5s_regs {
  /* when using gain 0x29:
   * a value of 0x00 produces mostly-black image
   * 0x09 destroys ridges (too white)
   * 0x01 or 0x02 seem good values */
  REG_CONTRAST = 0x02,

  /* when using contrast 0x01:
   * a value of 0x00 will produce an all-black image.
   * 0x29 produces a good contrast image: ridges quite dark, but some
   * light grey noise as background
   * 0x46 produces all-white image with grey ridges (not very dark) */
  REG_GAIN = 0x03,
};

enum v5s_cmd {
  /* scan one row. has parameter, at a guess this is which row to scan? */
  CMD_SCAN_ONE_ROW = 0xc0,

  /* scan whole image */
  CMD_SCAN = 0xc1,
};

/***** REGISTER I/O *****/

static void
sm_write_reg (FpiSsm       *ssm,
              FpDevice     *dev,
              unsigned char reg,
              unsigned char value)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

  fp_dbg ("set %02x=%02x", reg, value);
  fpi_usb_transfer_fill_control (transfer,
                                 G_USB_DEVICE_DIRECTION_HOST_TO_DEVICE,
                                 G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                 G_USB_DEVICE_RECIPIENT_DEVICE,
                                 reg, value, 0, 0);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL,
                           fpi_ssm_usb_transfer_cb, NULL);
}

static void
sm_exec_cmd (FpiSsm       *ssm,
             FpDevice     *dev,
             unsigned char cmd,
             unsigned char param)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

  fp_dbg ("cmd %02x param %02x", cmd, param);
  fpi_usb_transfer_fill_control (transfer,
                                 G_USB_DEVICE_DIRECTION_DEVICE_TO_HOST,
                                 G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                 G_USB_DEVICE_RECIPIENT_DEVICE,
                                 cmd, param, 0, 0);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL,
                           fpi_ssm_usb_transfer_cb, NULL);
}

/***** FINGER DETECTION *****/

/* We take 64x64 pixels at the center of the image, determine the average
 * pixel intensity, and threshold it. */
#define DETBOX_ROW_START 111
#define DETBOX_COL_START 117
#define DETBOX_ROWS 64
#define DETBOX_COLS 64
#define DETBOX_ROW_END (DETBOX_ROW_START + DETBOX_ROWS)
#define DETBOX_COL_END (DETBOX_COL_START + DETBOX_COLS)
#define FINGER_PRESENCE_THRESHOLD 100

static gboolean
finger_is_present (unsigned char *data)
{
  int row;
  guint16 imgavg = 0;

  for (row = DETBOX_ROW_START; row < DETBOX_ROW_END; row++)
    {
      unsigned char *rowdata = data + (row * IMG_WIDTH);
      guint16 rowavg = 0;
      int col;

      for (col = DETBOX_COL_START; col < DETBOX_COL_END; col++)
        rowavg += rowdata[col];
      rowavg /= DETBOX_COLS;
      imgavg += rowavg;
    }
  imgavg /= DETBOX_ROWS;
  fp_dbg ("img avg %d", imgavg);

  return imgavg <= FINGER_PRESENCE_THRESHOLD;
}



/***** IMAGE ACQUISITION *****/

static void capture_iterate (FpiSsm   *ssm,
                             FpDevice *dev);

static void
capture_cb (FpiUsbTransfer *transfer, FpDevice *device,
            gpointer user_data, GError *error)
{
  FpImageDevice *imgdev = FP_IMAGE_DEVICE (device);
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (device);

  if (error)
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  if (++self->capture_iteration == NR_REQS)
    {
      FpImage *img = self->capture_img;
      /* must clear this early, otherwise the call chain takes us into
       * loopsm_complete where we would free it, when in fact we are
       * supposed to be handing off this image */
      self->capture_img = NULL;

      fpi_image_device_report_finger_status (imgdev,
                                             finger_is_present (img->data));
      fpi_image_device_image_captured (imgdev, img);
      fpi_ssm_next_state (transfer->ssm);
    }
  else
    {
      capture_iterate (transfer->ssm, device);
    }
}

static void
capture_iterate (FpiSsm   *ssm,
                 FpDevice *dev)
{
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);
  int iteration = self->capture_iteration;
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk_full (transfer,
                                   EP_IN,
                                   self->capture_img->data + (RQ_SIZE * iteration),
                                   RQ_SIZE,
                                   NULL);

  fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL, capture_cb, NULL);
}


static void
sm_do_capture (FpiSsm   *ssm,
               FpDevice *dev)
{
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);
  FpImageDeviceClass *cls = FP_IMAGE_DEVICE_GET_CLASS (dev);

  G_DEBUG_HERE ();
  self->capture_img = fp_image_new (cls->img_width, cls->img_height);
  self->capture_iteration = 0;
  capture_iterate (ssm, dev);
}

/***** CAPTURE LOOP *****/

enum loop_states {
  LOOP_SET_CONTRAST,
  LOOP_SET_GAIN,
  LOOP_CMD_SCAN,
  LOOP_CAPTURE,
  LOOP_CAPTURE_DONE,
  LOOP_NUM_STATES,
};

static void
loop_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case LOOP_SET_CONTRAST:
      sm_write_reg (ssm, dev, REG_CONTRAST, 0x01);
      break;

    case LOOP_SET_GAIN:
      sm_write_reg (ssm, dev, REG_GAIN, 0x29);
      break;

    case LOOP_CMD_SCAN:
      if (self->deactivating)
        {
          fp_dbg ("deactivating, marking completed");
          fpi_ssm_mark_completed (ssm);
        }
      else
        {
          sm_exec_cmd (ssm, dev, CMD_SCAN, 0x00);
        }
      break;

    case LOOP_CAPTURE:
      sm_do_capture (ssm, dev);
      break;

    case LOOP_CAPTURE_DONE:
      fpi_ssm_jump_to_state (ssm, LOOP_CMD_SCAN);
      break;

    default:
      g_assert_not_reached ();
    }
}

static void
loopsm_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpImageDevice *imgdev = FP_IMAGE_DEVICE (dev);
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);

  g_object_unref (self->capture_img);
  self->capture_img = NULL;
  self->loop_running = FALSE;

  if (error && !self->deactivating)
    fpi_image_device_session_error (imgdev, error);
  else if (error)
    g_error_free (error);

  if (self->deactivating)
    fpi_image_device_deactivate_complete (imgdev, NULL);
}

static void
dev_activate (FpImageDevice *dev)
{
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), loop_run_state, LOOP_NUM_STATES);

  self->deactivating = FALSE;
  fpi_ssm_start (ssm, loopsm_complete);
  self->loop_running = TRUE;
  fpi_image_device_activate_complete (dev, NULL);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpDeviceVcom5s *self = FPI_DEVICE_VCOM5S (dev);

  if (self->loop_running)
    self->deactivating = TRUE;
  else
    fpi_image_device_deactivate_complete (dev, NULL);
}

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;

  g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error);

  fpi_image_device_open_complete (dev, error);
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
  { .vid = 0x061a,  .pid = 0x0110, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_vcom5s_init (FpDeviceVcom5s *self)
{
}

static void
fpi_device_vcom5s_class_init (FpDeviceVcom5sClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "vcom5s";
  dev_class->full_name = "Veridicom 5thSense";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_PRESS;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->img_width = IMG_WIDTH;
  img_class->img_height = IMG_HEIGHT;
}
