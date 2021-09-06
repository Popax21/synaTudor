/*
 * AuthenTec AES1660/AES2660 common routines
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2007 Cyrille Bagard
 * Copyright (C) 2007-2008,2012 Vasily Khoruzhick
 *
 * Based on AES2550 driver
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

#define FP_COMPONENT "aesX660"

#include "drivers_api.h"
#include "aeslib.h"
#include "aesx660.h"

typedef struct
{
  GByteArray         *stripe_packet;
  GSList             *strips;
  size_t              strips_len;
  gboolean            deactivating;
  struct aesX660_cmd *init_seq;
  size_t              init_seq_len;
  unsigned int        init_cmd_idx;
  unsigned int        init_seq_idx;
} FpiDeviceAesX660Private;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (FpiDeviceAesX660, fpi_device_aes_x660, FP_TYPE_IMAGE_DEVICE);

static void start_capture (FpImageDevice *dev);
static void complete_deactivation (FpImageDevice *dev);

#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)
#define BULK_TIMEOUT 4000
#define FRAME_HEIGHT AESX660_FRAME_HEIGHT

#define ID_LEN 8
#define INIT_LEN 4
#define CALIBRATE_DATA_LEN 4
#define FINGER_DET_DATA_LEN 4

FP_GNUC_ACCESS (read_only, 3, 4)
static void
aesX660_send_cmd_timeout (FpiSsm                *ssm,
                          FpDevice              *_dev,
                          const unsigned char   *cmd,
                          size_t                 cmd_len,
                          FpiUsbTransferCallback callback,
                          int                    timeout)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (_dev);

  fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT, (unsigned char *) cmd,
                                   cmd_len, NULL);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, timeout, NULL, callback, NULL);
}

FP_GNUC_ACCESS (read_only, 3, 4)
static void
aesX660_send_cmd (FpiSsm                *ssm,
                  FpDevice              *dev,
                  const unsigned char   *cmd,
                  size_t                 cmd_len,
                  FpiUsbTransferCallback callback)
{
  return aesX660_send_cmd_timeout (ssm, dev, cmd, cmd_len, callback, BULK_TIMEOUT);
}

static void
aesX660_read_response (FpiSsm                *ssm,
                       FpDevice              *_dev,
                       gboolean               short_is_error,
                       gboolean               cancellable,
                       size_t                 buf_len,
                       FpiUsbTransferCallback callback)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (_dev);
  GCancellable *cancel = NULL;

  if (cancellable)
    cancel = fpi_device_get_cancellable (_dev);

  fpi_usb_transfer_fill_bulk (transfer, EP_IN, buf_len);
  transfer->ssm = ssm;
  transfer->short_is_error = short_is_error;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, cancel, callback, NULL);
}

static void
aesX660_read_calibrate_data_cb (FpiUsbTransfer *transfer,
                                FpDevice *device,
                                gpointer user_data, GError *error)
{
  unsigned char *data = transfer->buffer;

  if (error)
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }
  /* Calibrate response was read correctly? */
  if (data[AESX660_RESPONSE_TYPE_OFFSET] != AESX660_CALIBRATE_RESPONSE)
    {
      fp_dbg ("Bogus calibrate response: %.2x", data[0]);
      fpi_ssm_mark_failed (transfer->ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Bogus calibrate "
                                                     "response: %.2x",
                                                     data[0]));
      return;
    }

  fpi_ssm_next_state (transfer->ssm);
}

/****** FINGER PRESENCE DETECTION ******/

enum finger_det_states {
  FINGER_DET_SEND_LED_CMD,
  FINGER_DET_SEND_FD_CMD,
  FINGER_DET_READ_FD_DATA,
  FINGER_DET_SET_IDLE,
  FINGER_DET_NUM_STATES,
};

