/*
 * AuthenTec AES2550/AES2810 driver for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2007 Cyrille Bagard
 * Copyright (C) 2007-2012 Vasily Khoruzhick
 *
 * Based on AES2501 driver
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

#define FP_COMPONENT "aes2550"

#include "drivers_api.h"
#include "aes2550.h"
#include "aeslib.h"

static void start_capture (FpImageDevice *dev);
static void complete_deactivation (FpImageDevice *dev);

#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)
#define BULK_TIMEOUT 4000

/*
 * The AES2550 is an imaging device using a swipe-type sensor. It samples
 * the finger at preprogrammed intervals, sending a 192x16 frame to the
 * computer.
 * Unless the user is scanning their finger unreasonably fast, the frames
 * *will* overlap. The implementation below detects this overlap and produces
 * a contiguous image as the end result.
 * The fact that the user determines the length of the swipe (and hence the
 * number of useful frames) and also the fact that overlap varies means that
 * images returned from this driver vary in height.
 */

#define FRAME_WIDTH 192
#define FRAME_HEIGHT 8
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT)
#define IMAGE_WIDTH (FRAME_WIDTH + (FRAME_WIDTH / 2))

struct _FpiDeviceAes2550
{
  FpImageDevice parent;

  GSList       *strips;
  size_t        strips_len;
  gboolean      deactivating;
  int           heartbeat_cnt;
};
G_DECLARE_FINAL_TYPE (FpiDeviceAes2550, fpi_device_aes2550, FPI, DEVICE_AES2550,
                      FpImageDevice);
G_DEFINE_TYPE (FpiDeviceAes2550, fpi_device_aes2550, FP_TYPE_IMAGE_DEVICE);

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = FRAME_WIDTH,
  .frame_height = FRAME_HEIGHT,
  .image_width = IMAGE_WIDTH,
  .get_pixel = aes_get_pixel,
};

/****** FINGER PRESENCE DETECTION ******/

static unsigned char finger_det_reqs[] = {
  0x80, AES2550_REG80_MASTER_RESET,
  0x95, (8 << AES2550_REG95_COL_SCANNED_OFS) | (1 << AES2550_REG95_EPIX_AVG_OFS),
  0xad, 0x00,
  0xbd, (0 << AES2550_REGBD_LPO_IN_15_8_OFS),
  0xbe, (0 << AES2550_REGBE_LPO_IN_7_0_OFS),
  0xcf, AES2550_REGCF_INTERFERENCE_CHK_EN,
  AES2550_CMD_HEARTBEAT, 0x00, 0x01, 0x00,       /* Heart beat off */
  AES2550_CMD_RUN_FD,
};

static void start_finger_detection (FpImageDevice *dev);

static void
finger_det_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  unsigned char *data = transfer->buffer;

  if (error)
    {
      fpi_image_device_session_error (FP_IMAGE_DEVICE (device), error);
      return;
    }

  fp_dbg ("transfer completed, len: %.4x, data: %.2x %.2x",
          (gint) transfer->actual_length, (int) data[0], (int) data[1]);

  /* Check if we got 2 bytes, reg address 0x83 and its value */
  if ((transfer->actual_length >= 2) && (data[0] == 0x83) && (data[1] & AES2550_REG83_FINGER_PRESENT))
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
finger_det_reqs_cb (FpiUsbTransfer *t, FpDevice *device,
                    gpointer user_data, GError *error)
{
  FpiUsbTransfer *transfer;
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);

  if (error)
    {
      fpi_image_device_session_error (dev, error);
      return;
    }

  transfer = fpi_usb_transfer_new (device);
  /* 2 bytes of result */
  fpi_usb_transfer_fill_bulk (transfer, EP_IN, AES2550_EP_IN_BUF_SIZE);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_data_cb, NULL);
}

static void
start_finger_detection (FpImageDevice *dev)
{
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);
  FpiUsbTransfer *transfer;

  G_DEBUG_HERE ();

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT, finger_det_reqs,
                                   sizeof (finger_det_reqs), NULL);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_reqs_cb, NULL);
}

/****** CAPTURE ******/

