/*
 * Validity Sensors, Inc. VFS5011 Fingerprint Reader driver for libfprint
 * Copyright (C) 2013 Arseniy Lartsev <arseniy@chalmers.se>
 *                    AceLan Kao <acelan.kao@canonical.com>
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

#define FP_COMPONENT "vfs5011"

#include "drivers_api.h"
#include "vfs5011_proto.h"

/* =================== sync/async USB transfer sequence ==================== */

enum {
  ACTION_SEND,
  ACTION_RECEIVE,
};

struct usb_action
{
  int            type;
  const char    *name;
  int            endpoint;
  int            size;
  unsigned char *data;
  int            correct_reply_size;
};

#define SEND(ENDPOINT, COMMAND) \
  { \
    .type = ACTION_SEND, \
    .endpoint = ENDPOINT, \
    .name = #COMMAND, \
    .size = sizeof (COMMAND), \
    .data = COMMAND \
  },

#define RECV(ENDPOINT, SIZE) \
  { \
    .type = ACTION_RECEIVE, \
    .endpoint = ENDPOINT, \
    .size = SIZE, \
    .data = NULL \
  },

#define RECV_CHECK(ENDPOINT, SIZE, EXPECTED) \
  { \
    .type = ACTION_RECEIVE, \
    .endpoint = ENDPOINT, \
    .size = SIZE, \
    .data = EXPECTED, \
    .correct_reply_size = sizeof (EXPECTED) \
  },

struct usbexchange_data
{
  int                stepcount;
  FpImageDevice     *device;
  struct usb_action *actions;
  void              *receive_buf;
  int                timeout;
};

static void start_scan (FpImageDevice *dev);

static void
async_send_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  struct usbexchange_data *data = fpi_ssm_get_data (transfer->ssm);
  struct usb_action *action;

  g_assert (!(fpi_ssm_get_cur_state (transfer->ssm) >= data->stepcount));

  action = &data->actions[fpi_ssm_get_cur_state (transfer->ssm)];
  g_assert (!(action->type != ACTION_SEND));

  if (error)
    {
      /* Transfer not completed, return IO error */
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  /* success */
  fpi_ssm_next_state (transfer->ssm);
}

static void
async_recv_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  struct usbexchange_data *data = fpi_ssm_get_data (transfer->ssm);
  struct usb_action *action;

  if (error)
    {
      /* Transfer not completed, return IO error */
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  g_assert (!(fpi_ssm_get_cur_state (transfer->ssm) >= data->stepcount));

  action = &data->actions[fpi_ssm_get_cur_state (transfer->ssm)];
  g_assert (!(action->type != ACTION_RECEIVE));

  if (action->data != NULL)
    {
      if (transfer->actual_length != action->correct_reply_size)
        {
          fp_err ("Got %d bytes instead of %d",
                  (gint) transfer->actual_length,
                  action->correct_reply_size);
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
          return;
        }
      if (memcmp (transfer->buffer, action->data,
                  action->correct_reply_size) != 0)
        {
          fp_dbg ("Wrong reply:");
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
          return;
        }
    }
  else
    {
      fp_dbg ("Got %d bytes out of %d",
              (gint) transfer->actual_length,
              (gint) transfer->length);
    }

  fpi_ssm_next_state (transfer->ssm);
}

static void
usbexchange_loop (FpiSsm *ssm, FpDevice *_dev)
{
  struct usbexchange_data *data = fpi_ssm_get_data (ssm);
  struct usb_action *action = &data->actions[fpi_ssm_get_cur_state (ssm)];
  FpiUsbTransfer *transfer;

  g_assert (fpi_ssm_get_cur_state (ssm) < data->stepcount);

  switch (action->type)
    {
    case ACTION_SEND:
      fp_dbg ("Sending %s", action->name);
      transfer = fpi_usb_transfer_new (_dev);
      fpi_usb_transfer_fill_bulk_full (transfer, action->endpoint,
                                       action->data, action->size,
                                       NULL);
      transfer->ssm = ssm;
      transfer->short_is_error = TRUE;
      fpi_usb_transfer_submit (transfer, data->timeout, NULL,
                               async_send_cb, NULL);
      break;

    case ACTION_RECEIVE:
      fp_dbg ("Receiving %d bytes", action->size);
      transfer = fpi_usb_transfer_new (_dev);
      fpi_usb_transfer_fill_bulk_full (transfer, action->endpoint,
                                       data->receive_buf,
                                       action->size, NULL);
      transfer->ssm = ssm;
      fpi_usb_transfer_submit (transfer, data->timeout, NULL,
                               async_recv_cb, NULL);
      break;

    default:
      fp_err ("Bug detected: invalid action %d", action->type);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
      return;
    }
}