static void
finger_det_read_fd_data_cb (FpiUsbTransfer *transfer,
                            FpDevice *device, gpointer user_data,
                            GError *error)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  unsigned char *data = transfer->buffer;

  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
      fpi_ssm_next_state (transfer->ssm);
      return;
    }

  if (error)
    {
      fp_dbg ("Failed to read FD data");
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  if (data[AESX660_RESPONSE_TYPE_OFFSET] != AESX660_FINGER_DET_RESPONSE)
    {
      fp_dbg ("Bogus FD response: %.2x", data[0]);
      fpi_ssm_mark_failed (transfer->ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Bogus FD response %.2x",
                                                     data[0]));
      return;
    }

  if (data[AESX660_FINGER_PRESENT_OFFSET] == AESX660_FINGER_PRESENT || priv->deactivating)
    {
      /* Finger present or we're deactivating... */
      fpi_ssm_next_state (transfer->ssm);
    }
  else
    {
      fp_dbg ("Wait for finger returned %.2x as result",
              data[AESX660_FINGER_PRESENT_OFFSET]);
      fpi_ssm_jump_to_state (transfer->ssm, FINGER_DET_SEND_FD_CMD);
    }
}

static void
finger_det_set_idle_cmd_cb (FpiUsbTransfer *transfer,
                            FpDevice *device, gpointer user_data,
                            GError *error)
{
  if (!error)
    fpi_ssm_mark_completed (transfer->ssm);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

static void
finger_det_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (_dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);

  fp_dbg ("Finger detection completed");
  fpi_image_device_report_finger_status (dev, TRUE);

  if (priv->deactivating)
    {
      complete_deactivation (dev);
      if (error)
        g_error_free (error);
    }
  else if (error)
    {
      fpi_image_device_session_error (dev, error);
    }
  else
    {
      fpi_image_device_report_finger_status (dev, TRUE);
      start_capture (dev);
    }
}

static void
finger_det_run_state (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case FINGER_DET_SEND_LED_CMD:
      aesX660_send_cmd (ssm, dev, led_blink_cmd, sizeof (led_blink_cmd),
                        fpi_ssm_usb_transfer_cb);
      break;

    case FINGER_DET_SEND_FD_CMD:
      aesX660_send_cmd_timeout (ssm, dev, wait_for_finger_cmd, sizeof (wait_for_finger_cmd),
                                fpi_ssm_usb_transfer_cb, 0);
      break;

    case FINGER_DET_READ_FD_DATA:
      aesX660_read_response (ssm, dev, TRUE, TRUE, FINGER_DET_DATA_LEN, finger_det_read_fd_data_cb);
      break;

    case FINGER_DET_SET_IDLE:
      aesX660_send_cmd (ssm, dev, set_idle_cmd, sizeof (set_idle_cmd),
                        finger_det_set_idle_cmd_cb);
      break;
    }
}

static void
start_finger_detection (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiSsm *ssm;

  if (priv->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  ssm = fpi_ssm_new (FP_DEVICE (dev), finger_det_run_state,
                     FINGER_DET_NUM_STATES);
  fpi_ssm_start (ssm, finger_det_sm_complete);
}

/****** CAPTURE ******/

enum capture_states {
  CAPTURE_SEND_LED_CMD,
  CAPTURE_SEND_CAPTURE_CMD,
  CAPTURE_READ_STRIPE_DATA,
  CAPTURE_SET_IDLE,
  CAPTURE_NUM_STATES,
};

/* Returns number of processed bytes */
static int
process_stripe_data (FpiSsm *ssm, FpiDeviceAesX660 *self,
                     unsigned char *data, gsize length)
{
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiDeviceAesX660Class *cls = FPI_DEVICE_AES_X660_GET_CLASS (self);
  struct fpi_frame *stripe;
  unsigned char *stripdata;

  if (length < AESX660_IMAGE_OFFSET + cls->assembling_ctx->frame_width * FRAME_HEIGHT / 2)
    {
      fp_warn ("Received stripe data is too short, got %zi expected %i bytes!",
               length,
               AESX660_IMAGE_OFFSET + cls->assembling_ctx->frame_width * FRAME_HEIGHT / 2);
      return 0;
    }

  stripe = g_malloc (cls->assembling_ctx->frame_width * FRAME_HEIGHT / 2 + sizeof (struct fpi_frame));     /* 4 bpp */
  stripdata = stripe->data;

  fp_dbg ("Processing frame %.2x %.2x", data[AESX660_IMAGE_OK_OFFSET],
          data[AESX660_LAST_FRAME_OFFSET]);

  stripe->delta_x = (int8_t) data[AESX660_FRAME_DELTA_X_OFFSET];
  stripe->delta_y = -(int8_t) data[AESX660_FRAME_DELTA_Y_OFFSET];
  fp_dbg ("Offset to previous frame: %d %d", stripe->delta_x, stripe->delta_y);

  if (data[AESX660_IMAGE_OK_OFFSET] == AESX660_IMAGE_OK)
    {
      memcpy (stripdata, data + AESX660_IMAGE_OFFSET, cls->assembling_ctx->frame_width * FRAME_HEIGHT / 2);

      priv->strips = g_slist_prepend (priv->strips, stripe);
      priv->strips_len++;
      return data[AESX660_LAST_FRAME_OFFSET] & AESX660_LAST_FRAME_BIT;
    }

  g_free (stripe);
  return 0;
}

static void
capture_set_idle_cmd_cb (FpiUsbTransfer *transfer, FpDevice *device,
                         gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiDeviceAesX660Class *cls = FPI_DEVICE_AES_X660_GET_CLASS (self);

  if (!error)
    {
      FpImage *img;

      priv->strips = g_slist_reverse (priv->strips);
      img = fpi_assemble_frames (cls->assembling_ctx, priv->strips);
      img->flags |= FPI_IMAGE_PARTIAL;
      g_slist_foreach (priv->strips, (GFunc) g_free, NULL);
      g_slist_free (priv->strips);
      priv->strips = NULL;
      priv->strips_len = 0;
      fpi_image_device_image_captured (dev, img);
      fpi_image_device_report_finger_status (dev, FALSE);
      fpi_ssm_mark_completed (transfer->ssm);
    }
  else
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
    }
}

