/*
 * UPEK TouchChip driver for libfprint
 * Copyright (C) 2013 Vasily Khoruzhick <anarsoul@gmail.com>
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

#define FP_COMPONENT "upektc_img"

#include "drivers_api.h"
#include "upek_proto.h"
#include "upektc_img.h"

static void start_capture (FpImageDevice *dev);
static void start_deactivation (FpImageDevice *dev);

#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)
#define CTRL_TIMEOUT 4000
#define BULK_TIMEOUT 4000

#define IMAGE_WIDTH 144
#define IMAGE_HEIGHT 384
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT)

#define MAX_CMD_SIZE 64
#define MAX_RESPONSE_SIZE 2052
#define SHORT_RESPONSE_SIZE 64

struct _FpiDeviceUpektcImg
{
  FpImageDevice  parent;

  unsigned char  cmd[MAX_CMD_SIZE];
  unsigned char  response[MAX_RESPONSE_SIZE];
  unsigned char *image_bits;
  unsigned char  seq;
  size_t         image_size;
  size_t         response_rest;
  gboolean       deactivating;
};
G_DECLARE_FINAL_TYPE (FpiDeviceUpektcImg, fpi_device_upektc_img, FPI,
                      DEVICE_UPEKTC_IMG, FpImageDevice);
G_DEFINE_TYPE (FpiDeviceUpektcImg, fpi_device_upektc_img, FP_TYPE_IMAGE_DEVICE);

/****** HELPERS ******/

static void
upektc_img_cmd_fix_seq (unsigned char *cmd_buf, unsigned char seq)
{
  uint8_t byte;

  byte = cmd_buf[5];
  byte &= 0x0f;
  byte |= (seq << 4);
  cmd_buf[5] = byte;
}

static void
upektc_img_cmd_update_crc (unsigned char *cmd_buf, size_t size)
{
  /* CRC does not cover Ciao prefix (4 bytes) and CRC location (2 bytes) */
  uint16_t crc = udf_crc (cmd_buf + 4, size - 6);

  cmd_buf[size - 2] = (crc & 0x00ff);
  cmd_buf[size - 1] = (crc & 0xff00) >> 8;
}

FP_GNUC_ACCESS (read_only, 3, 4)
static void
upektc_img_submit_req (FpiSsm                *ssm,
                       FpImageDevice         *dev,
                       const unsigned char   *buf,
                       size_t                 buf_size,
                       unsigned char          seq,
                       FpiUsbTransferCallback cb)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  BUG_ON (buf_size > MAX_CMD_SIZE);

  memcpy (self->cmd, buf, buf_size);
  upektc_img_cmd_fix_seq (self->cmd, seq);
  upektc_img_cmd_update_crc (self->cmd, buf_size);

  fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT, self->cmd, buf_size,
                                   NULL);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL, cb, NULL);
}

static void
upektc_img_read_data (FpiSsm                *ssm,
                      FpImageDevice         *dev,
                      size_t                 buf_size,
                      size_t                 buf_offset,
                      FpiUsbTransferCallback cb)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);

  BUG_ON (buf_offset + buf_size > MAX_RESPONSE_SIZE);

  fpi_usb_transfer_fill_bulk_full (transfer, EP_IN,
                                   self->response + buf_offset, buf_size,
                                   NULL);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL, cb, NULL);
}

/****** CAPTURE ******/

enum capture_states {
  CAPTURE_INIT_CAPTURE,
  CAPTURE_READ_DATA,
  CAPTURE_READ_DATA_TERM,
  CAPTURE_ACK_00_28,
  CAPTURE_ACK_08,
  CAPTURE_ACK_FRAME,
  CAPTURE_ACK_00_28_TERM,
  CAPTURE_NUM_STATES,
};

static void
capture_reqs_cb (FpiUsbTransfer *transfer, FpDevice *device,
                 gpointer user_data, GError *error)
{
  if (error)
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }
  switch (fpi_ssm_get_cur_state (transfer->ssm))
    {
    case CAPTURE_ACK_00_28_TERM:
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA_TERM);
      break;

    default:
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA);
      break;
    }
}