static void
usb_exchange_async (FpiSsm                  *ssm,
                    struct usbexchange_data *data,
                    const char              *exchange_name)
{
  FpiSsm *subsm = fpi_ssm_new_full (FP_DEVICE (data->device),
                                    usbexchange_loop,
                                    data->stepcount,
                                    exchange_name);

  fpi_ssm_set_data (subsm, data, NULL);
  fpi_ssm_start_subsm (ssm, subsm);
}

/* ====================== utils ======================= */

/* Calculade squared standand deviation of sum of two lines */
static int
vfs5011_get_deviation2 (struct fpi_line_asmbl_ctx *ctx, GSList *row1, GSList *row2)
{
  unsigned char *buf1, *buf2;
  int res = 0, mean = 0, i;
  const int size = 64;

  buf1 = (unsigned char *) row1->data + 56;
  buf2 = (unsigned char *) row2->data + 168;

  for (i = 0; i < size; i++)
    mean += (int) buf1[i] + (int) buf2[i];

  mean /= size;

  for (i = 0; i < size; i++)
    {
      int dev = (int) buf1[i] + (int) buf2[i] - mean;
      res += dev * dev;
    }

  return res / size;
}

static unsigned char
vfs5011_get_pixel (struct fpi_line_asmbl_ctx *ctx,
                   GSList                    *row,
                   unsigned                   x)
{
  unsigned char *data = (unsigned char *) row->data + 8;

  return data[x];
}

/* ====================== main stuff ======================= */

enum {
  CAPTURE_LINES = 256,
  MAXLINES = 2000,
  MAX_CAPTURE_LINES = 100000,
};

static struct fpi_line_asmbl_ctx assembling_ctx = {
  .line_width = VFS5011_IMAGE_WIDTH,
  .max_height = MAXLINES,
  .resolution = 10,
  .median_filter_size = 25,
  .max_search_offset = 30,
  .get_deviation = vfs5011_get_deviation2,
  .get_pixel = vfs5011_get_pixel,
};

struct _FpDeviceVfs5011
{
  FpImageDevice           parent;

  unsigned char          *total_buffer;
  unsigned char          *capture_buffer;
  unsigned char          *row_buffer;
  unsigned char          *lastline;
  GSList                 *rows;
  int                     lines_captured, lines_recorded, empty_lines;
  int                     max_lines_captured, max_lines_recorded;
  int                     lines_total, lines_total_allocated;
  gboolean                loop_running;
  gboolean                deactivating;
  struct usbexchange_data init_sequence;
};

G_DECLARE_FINAL_TYPE (FpDeviceVfs5011, fpi_device_vfs5011, FPI, DEVICE_VFS5011,
                      FpImageDevice);
G_DEFINE_TYPE (FpDeviceVfs5011, fpi_device_vfs5011, FP_TYPE_IMAGE_DEVICE);

enum {
  DEV_ACTIVATE_REQUEST_FPRINT,
  DEV_ACTIVATE_INIT_COMPLETE,
  DEV_ACTIVATE_READ_DATA,
  DEV_ACTIVATE_DATA_COMPLETE,
  DEV_ACTIVATE_PREPARE_NEXT_CAPTURE,
  DEV_ACTIVATE_NUM_STATES
};

enum {
  DEV_OPEN_START,
  DEV_OPEN_NUM_STATES
};

static void
capture_init (FpDeviceVfs5011 *self, int max_captured,
              int max_recorded)
{
  fp_dbg ("capture_init");
  self->lastline = NULL;
  self->lines_captured = 0;
  self->lines_recorded = 0;
  self->empty_lines = 0;
  self->lines_total = 0;
  self->lines_total_allocated = 0;
  self->total_buffer = NULL;
  self->max_lines_captured = max_captured;
  self->max_lines_recorded = max_recorded;
}