static void
capture_read_stripe_data_cb (FpiUsbTransfer *transfer,
                             FpDevice *device, gpointer user_data,
                             GError *error)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  unsigned char *data = transfer->buffer;
  int finger_missing = 0;
  size_t actual_length = transfer->actual_length;

  if (error)
    {
      g_byte_array_set_size (priv->stripe_packet, 0);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  fp_dbg ("Got %lu bytes of data", actual_length);
  while (actual_length)
    {
      gssize payload_length;
      gssize still_needed_len;
      gssize copy_len;

      still_needed_len = MAX (0, AESX660_HEADER_SIZE - (gssize) priv->stripe_packet->len);
      copy_len = MIN (actual_length, still_needed_len);
      g_byte_array_append (priv->stripe_packet, data, copy_len);
      data += copy_len;
      actual_length -= copy_len;

      /* Do we have a full header? */
      if (priv->stripe_packet->len < AESX660_HEADER_SIZE)
        break;

      payload_length = priv->stripe_packet->data[AESX660_RESPONSE_SIZE_LSB_OFFSET] +
                       (priv->stripe_packet->data[AESX660_RESPONSE_SIZE_MSB_OFFSET] << 8);
      fp_dbg ("Got frame, type %.2x payload of size %.4lx",
              priv->stripe_packet->data[AESX660_RESPONSE_TYPE_OFFSET],
              payload_length);

      still_needed_len = MAX (0, AESX660_HEADER_SIZE + payload_length - (gssize) priv->stripe_packet->len);
      copy_len = MIN (actual_length, still_needed_len);
      g_byte_array_append (priv->stripe_packet, data, copy_len);
      data += copy_len;
      actual_length -= copy_len;

      /* Do we have a full packet including the payload? */
      if (priv->stripe_packet->len < payload_length + AESX660_HEADER_SIZE)
        break;

      finger_missing |= process_stripe_data (transfer->ssm,
                                             self,
                                             priv->stripe_packet->data,
                                             priv->stripe_packet->len);

      g_byte_array_set_size (priv->stripe_packet, 0);
    }

  fp_dbg ("finger %s", finger_missing ? "missing" : "present");

  if (finger_missing)
    fpi_ssm_next_state (transfer->ssm);
  else
    fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_STRIPE_DATA);
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (_dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiDeviceAesX660Class *cls = FPI_DEVICE_AES_X660_GET_CLASS (self);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_SEND_LED_CMD:
      aesX660_send_cmd (ssm, _dev, led_solid_cmd, sizeof (led_solid_cmd),
                        fpi_ssm_usb_transfer_cb);
      break;

    case CAPTURE_SEND_CAPTURE_CMD:
      g_byte_array_set_size (priv->stripe_packet, 0);
      aesX660_send_cmd (ssm, _dev, cls->start_imaging_cmd,
                        cls->start_imaging_cmd_len,
                        fpi_ssm_usb_transfer_cb);
      break;

    case CAPTURE_READ_STRIPE_DATA:
      aesX660_read_response (ssm, _dev, FALSE, FALSE, AESX660_BULK_TRANSFER_SIZE,
                             capture_read_stripe_data_cb);
      break;

    case CAPTURE_SET_IDLE:
      fp_dbg ("Got %lu frames", priv->strips_len);
      aesX660_send_cmd (ssm, _dev, set_idle_cmd, sizeof (set_idle_cmd),
                        capture_set_idle_cmd_cb);
      break;
    }
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *device, GError *error)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);

  fp_dbg ("Capture completed");

  if (priv->deactivating)
    {
      complete_deactivation (FP_IMAGE_DEVICE (device));
      if (error)
        g_error_free (error);
    }
  else if (error)
    {
      fpi_image_device_session_error (FP_IMAGE_DEVICE (device), error);
    }
  else
    {
      start_finger_detection (FP_IMAGE_DEVICE (device));
    }
}