static int
upektc_img_process_image_frame (unsigned char *image_buf, unsigned char *cmd_res)
{
  int offset = 8;
  int len = ((cmd_res[5] & 0x0f) << 8) | (cmd_res[6]);

  len -= 1;
  if (cmd_res[7] == 0x2c)
    {
      len -= 10;
      offset += 10;
    }
  if (cmd_res[7] == 0x20)
    len -= 4;
  memcpy (image_buf, cmd_res + offset, len);

  return len;
}

static void
capture_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                      gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  unsigned char *data = self->response;
  FpImage *img;
  size_t response_size;

  if (error)
    {
      fp_dbg ("request is not completed, %s", error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  if (self->deactivating)
    {
      fp_dbg ("Deactivate requested");
      fpi_ssm_mark_completed (transfer->ssm);
      return;
    }

  fp_dbg ("request completed, len: %.4x", (gint) transfer->actual_length);
  if (transfer->actual_length == 0)
    {
      fpi_ssm_jump_to_state (transfer->ssm,
                             fpi_ssm_get_cur_state (transfer->ssm));
      return;
    }

  if (fpi_ssm_get_cur_state (transfer->ssm) == CAPTURE_READ_DATA_TERM)
    {
      fp_dbg ("Terminating SSM");
      fpi_ssm_mark_completed (transfer->ssm);
      return;
    }

  if (!self->response_rest)
    {
      response_size = ((data[5] & 0x0f) << 8) + data[6];
      response_size += 9;           /* 7 bytes for header, 2 for CRC */
      if (response_size > transfer->actual_length)
        {
          fp_dbg ("response_size is %lu, actual_length is %d",
                  response_size, (gint) transfer->actual_length);
          fp_dbg ("Waiting for rest of transfer");
          BUG_ON (self->response_rest);
          self->response_rest = response_size - transfer->actual_length;
          fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA);
          return;
        }
    }
  self->response_rest = 0;

  switch (data[4])
    {
    case 0x00:
      switch (data[7])
        {
        /* No finger */
        case 0x28:
          fp_dbg ("18th byte is %.2x", data[18]);
          switch (data[18])
            {
            case 0x0c:
              /* no finger */
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_ACK_00_28);
              break;

            case 0x00:
              /* finger is present! */
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_ACK_00_28);
              break;

            case 0x1e:
              /* short scan */
              fp_err ("short scan, aborting");
              fpi_image_device_retry_scan (dev,
                                           FP_DEVICE_RETRY_TOO_SHORT);
              fpi_image_device_report_finger_status (dev,
                                                     FALSE);
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_ACK_00_28_TERM);
              break;

            case 0x1d:
              /* too much horisontal movement */
              fp_err ("too much horisontal movement, aborting");
              fpi_image_device_retry_scan (dev,
                                           FP_DEVICE_RETRY_CENTER_FINGER);
              fpi_image_device_report_finger_status (dev,
                                                     FALSE);
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_ACK_00_28_TERM);
              break;

            default:
              /* some error happened, cancel scan */
              fp_err ("something bad happened, stop scan");
              fpi_image_device_retry_scan (dev,
                                           FP_DEVICE_RETRY);
              fpi_image_device_report_finger_status (dev,
                                                     FALSE);
              fpi_ssm_jump_to_state (transfer->ssm,
                                     CAPTURE_ACK_00_28_TERM);
              break;
            }
          break;

        /* Image frame with additional info */
        case 0x2c:
          fpi_image_device_report_finger_status (dev,
                                                 TRUE);

        /* Plain image frame */
        case 0x24:
          self->image_size +=
            upektc_img_process_image_frame (self->image_bits + self->image_size,
                                            data);
          fpi_ssm_jump_to_state (transfer->ssm,
                                 CAPTURE_ACK_FRAME);
          break;

        /* Last image frame */
        case 0x20:
          self->image_size +=
            upektc_img_process_image_frame (self->image_bits + self->image_size,
                                            data);
          BUG_ON (self->image_size != IMAGE_SIZE);
          fp_dbg ("Image size is %lu",
                  self->image_size);
          img = fp_image_new (IMAGE_WIDTH, IMAGE_HEIGHT);
          img->flags |= FPI_IMAGE_PARTIAL;
          memcpy (img->data, self->image_bits,
                  IMAGE_SIZE);
          fpi_image_device_image_captured (dev, img);
          fpi_image_device_report_finger_status (dev,
                                                 FALSE);
          fpi_ssm_mark_completed (transfer->ssm);
          break;

        default:
          fp_err ("Unknown response!");
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
          break;
        }
      break;

    case 0x08:
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_ACK_08);
      break;

    default:
      fp_err ("Not handled response!");
      fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
    }
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_INIT_CAPTURE:
      upektc_img_submit_req (ssm, dev, upek2020_init_capture, sizeof (upek2020_init_capture),
                             self->seq, capture_reqs_cb);
      self->seq++;
      break;

    case CAPTURE_READ_DATA:
    case CAPTURE_READ_DATA_TERM:
      if (!self->response_rest)
        upektc_img_read_data (ssm, dev, SHORT_RESPONSE_SIZE, 0, capture_read_data_cb);
      else
        upektc_img_read_data (ssm, dev, MAX_RESPONSE_SIZE - SHORT_RESPONSE_SIZE,
                              SHORT_RESPONSE_SIZE, capture_read_data_cb);
      break;

    case CAPTURE_ACK_00_28:
    case CAPTURE_ACK_00_28_TERM:
      upektc_img_submit_req (ssm, dev, upek2020_ack_00_28, sizeof (upek2020_ack_00_28),
                             self->seq, capture_reqs_cb);
      self->seq++;
      break;

    case CAPTURE_ACK_08:
      upektc_img_submit_req (ssm, dev, upek2020_ack_08, sizeof (upek2020_ack_08),
                             0, capture_reqs_cb);
      break;

    case CAPTURE_ACK_FRAME:
      upektc_img_submit_req (ssm, dev, upek2020_ack_frame, sizeof (upek2020_ack_frame),
                             self->seq, capture_reqs_cb);
      self->seq++;
      break;
    }
  ;
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error_arg)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (_dev);

  g_autoptr(GError) error = error_arg;


  /* Note: We assume that the error is a cancellation in the deactivation case */
  if (self->deactivating)
    start_deactivation (dev);
  else if (error)
    fpi_image_device_session_error (dev, g_steal_pointer (&error));
  else
    start_capture (dev);
}