static unsigned char capture_reqs[] = {
  0x80, AES2550_REG80_MASTER_RESET,
  0x80, (1 << AES2550_REG80_SENSOR_MODE_OFS) | (AES2550_REG80_HGC_ENABLE),
  0x85, AES2550_REG85_FLUSH_PER_FRAME,
  0x8f, AES2550_REG8F_AUTH_DISABLE | AES2550_REG8F_EHISTO_DISABLE,
  0xbf, AES2550_REGBF_RSR_DIR_UPDOWN_MOTION | AES2550_REGBF_RSR_LEVEL_SUPER_RSR,
  0xcf, (3 << AES2550_REGCF_INTERFERENCE_AVG_OFFS) | AES2550_REGCF_INTERFERENCE_AVG_EN,
  0xdc, (1 << AES2550_REGDC_BP_NUM_REF_SWEEP_OFS),
  AES2550_CMD_HEARTBEAT, 0x00, 0x01, 0x03,       /* Heart beat cmd, 3 * 16 cycles without sending image */
  AES2550_CMD_GET_ENROLL_IMG,
};

static unsigned char capture_set_idle_reqs[] = {
  0x80, AES2550_REG80_MASTER_RESET,
  AES2550_CMD_HEARTBEAT, 0x00, 0x01, 0x00,       /* Heart beat off */
  AES2550_CMD_SET_IDLE_MODE,
};

enum capture_states {
  CAPTURE_WRITE_REQS,
  CAPTURE_READ_DATA,
  CAPTURE_SET_IDLE,
  CAPTURE_NUM_STATES,
};

/* Returns number of processed bytes */
static gboolean
process_strip_data (FpiSsm *ssm, FpImageDevice *dev,
                    unsigned char *data)
{
  unsigned char *stripdata;
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);
  struct fpi_frame *stripe;
  int len;

  if (data[0] != AES2550_EDATA_MAGIC)
    {
      fp_dbg ("Bogus magic: %.2x", (int) (data[0]));
      return FALSE;
    }
  len = data[1] * 256 + data[2];
  if (len != (AES2550_STRIP_SIZE - 3))
    fp_dbg ("Bogus frame len: %.4x", len);
  stripe = g_malloc0 (FRAME_WIDTH * FRAME_HEIGHT / 2 + sizeof (struct fpi_frame));     /* 4 bits per pixel */
  stripe->delta_x = (int8_t) data[6];
  stripe->delta_y = -(int8_t) data[7];
  stripdata = stripe->data;
  memcpy (stripdata, data + 33, FRAME_WIDTH * FRAME_HEIGHT / 2);
  self->strips = g_slist_prepend (self->strips, stripe);
  self->strips_len++;

  fp_dbg ("deltas: %dx%d", stripe->delta_x, stripe->delta_y);

  return TRUE;
}

static void
capture_set_idle_reqs_cb (FpiUsbTransfer *transfer,
                          FpDevice *device, gpointer user_data,
                          GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);

  if (!error && self->strips_len)
    {
      FpImage *img;

      self->strips = g_slist_reverse (self->strips);
      img = fpi_assemble_frames (&assembling_ctx, self->strips);
      img->flags |= FPI_IMAGE_PARTIAL;
      g_slist_free_full (self->strips, g_free);
      self->strips = NULL;
      self->strips_len = 0;
      fpi_image_device_image_captured (dev, img);
      fpi_image_device_report_finger_status (dev, FALSE);
      /* marking machine complete will re-trigger finger detection loop */
      fpi_ssm_mark_completed (transfer->ssm);
    }
  else
    {
      if (error)
        fpi_ssm_mark_failed (transfer->ssm, error);
      else
        fpi_ssm_mark_failed (transfer->ssm,
                             fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
    }
}