static int
process_chunk (FpDeviceVfs5011 *self, int transferred)
{
  enum {
    DEVIATION_THRESHOLD = 15 * 15,
    DIFFERENCE_THRESHOLD = 600,
    STOP_CHECK_LINES = 50
  };

  fp_dbg ("process_chunk: got %d bytes", transferred);
  int lines_captured = transferred / VFS5011_LINE_SIZE;
  int i;

  for (i = 0; i < lines_captured; i++)
    {
      unsigned char *linebuf = self->capture_buffer
                               + i * VFS5011_LINE_SIZE;

      if (fpi_std_sq_dev (linebuf + 8, VFS5011_IMAGE_WIDTH)
          < DEVIATION_THRESHOLD)
        {
          if (self->lines_captured == 0)
            continue;
          else
            self->empty_lines++;
        }
      else
        {
          self->empty_lines = 0;
        }
      if (self->empty_lines >= STOP_CHECK_LINES)
        {
          fp_dbg ("process_chunk: got %d empty lines, finishing",
                  self->empty_lines);
          return 1;
        }

      self->lines_captured++;
      if (self->lines_captured > self->max_lines_captured)
        {
          fp_dbg ("process_chunk: captured %d lines, finishing",
                  self->lines_captured);
          return 1;
        }

      if ((self->lastline == NULL) ||
          (fpi_mean_sq_diff_norm (self->lastline + 8,
                                  linebuf + 8,
                                  VFS5011_IMAGE_WIDTH) >= DIFFERENCE_THRESHOLD))
        {
          self->lastline = g_malloc (VFS5011_LINE_SIZE);
          self->rows = g_slist_prepend (self->rows,
                                        self->lastline);
          memmove (self->lastline, linebuf, VFS5011_LINE_SIZE);
          self->lines_recorded++;
          if (self->lines_recorded >= self->max_lines_recorded)
            {
              fp_dbg ("process_chunk: recorded %d lines, finishing",
                      self->lines_recorded);
              return 1;
            }
        }
    }
  return 0;
}

static void
submit_image (FpiSsm          *ssm,
              FpDeviceVfs5011 *self,
              FpImageDevice   *dev)
{
  FpImage *img;

  if (self->lines_recorded < VFS5011_IMAGE_WIDTH)
    {
      fpi_image_device_retry_scan (dev, FP_DEVICE_RETRY_TOO_SHORT);
      return;
    }

  g_assert (self->rows != NULL);

  self->rows = g_slist_reverse (self->rows);

  img = fpi_assemble_lines (&assembling_ctx, self->rows,
                            self->lines_recorded);

  g_slist_free_full (self->rows, g_free);
  self->rows = NULL;

  fp_dbg ("Image captured, committing");

  fpi_image_device_image_captured (dev, img);
}

static void
chunk_capture_callback (FpiUsbTransfer *transfer, FpDevice *device,
                        gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (dev);

  if (!error ||
      g_error_matches (error, G_USB_DEVICE_ERROR, G_USB_DEVICE_ERROR_TIMED_OUT))
    {
      if (error)
        g_error_free (error);

      if (transfer->actual_length > 0)
        fpi_image_device_report_finger_status (dev, TRUE);

      if (process_chunk (self, transfer->actual_length))
        fpi_ssm_jump_to_state (transfer->ssm,
                               DEV_ACTIVATE_DATA_COMPLETE);
      else
        fpi_ssm_jump_to_state (transfer->ssm,
                               DEV_ACTIVATE_READ_DATA);
    }
  else
    {
      if (!self->deactivating)
        {
          fp_err ("Failed to capture data");
          fpi_ssm_mark_failed (transfer->ssm, error);
        }
      else
        {
          g_error_free (error);
          fpi_ssm_mark_completed (transfer->ssm);
        }
    }
}

static void
capture_chunk_async (FpDeviceVfs5011 *self,
                     GUsbDevice *handle, int nline,
                     int timeout, FpiSsm *ssm)
{
  FpiUsbTransfer *transfer;

  fp_dbg ("capture_chunk_async: capture %d lines, already have %d",
          nline, self->lines_recorded);
  enum {
    DEVIATION_THRESHOLD = 15 * 15,
    DIFFERENCE_THRESHOLD = 600,
    STOP_CHECK_LINES = 50
  };

  transfer = fpi_usb_transfer_new (FP_DEVICE (self));
  fpi_usb_transfer_fill_bulk_full (transfer,
                                   VFS5011_IN_ENDPOINT_DATA,
                                   self->capture_buffer,
                                   nline * VFS5011_LINE_SIZE, NULL);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, timeout, fpi_device_get_cancellable (FP_DEVICE (self)),
                           chunk_capture_callback, NULL);
}

/*
 *  Device initialization. Windows driver only does it when the device is
 *  plugged in, but it doesn't harm to do this every time before scanning the
 *  image.
 */