static void
start_capture (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  FpiSsm *ssm;

  self->image_size = 0;

  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state,
                     CAPTURE_NUM_STATES);
  fpi_ssm_start (ssm, capture_sm_complete);
}

/****** INITIALIZATION/DEINITIALIZATION ******/

enum deactivate_states {
  DEACTIVATE_DEINIT,
  DEACTIVATE_READ_DEINIT_DATA,
  DEACTIVATE_NUM_STATES,
};

static void
deactivate_reqs_cb (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  if (!error)
    fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_READ_DATA);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

/* TODO: process response properly */
static void
deactivate_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                         gpointer user_data, GError *error)
{
  if (!error)
    fpi_ssm_mark_completed (transfer->ssm);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

static void
deactivate_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case DEACTIVATE_DEINIT:
      upektc_img_submit_req (ssm, dev, upek2020_deinit, sizeof (upek2020_deinit),
                             self->seq, deactivate_reqs_cb);
      self->seq++;
      break;

    case DEACTIVATE_READ_DEINIT_DATA:
      upektc_img_read_data (ssm, dev, SHORT_RESPONSE_SIZE, 0, deactivate_read_data_cb);
      break;
    }
  ;
}

static void
deactivate_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (_dev);

  fp_dbg ("Deactivate completed");

  self->deactivating = FALSE;
  fpi_image_device_deactivate_complete (dev,  error);
}

static void
start_deactivation (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  FpiSsm *ssm;

  self->image_size = 0;

  ssm = fpi_ssm_new (FP_DEVICE (dev), deactivate_run_state,
                     DEACTIVATE_NUM_STATES);
  fpi_ssm_start (ssm, deactivate_sm_complete);
}

enum activate_states {
  ACTIVATE_CONTROL_REQ_1,
  ACTIVATE_READ_CTRL_RESP_1,
  ACTIVATE_INIT_1,
  ACTIVATE_READ_INIT_1_RESP,
  ACTIVATE_INIT_2,
  ACTIVATE_READ_INIT_2_RESP,
  ACTIVATE_CONTROL_REQ_2,
  ACTIVATE_READ_CTRL_RESP_2,
  ACTIVATE_INIT_3,
  ACTIVATE_READ_INIT_3_RESP,
  ACTIVATE_INIT_4,
  ACTIVATE_READ_INIT_4_RESP,
  ACTIVATE_NUM_STATES,
};