static void
start_capture (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiSsm *ssm;

  if (priv->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state, CAPTURE_NUM_STATES);
  G_DEBUG_HERE ();
  fpi_ssm_start (ssm, capture_sm_complete);
}

/****** INITIALIZATION/DEINITIALIZATION ******/

enum activate_states {
  ACTIVATE_SET_IDLE,
  ACTIVATE_SEND_READ_ID_CMD,
  ACTIVATE_READ_ID,
  ACTIVATE_SEND_CALIBRATE_CMD,
  ACTIVATE_READ_CALIBRATE_DATA,
  ACTIVATE_SEND_INIT_CMD,
  ACTIVATE_READ_INIT_RESPONSE,
  ACTIVATE_NUM_STATES,
};

static void
activate_read_id_cb (FpiUsbTransfer *transfer, FpDevice *device,
                     gpointer user_data, GError *error)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  FpiDeviceAesX660Class *cls = FPI_DEVICE_AES_X660_GET_CLASS (self);
  unsigned char *data = transfer->buffer;

  if (error)
    {
      fp_dbg ("read_id cmd failed");
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }
  /* ID was read correctly */
  if (data[0] == 0x07)
    {
      fp_dbg ("Sensor device id: %.2x%2x, bcdDevice: %.2x.%.2x, init status: %.2x",
              data[4], data[3], data[5], data[6], data[7]);
    }
  else
    {
      fp_dbg ("Bogus read ID response: %.2x", data[AESX660_RESPONSE_TYPE_OFFSET]);
      fpi_ssm_mark_failed (transfer->ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Bogus read ID response %.2x",
                                                     data[AESX660_RESPONSE_TYPE_OFFSET]));
      return;
    }

  switch (priv->init_seq_idx)
    {
    case 0:
      priv->init_seq = cls->init_seqs[0];
      priv->init_seq_len = cls->init_seqs_len[0];
      priv->init_seq_idx = 1;
      priv->init_cmd_idx = 0;
      /* Do calibration only after 1st init sequence */
      fpi_ssm_jump_to_state (transfer->ssm, ACTIVATE_SEND_INIT_CMD);
      break;

    case 1:
      priv->init_seq = cls->init_seqs[1];
      priv->init_seq_len = cls->init_seqs_len[1];
      priv->init_seq_idx = 2;
      priv->init_cmd_idx = 0;
      fpi_ssm_next_state (transfer->ssm);
      break;

    default:
      fp_dbg ("Failed to init device! init status: %.2x", data[7]);
      fpi_ssm_mark_failed (transfer->ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Failed to init device %.2x",
                                                     data[7]));
      break;
    }
}