struct usb_action vfs5011_initialization[] = {
  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_01)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_19)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)      /* B5C457F9 */

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_00)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)      /* 0000FFFFFFFF */

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_01)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)      /* 0000FFFFFFFFFF */

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_02)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_01)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_03)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_04)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 256)
  RECV (VFS5011_IN_ENDPOINT_DATA, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_05)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_01)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_06)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 17216)
  RECV (VFS5011_IN_ENDPOINT_DATA, 32)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_07)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 45056)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_08)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 16896)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_09)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 4928)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_10)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 5632)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_11)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 5632)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_12)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 3328)
  RECV (VFS5011_IN_ENDPOINT_DATA, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_13)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_03)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_14)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  RECV (VFS5011_IN_ENDPOINT_DATA, 4800)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_02)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_27)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_15)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_16)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 2368)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)
  RECV (VFS5011_IN_ENDPOINT_DATA, 4800)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_17)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_init_18)
  /* 0000 */
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  /*
   * Windows driver does this and it works
   * But in this driver this call never returns...
   * RECV(VFS5011_IN_ENDPOINT_CTRL2, 8) //00D3054000
   */
};

/* Initiate recording the image */
struct usb_action vfs5011_initiate_capture[] = {
  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_04)
  RECV (VFS5011_IN_ENDPOINT_DATA, 64)
  RECV (VFS5011_IN_ENDPOINT_DATA, 84032)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_prepare_00)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_cmd_1A)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_prepare_01)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_prepare_02)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 2368)
  RECV (VFS5011_IN_ENDPOINT_CTRL, 64)
  RECV (VFS5011_IN_ENDPOINT_DATA, 4800)

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_prepare_03)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 64, VFS5011_NORMAL_CONTROL_REPLY)
  /*
   * Windows driver does this and it works
   * But in this driver this call never returns...
   * RECV(VFS5011_IN_ENDPOINT_CTRL2, 8);
   */

  SEND (VFS5011_OUT_ENDPOINT, vfs5011_prepare_04)
  RECV_CHECK (VFS5011_IN_ENDPOINT_CTRL, 2368, VFS5011_NORMAL_CONTROL_REPLY)

  /*
   * Windows driver does this and it works
   * But in this driver this call never returns...
   * RECV(VFS5011_IN_ENDPOINT_CTRL2, 8);
   */
};

/* ====================== lifprint interface ======================= */

static void
activate_loop (FpiSsm *ssm, FpDevice *_dev)
{
  enum {READ_TIMEOUT = 0};

  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (_dev);

  fp_dbg ("main_loop: state %d", fpi_ssm_get_cur_state (ssm));

  if (self->deactivating)
    {
      fp_dbg ("deactivating, marking completed");
      fpi_ssm_mark_completed (ssm);
      return;
    }

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case DEV_ACTIVATE_REQUEST_FPRINT:
      self->init_sequence.stepcount =
        G_N_ELEMENTS (vfs5011_initiate_capture);
      self->init_sequence.actions = vfs5011_initiate_capture;
      self->init_sequence.device = dev;
      if (self->init_sequence.receive_buf == NULL)
        self->init_sequence.receive_buf =
          g_malloc0 (VFS5011_RECEIVE_BUF_SIZE);
      self->init_sequence.timeout = 1000;
      usb_exchange_async (ssm, &self->init_sequence, "ACTIVATE REQUEST");
      break;

    case DEV_ACTIVATE_INIT_COMPLETE:
      if (self->init_sequence.receive_buf != NULL)
        g_free (self->init_sequence.receive_buf);
      self->init_sequence.receive_buf = NULL;
      capture_init (self, MAX_CAPTURE_LINES, MAXLINES);
      fpi_image_device_activate_complete (dev, NULL);
      fpi_ssm_next_state (ssm);
      break;

    case DEV_ACTIVATE_READ_DATA:
      capture_chunk_async (self,
                           fpi_device_get_usb_device (FP_DEVICE (dev)),
                           CAPTURE_LINES,
                           READ_TIMEOUT, ssm);
      break;

    case DEV_ACTIVATE_DATA_COMPLETE:
      fpi_ssm_next_state_delayed (ssm, 1, NULL);
      break;

    case DEV_ACTIVATE_PREPARE_NEXT_CAPTURE:
      self->init_sequence.stepcount =
        G_N_ELEMENTS (vfs5011_initiate_capture);
      self->init_sequence.actions = vfs5011_initiate_capture;
      self->init_sequence.device = dev;
      if (self->init_sequence.receive_buf == NULL)
        self->init_sequence.receive_buf =
          g_malloc0 (VFS5011_RECEIVE_BUF_SIZE);
      self->init_sequence.timeout = VFS5011_DEFAULT_WAIT_TIMEOUT;
      usb_exchange_async (ssm, &self->init_sequence, "PREPARE CAPTURE");
      break;

    }
}