static void
init_reqs_cb (FpiUsbTransfer *transfer, FpDevice *device,
              gpointer user_data, GError *error)
{
  if (!error)
    fpi_ssm_next_state (transfer->ssm);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

/* TODO: process response properly */
static void
init_read_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                   gpointer user_data, GError *error)
{
  if (!error)
    fpi_ssm_next_state (transfer->ssm);
  else
    fpi_ssm_mark_failed (transfer->ssm, error);
}

static void
activate_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpiUsbTransfer *transfer;
  FpImageDevice *idev = FP_IMAGE_DEVICE (dev);
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case ACTIVATE_CONTROL_REQ_1:
    case ACTIVATE_CONTROL_REQ_2:
      {
        transfer = fpi_usb_transfer_new (dev);

        fpi_usb_transfer_fill_control (transfer,
                                       G_USB_DEVICE_DIRECTION_HOST_TO_DEVICE,
                                       G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                       G_USB_DEVICE_RECIPIENT_DEVICE,
                                       0x0c, 0x100, 0x0400, 1);
        transfer->buffer[0] = '\0';
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL,
                                 fpi_ssm_usb_transfer_cb, NULL);
      }
      break;

    case ACTIVATE_INIT_1:
      upektc_img_submit_req (ssm, idev, upek2020_init_1, sizeof (upek2020_init_1),
                             0, init_reqs_cb);
      break;

    case ACTIVATE_INIT_2:
      upektc_img_submit_req (ssm, idev, upek2020_init_2, sizeof (upek2020_init_2),
                             0, init_reqs_cb);
      break;

    case ACTIVATE_INIT_3:
      upektc_img_submit_req (ssm, idev, upek2020_init_3, sizeof (upek2020_init_3),
                             0, init_reqs_cb);
      break;

    case ACTIVATE_INIT_4:
      upektc_img_submit_req (ssm, idev, upek2020_init_4, sizeof (upek2020_init_4),
                             self->seq, init_reqs_cb);
      /* Seq should be updated after 4th init */
      self->seq++;
      break;

    case ACTIVATE_READ_CTRL_RESP_1:
    case ACTIVATE_READ_CTRL_RESP_2:
    case ACTIVATE_READ_INIT_1_RESP:
    case ACTIVATE_READ_INIT_2_RESP:
    case ACTIVATE_READ_INIT_3_RESP:
    case ACTIVATE_READ_INIT_4_RESP:
      upektc_img_read_data (ssm, idev, SHORT_RESPONSE_SIZE, 0, init_read_data_cb);
      break;
    }
}

static void
activate_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);

  fpi_image_device_activate_complete (dev, error);

  if (!error)
    start_capture (dev);
}

static void
dev_activate (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  self->seq = 0;
  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);

  self->deactivating = TRUE;
}

static void
dev_init (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  GError *error = NULL;

  /* TODO check that device has endpoints we're using */

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  self->image_bits = g_malloc0 (IMAGE_SIZE * 2);
  fpi_image_device_open_complete (dev, NULL);
}

static void
dev_deinit (FpImageDevice *dev)
{
  FpiDeviceUpektcImg *self = FPI_DEVICE_UPEKTC_IMG (dev);
  GError *error = NULL;

  g_clear_pointer (&self->image_bits, g_free);
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);
  fpi_image_device_close_complete (dev, error);
}

static int
discover (GUsbDevice *usb_device)
{
  gint16 pid = g_usb_device_get_pid (usb_device);
  gint16 bcd = g_usb_device_get_release (usb_device);

  if (pid == 0x2020 && bcd == 1)
    return 100;
  if (pid == 0x2016 && bcd == 2)
    return 100;

  return 0;
}

static const FpIdEntry id_table[] = {
  { .vid = 0x147e,  .pid = 0x2016, },
  { .vid = 0x147e,  .pid = 0x2020, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_upektc_img_init (FpiDeviceUpektcImg *self)
{
}
static void
fpi_device_upektc_img_class_init (FpiDeviceUpektcImgClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "upektc_img";
  dev_class->full_name = "Upek TouchChip Fingerprint Coprocessor";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;
  dev_class->usb_discover = discover;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 20;

  img_class->img_width = IMAGE_WIDTH;
  img_class->img_height = IMAGE_HEIGHT;
}