static void
capture_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                      gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);
  unsigned char *data = transfer->buffer;

  if (error)
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  fp_dbg ("request completed, len: %.4x", (gint) transfer->actual_length);
  if (transfer->actual_length >= 2)
    fp_dbg ("data: %.2x %.2x", (int) data[0], (int) data[1]);

  switch (transfer->actual_length)
    {
    case AES2550_STRIP_SIZE:
      if (!process_strip_data (transfer->ssm, dev, data))
        {
          fp_dbg ("Processing strip data failed");
          fpi_ssm_mark_failed (transfer->ssm,
                               fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          return;
        }
      self->heartbeat_cnt = 0;
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA);
      break;

    case AES2550_HEARTBEAT_SIZE:
      if (data[0] == AES2550_HEARTBEAT_MAGIC)
        {
          /* No data for a long time => finger was removed or there's no movement */
          self->heartbeat_cnt++;
          if (self->heartbeat_cnt == 3)
            {
              /* Got 3 heartbeat message, that's enough to consider that finger was removed,
               * assemble image and submit it to the library */
              fp_dbg ("Got 3 heartbeats => finger removed");
              fpi_ssm_next_state (transfer->ssm);
            }
          else
            {
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_READ_DATA);
            }
        }
      break;

    default:
      fp_dbg ("Short frame %d, skip",
              (gint) transfer->actual_length);
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA);
      break;
    }
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_WRITE_REQS:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT, capture_reqs,
                                         sizeof (capture_reqs), NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case CAPTURE_READ_DATA:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk (transfer, EP_IN, AES2550_EP_IN_BUF_SIZE);
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 capture_read_data_cb, NULL);
      }
      break;

    case CAPTURE_SET_IDLE:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT,
                                         capture_set_idle_reqs,
                                         sizeof (capture_set_idle_reqs),
                                         NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 capture_set_idle_reqs_cb, NULL);
      }
      break;
    }
  ;
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (_dev);
  FpImageDevice *dev = FP_IMAGE_DEVICE (self);

  fp_dbg ("Capture completed");

  if (self->deactivating)
    {
      complete_deactivation (dev);
      g_clear_pointer (&error, g_error_free);
    }
  else if (error)
    {
      fpi_image_device_session_error (dev, error);
    }
  else
    {
      start_finger_detection (dev);
    }
}

static void
start_capture (FpImageDevice *dev)
{
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);
  FpiSsm *ssm;

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  self->heartbeat_cnt = 0;
  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state, CAPTURE_NUM_STATES);
  G_DEBUG_HERE ();
  fpi_ssm_start (ssm, capture_sm_complete);
}

/****** INITIALIZATION/DEINITIALIZATION ******/

static unsigned char init_reqs[] = {
  0x80, AES2550_REG80_MASTER_RESET,       /* Master reset */
  0x80, (1 << AES2550_REG80_SENSOR_MODE_OFS) | (AES2550_REG80_FORCE_FINGER_PRESENT),
  0x85, AES2550_REG85_FLUSH_PER_FRAME,
  0xa8, AES2550_REGA8_DIG_BIT_EN,
  0x81, AES2550_REG81_NSHOT,
};

static unsigned char calibrate_reqs[] = {
  0x80, AES2550_REG80_MASTER_RESET,       /* Master reset */
  AES2550_CMD_CALIBRATE,
  AES2550_CMD_READ_CALIBRATION_DATA,
};

enum activate_states {
  WRITE_INIT,
  READ_DATA,
  CALIBRATE,
  READ_CALIB_TABLE,
  ACTIVATE_NUM_STATES,
};

/* TODO: use calibration table, datasheet is rather terse on that
 * need more info for implementation */
static void
calibrate_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                        gpointer user_data, GError *error)
{
  fpi_ssm_usb_transfer_cb (transfer, device, user_data, error);
}

static void
activate_run_state (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case WRITE_INIT:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT, init_reqs,
                                         sizeof (init_reqs), NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case READ_DATA:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk (transfer, EP_IN, AES2550_EP_IN_BUF_SIZE);
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case CALIBRATE:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT,
                                         calibrate_reqs,
                                         sizeof (calibrate_reqs), NULL);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case READ_CALIB_TABLE:
      {
        FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_bulk (transfer, EP_IN, AES2550_EP_IN_BUF_SIZE);
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 calibrate_read_data_cb, NULL);
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

static void
dev_activate (FpImageDevice *dev)
{
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);

  self->deactivating = TRUE;
}

static void
complete_deactivation (FpImageDevice *dev)
{
  FpiDeviceAes2550 *self = FPI_DEVICE_AES2550 (dev);

  G_DEBUG_HERE ();

  self->deactivating = FALSE;
  g_slist_free (self->strips);
  self->strips = NULL;
  self->strips_len = 0;
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;

  /* TODO check that device has endpoints we're using */

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
  { .vid = 0x08ff,  .pid = 0x2550, },/* AES2550 */
  { .vid = 0x08ff,  .pid = 0x2810, },/* AES2810 */
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_aes2550_init (FpiDeviceAes2550 *self)
{
}
static void
fpi_device_aes2550_class_init (FpiDeviceAes2550Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "aes2550";
  dev_class->full_name = "AuthenTec AES2550/AES2810";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->img_width = FRAME_WIDTH + FRAME_WIDTH / 2;
  img_class->img_height = -1;
}