static void
activate_loop_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (_dev);

  fp_dbg ("finishing");
  if (self->init_sequence.receive_buf != NULL)
    g_free (self->init_sequence.receive_buf);
  self->init_sequence.receive_buf = NULL;
  if (!self->deactivating && !error)
    {
      submit_image (ssm, self, dev);
      fpi_image_device_report_finger_status (dev, FALSE);
    }

  self->loop_running = FALSE;

  if (self->deactivating)
    fpi_image_device_deactivate_complete (dev, error);
  else if (error)
    fpi_image_device_session_error (dev, error);
  else
    start_scan (dev);
}


static void
open_loop (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case DEV_OPEN_START:
      self->init_sequence.stepcount =
        G_N_ELEMENTS (vfs5011_initialization);
      self->init_sequence.actions = vfs5011_initialization;
      self->init_sequence.device = dev;
      self->init_sequence.receive_buf =
        g_malloc0 (VFS5011_RECEIVE_BUF_SIZE);
      self->init_sequence.timeout = VFS5011_DEFAULT_WAIT_TIMEOUT;
      usb_exchange_async (ssm, &self->init_sequence, "DEVICE OPEN");
      break;
    }
  ;
}

static void
open_loop_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (_dev);
  g_free (self->init_sequence.receive_buf);
  self->init_sequence.receive_buf = NULL;

  fpi_image_device_open_complete (dev, error);
}

static void
dev_open (FpImageDevice *dev)
{
  FpiSsm *ssm;
  GError *error = NULL;
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (dev);
  self->capture_buffer = g_new0 (unsigned char, CAPTURE_LINES * VFS5011_LINE_SIZE);

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  ssm = fpi_ssm_new (FP_DEVICE (dev), open_loop, DEV_OPEN_NUM_STATES);
  fpi_ssm_start (ssm, open_loop_complete);
}

static void
dev_close (FpImageDevice *dev)
{
  GError *error = NULL;
  FpDeviceVfs5011 *self = FPI_DEVICE_VFS5011 (dev);

  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);

  g_free (self->capture_buffer);
  g_slist_free_full (g_steal_pointer (&self->rows), g_free);

  fpi_image_device_close_complete (dev, error);
}

static void
start_scan (FpImageDevice *dev)
{
  FpDeviceVfs5011 *self;
  FpiSsm *ssm;

  self = FPI_DEVICE_VFS5011 (dev);
  self->loop_running = TRUE;
  fp_dbg ("creating ssm");
  ssm = fpi_ssm_new (FP_DEVICE (dev), activate_loop, DEV_ACTIVATE_NUM_STATES);
  fp_dbg ("starting ssm");
  fpi_ssm_start (ssm, activate_loop_complete);
  fp_dbg ("ssm done, getting out");
}

static void
dev_activate (FpImageDevice *dev)
{
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (dev);
  fp_dbg ("device initialized");
  self->deactivating = FALSE;

  start_scan (dev);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpDeviceVfs5011 *self;

  self = FPI_DEVICE_VFS5011 (dev);
  if (self->loop_running)
    self->deactivating = TRUE;
  else
    fpi_image_device_deactivate_complete (dev, NULL);
}

static const FpIdEntry id_table[] = {
  { /* Validity device from some Toshiba laptops */ .vid = 0x138a,  .pid = 0x0010, },
  { /* vfs5011 */ .vid = 0x138a,  .pid = 0x0011, },
  { /* Validity device from Lenovo Preferred Pro USB Fingerprint Keyboard KUF1256 */ .vid = 0x138a,  .pid = 0x0015, },
  { /* Validity device from Lenovo T440 laptops */ .vid = 0x138a,  .pid = 0x0017, },
  { /* one more Validity device */ .vid = 0x138a,  .pid = 0x0018, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_vfs5011_init (FpDeviceVfs5011 *self)
{
}
static void
fpi_device_vfs5011_class_init (FpDeviceVfs5011Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "vfs5011";
  dev_class->full_name = "Validity VFS5011";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_open;
  img_class->img_close = dev_close;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 20;

  img_class->img_width = VFS5011_IMAGE_WIDTH;
  img_class->img_height = -1;
}