static void
activate_read_init_cb (FpiUsbTransfer *transfer, FpDevice *device,
                       gpointer user_data, GError *error)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (device);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  unsigned char *data = transfer->buffer;

  fp_dbg ("read_init_cb");

  if (error)
    {
      fp_dbg ("read_init transfer status: %s, actual_len: %d", error->message,
              (gint) transfer->actual_length);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }
  /* ID was read correctly */
  if (data[0] != 0x42 || data[3] != 0x01)
    {
      fp_dbg ("Bogus read init response: %.2x %.2x", data[0],
              data[3]);
      fpi_ssm_mark_failed (transfer->ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Bogus read init response: "
                                                     "%.2x %.2x", data[0], data[3]));
      return;
    }
  priv->init_cmd_idx++;
  if (priv->init_cmd_idx == priv->init_seq_len)
    {
      if (priv->init_seq_idx < 2)
        fpi_ssm_jump_to_state (transfer->ssm,
                               ACTIVATE_SEND_READ_ID_CMD);
      else
        fpi_ssm_mark_completed (transfer->ssm);
      return;
    }

  fpi_ssm_jump_to_state (transfer->ssm, ACTIVATE_SEND_INIT_CMD);
}

static void
activate_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (_dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case ACTIVATE_SET_IDLE:
      priv->init_seq_idx = 0;
      fp_dbg ("Activate: set idle");
      aesX660_send_cmd (ssm, _dev, set_idle_cmd, sizeof (set_idle_cmd),
                        fpi_ssm_usb_transfer_cb);
      break;

    case ACTIVATE_SEND_READ_ID_CMD:
      fp_dbg ("Activate: read ID");
      aesX660_send_cmd (ssm, _dev, read_id_cmd, sizeof (read_id_cmd),
                        fpi_ssm_usb_transfer_cb);
      break;

    case ACTIVATE_READ_ID:
      aesX660_read_response (ssm, _dev, TRUE, FALSE, ID_LEN, activate_read_id_cb);
      break;

    case ACTIVATE_SEND_INIT_CMD:
      fp_dbg ("Activate: send init seq #%d cmd #%d",
              priv->init_seq_idx,
              priv->init_cmd_idx);
      aesX660_send_cmd (ssm, _dev,
                        priv->init_seq[priv->init_cmd_idx].cmd,
                        priv->init_seq[priv->init_cmd_idx].len,
                        fpi_ssm_usb_transfer_cb);
      break;

    case ACTIVATE_READ_INIT_RESPONSE:
      fp_dbg ("Activate: read init response");
      aesX660_read_response (ssm, _dev, TRUE, FALSE, INIT_LEN, activate_read_init_cb);
      break;

    case ACTIVATE_SEND_CALIBRATE_CMD:
      aesX660_send_cmd (ssm, _dev, calibrate_cmd, sizeof (calibrate_cmd),
                        fpi_ssm_usb_transfer_cb);
      break;

    case ACTIVATE_READ_CALIBRATE_DATA:
      aesX660_read_response (ssm, _dev, TRUE, FALSE, CALIBRATE_DATA_LEN, aesX660_read_calibrate_data_cb);
      break;
    }
}

static void
activate_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  fpi_image_device_activate_complete (FP_IMAGE_DEVICE (_dev), error);

  if (!error)
    start_finger_detection (FP_IMAGE_DEVICE (_dev));
}

static void
aesX660_dev_activate (FpImageDevice *dev)
{
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
aesX660_dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);

  priv->deactivating = TRUE;
}

static void
aesX660_dev_init (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  GError *error = NULL;

  g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error);

  priv->stripe_packet = g_byte_array_new ();

  fpi_image_device_open_complete (dev, error);
}

static void
aesX660_dev_deinit (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);
  GError *error = NULL;

  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);

  g_clear_pointer (&priv->stripe_packet, g_byte_array_unref);

  fpi_image_device_close_complete (dev, error);
}


static void
complete_deactivation (FpImageDevice *dev)
{
  FpiDeviceAesX660 *self = FPI_DEVICE_AES_X660 (dev);
  FpiDeviceAesX660Private *priv = fpi_device_aes_x660_get_instance_private (self);

  G_DEBUG_HERE ();

  priv->deactivating = FALSE;
  g_slist_free (priv->strips);
  priv->strips = NULL;
  priv->strips_len = 0;
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
fpi_device_aes_x660_init (FpiDeviceAesX660 *self)
{
}

static void
fpi_device_aes_x660_class_init (FpiDeviceAesX660Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = aesX660_dev_init;
  img_class->img_close = aesX660_dev_deinit;
  img_class->activate = aesX660_dev_activate;
  img_class->deactivate = aesX660_dev_deactivate;

  /* Everything else is set by the subclasses. */
}
