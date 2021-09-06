/*
 * Elan driver for libfprint
 *
 * Copyright (C) 2017 Igor Filatov <ia.filatov@gmail.com>
 * Copyright (C) 2018 Sébastien Béchet <sebastien.bechet@osinix.com >
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
 * The algorithm which libfprint uses to match fingerprints doesn't like small
 * images like the ones these drivers produce. There's just not enough minutiae
 * (recognizable print-specific points) on them for a reliable match. This means
 * that unless another matching algo is found/implemented, these readers will
 * not work as good with libfprint as they do with vendor drivers.
 *
 * To get bigger images the driver expects you to swipe the finger over the
 * reader. This works quite well for readers with a rectangular 144x64 sensor.
 * Worse than real swipe readers but good enough for day-to-day use. It needs
 * a steady and relatively slow swipe. There are also square 96x96 sensors and
 * I don't know whether they are in fact usable or not because I don't have one.
 * I imagine they'd be less reliable because the resulting image is even
 * smaller. If they can't be made usable with libfprint, I might end up dropping
 * them because it's better than saying they work when they don't.
 */

#define FP_COMPONENT "elan"

#include "drivers_api.h"
#include "elan.h"

static unsigned char
elan_get_pixel (struct fpi_frame_asmbl_ctx *ctx,
                struct fpi_frame *frame, unsigned int x,
                unsigned int y)
{
  return frame->data[x + y * ctx->frame_width];
}

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = 0,
  .frame_height = 0,
  .image_width = 0,
  .get_pixel = elan_get_pixel,
};

struct _FpiDeviceElan
{
  FpImageDevice parent;

  /* device config */
  unsigned short dev_type;
  unsigned short fw_ver;
  void           (*process_frame) (unsigned short *raw_frame,
                                   GSList       ** frames);
  /* end device config */

  /* commands */
  const struct elan_cmd *cmd;
  int                    cmd_timeout;
  /* end commands */

  /* state */
  gboolean        active;
  gboolean        deactivating;
  unsigned char  *last_read;
  unsigned char   calib_atts_left;
  unsigned char   calib_status;
  unsigned short *background;
  unsigned char   frame_width;
  unsigned char   frame_height;
  unsigned char   raw_frame_height;
  int             num_frames;
  GSList         *frames;
  /* end state */
};
G_DEFINE_TYPE (FpiDeviceElan, fpi_device_elan, FP_TYPE_IMAGE_DEVICE);

static int
cmp_short (const void *a, const void *b)
{
  return (int) (*(short *) a - *(short *) b);
}

static void
elan_dev_reset_state (FpiDeviceElan *elandev)
{
  G_DEBUG_HERE ();

  elandev->cmd = NULL;
  elandev->cmd_timeout = ELAN_CMD_TIMEOUT;

  elandev->calib_status = 0;

  g_free (elandev->last_read);
  elandev->last_read = NULL;

  g_slist_free_full (elandev->frames, g_free);
  elandev->frames = NULL;
  elandev->num_frames = 0;
}

static void
elan_save_frame (FpiDeviceElan *self, unsigned short *frame)
{
  G_DEBUG_HERE ();

  /* so far 3 types of readers by sensor dimensions and orientation have been
   * seen in the wild:
   * 1. 144x64. Raw images are in portrait orientation while readers themselves
   *    are placed (e.g. built into a touchpad) in landscape orientation. These
   *    need to be rotated before assembling.
   * 2. 96x96 rotated. Like the first type but square. Likewise, need to be
   *    rotated before assembling.
   * 3. 96x96 normal. Square and need NOT be rotated. So far there's only been
   *    1 report of a 0c03 of this type. Hopefully this type can be identified
   *    by device id (and manufacturers don't just install the readers as they
   *    please).
   * we also discard stripes of 'frame_margin' from bottom and top because
   * assembling works bad for tall frames */

  unsigned char frame_width = self->frame_width;
  unsigned char frame_height = self->frame_height;
  unsigned char raw_height = self->raw_frame_height;
  unsigned char frame_margin = (raw_height - self->frame_height) / 2;
  int frame_idx, raw_idx;

  for (int y = 0; y < frame_height; y++)
    for (int x = 0; x < frame_width; x++)
      {
        if (self->dev_type & ELAN_NOT_ROTATED)
          raw_idx = x + (y + frame_margin) * frame_width;
        else
          raw_idx = frame_margin + y + x * raw_height;
        frame_idx = x + y * frame_width;
        frame[frame_idx] =
          ((unsigned short *) self->last_read)[raw_idx];
      }
}

static void
elan_save_background (FpiDeviceElan *elandev)
{
  G_DEBUG_HERE ();

  g_free (elandev->background);
  elandev->background =
    g_malloc (elandev->frame_width * elandev->frame_height *
              sizeof (short));
  elan_save_frame (elandev, elandev->background);
}

/* save a frame as part of the fingerprint image
 * background needs to have been captured for this routine to work
 * Elantech recommends 2-step non-linear normalization in order to reduce
 * 2^14 ADC resolution to 2^8 image:
 *
 * 1. background is subtracted (done here)
 *
 * 2. pixels are grouped in 3 groups by intensity and each group is mapped
 *    separately onto the normalized frame (done in elan_process_frame_*)
 *    ==== 16383     ____> ======== 255
 *                  /
 *    ----- lvl3 __/
 *                   35% pixels
 *
 *    ----- lvl2 --------> ======== 156
 *
 *                   30% pixels
 *    ----- lvl1 --------> ======== 99
 *
 *                   35% pixels
 *    ----- lvl0 __
 *                 \
 *    ======== 0    \____> ======== 0
 *
 * For some devices we don't do 2. but instead do a simple linear mapping
 * because it seems to produce better results (or at least as good):
 *    ==== 16383      ___> ======== 255
 *                   /
 *    ------ max  __/
 *
 *
 *    ------ min  __
 *                  \
 *    ======== 0     \___> ======== 0
 */
static int
elan_save_img_frame (FpiDeviceElan *elandev)
{
  G_DEBUG_HERE ();

  unsigned int frame_size = elandev->frame_width * elandev->frame_height;
  unsigned short *frame = g_malloc (frame_size * sizeof (short));

  elan_save_frame (elandev, frame);
  unsigned int sum = 0;

  for (int i = 0; i < frame_size; i++)
    {
      if (elandev->background[i] > frame[i])
        frame[i] = 0;
      else
        frame[i] -= elandev->background[i];
      sum += frame[i];
    }

  if (sum == 0)
    {
      fp_dbg
        ("frame darker than background; finger present during calibration?");
      g_free (frame);
      return -1;
    }

  elandev->frames = g_slist_prepend (elandev->frames, frame);
  elandev->num_frames += 1;
  return 0;
}

static void
elan_process_frame_linear (unsigned short *raw_frame,
                           GSList       ** frames)
{
  unsigned int frame_size =
    assembling_ctx.frame_width * assembling_ctx.frame_height;
  struct fpi_frame *frame =
    g_malloc (frame_size + sizeof (struct fpi_frame));

  G_DEBUG_HERE ();

  unsigned short min = 0xffff, max = 0;

  for (int i = 0; i < frame_size; i++)
    {
      if (raw_frame[i] < min)
        min = raw_frame[i];
      if (raw_frame[i] > max)
        max = raw_frame[i];
    }

  g_assert (max != min);

  unsigned short px;

  for (int i = 0; i < frame_size; i++)
    {
      px = raw_frame[i];
      px = (px - min) * 0xff / (max - min);
      frame->data[i] = (unsigned char) px;
    }

  *frames = g_slist_prepend (*frames, frame);
}

static void
elan_process_frame_thirds (unsigned short *raw_frame,
                           GSList       ** frames)
{
  G_DEBUG_HERE ();

  unsigned int frame_size =
    assembling_ctx.frame_width * assembling_ctx.frame_height;
  struct fpi_frame *frame =
    g_malloc (frame_size + sizeof (struct fpi_frame));

  unsigned short lvl0, lvl1, lvl2, lvl3;
  unsigned short *sorted = g_malloc (frame_size * sizeof (short));

  memcpy (sorted, raw_frame, frame_size * sizeof (short));
  qsort (sorted, frame_size, sizeof (short), cmp_short);
  lvl0 = sorted[0];
  lvl1 = sorted[frame_size * 3 / 10];
  lvl2 = sorted[frame_size * 65 / 100];
  lvl3 = sorted[frame_size - 1];
  g_free (sorted);

  unsigned short px;

  for (int i = 0; i < frame_size; i++)
    {
      px = raw_frame[i];
      if (lvl0 <= px && px < lvl1)
        px = (px - lvl0) * 99 / (lvl1 - lvl0);
      else if (lvl1 <= px && px < lvl2)
        px = 99 + ((px - lvl1) * 56 / (lvl2 - lvl1));
      else                      // (lvl2 <= px && px <= lvl3)
        px = 155 + ((px - lvl2) * 100 / (lvl3 - lvl2));
      frame->data[i] = (unsigned char) px;
    }

  *frames = g_slist_prepend (*frames, frame);
}

static void
elan_submit_image (FpImageDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);
  GSList *raw_frames;
  GSList *frames = NULL;
  FpImage *img;

  G_DEBUG_HERE ();

  raw_frames = g_slist_nth (self->frames, ELAN_SKIP_LAST_FRAMES);

  assembling_ctx.frame_width = self->frame_width;
  assembling_ctx.frame_height = self->frame_height;
  assembling_ctx.image_width = self->frame_width * 3 / 2;
  g_slist_foreach (raw_frames, (GFunc) self->process_frame, &frames);
  fpi_do_movement_estimation (&assembling_ctx, frames);
  img = fpi_assemble_frames (&assembling_ctx, frames);
  img->flags |= FPI_IMAGE_PARTIAL;

  g_slist_free_full (frames, g_free);

  fpi_image_device_image_captured (dev, img);
}

static void
elan_cmd_done (FpiSsm *ssm)
{
  G_DEBUG_HERE ();
  fpi_ssm_next_state (ssm);
}

static void
elan_cmd_cb (FpiUsbTransfer *transfer, FpDevice *dev,
             gpointer user_data, GError *error)
{
  FpiSsm *ssm = transfer->ssm;
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  if (error)
    {
      /* XXX: In the cancellation case we used to not
       *      mark the SSM as failed?! */
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  /* XXX: We used to reset the device in error cases! */
  if (transfer->endpoint & FPI_USB_ENDPOINT_IN)
    {
      /* just finished receiving */
      self->last_read = g_memdup (transfer->buffer, transfer->actual_length);
      elan_cmd_done (ssm);
    }
  else
    {
      /* just finished sending */
      G_DEBUG_HERE ();
      elan_cmd_read (ssm, dev);
    }
}

static void
elan_cmd_read (FpiSsm *ssm, FpDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);
  FpiUsbTransfer *transfer;
  GCancellable *cancellable = NULL;
  int response_len = self->cmd->response_len;

  G_DEBUG_HERE ();

  if (self->cmd->response_len == ELAN_CMD_SKIP_READ)
    {
      fp_dbg ("skipping read, not expecting anything");
      elan_cmd_done (ssm);
      return;
    }

  if (self->dev_type == ELAN_0C42)
    {
      /* ELAN_0C42 sends an extra byte in one byte responses */
      if (self->cmd->response_len == 1)
        response_len = 2;
    }

  if (self->cmd->cmd == get_image_cmd.cmd)
    /* raw data has 2-byte "pixels" and the frame is vertical */
    response_len =
      self->raw_frame_height * self->frame_width * 2;

  g_clear_pointer (&self->last_read, g_free);

  transfer = fpi_usb_transfer_new (dev);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;

  fpi_usb_transfer_fill_bulk (transfer,
                              self->cmd->response_in,
                              response_len);

  if (!self->cmd->never_cancel)
    cancellable = fpi_device_get_cancellable (dev);

  fpi_usb_transfer_submit (transfer, self->cmd_timeout, cancellable, elan_cmd_cb, NULL);
}

static void
elan_run_cmd (FpiSsm                *ssm,
              FpDevice              *dev,
              const struct elan_cmd *cmd,
              int                    cmd_timeout)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);
  FpiUsbTransfer *transfer;
  GCancellable *cancellable = NULL;

  self->cmd = cmd;
  if (cmd_timeout != -1)
    self->cmd_timeout = cmd_timeout;

  if (cmd->devices != ELAN_ALL_DEV && !(cmd->devices & self->dev_type))
    {
      fp_dbg ("skipping command 0x%x 0x%x for this device (for devices 0x%x but device is 0x%x)",
              cmd->cmd[0], cmd->cmd[1], cmd->devices, self->dev_type);
      elan_cmd_done (ssm);
      return;
    }

  transfer = fpi_usb_transfer_new (dev);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;

  fpi_usb_transfer_fill_bulk_full (transfer,
                                   ELAN_EP_CMD_OUT,
                                   (guint8 *) cmd->cmd,
                                   ELAN_CMD_LEN,
                                   NULL);

  if (!self->cmd->never_cancel)
    cancellable = fpi_device_get_cancellable (dev);

  fpi_usb_transfer_submit (transfer,
                           self->cmd_timeout,
                           cancellable,
                           elan_cmd_cb,
                           NULL);
}

enum stop_capture_states {
  STOP_CAPTURE,
  STOP_CAPTURE_NUM_STATES,
};

static void
stop_capture_run_state (FpiSsm *ssm, FpDevice *dev)
{
  G_DEBUG_HERE ();

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case STOP_CAPTURE:
      elan_run_cmd (ssm, dev, &stop_cmd,
                    ELAN_CMD_TIMEOUT);
      break;
    }
}

static void
stop_capture_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();


  /* The device is inactive at this point. */
  self->active = FALSE;

  if (self->deactivating)
    {
      /* Simply complete the pending deactivation. */
      self->deactivating = FALSE;
      fpi_image_device_deactivate_complete (dev, error);
      return;
    }

  if (!error)
    fpi_image_device_report_finger_status (dev, FALSE);
  else
    /* NOTE: We cannot get a cancellation error here. */
    fpi_image_device_session_error (dev, error);
}

static void
elan_stop_capture (FpiDeviceElan *self)
{
  G_DEBUG_HERE ();

  elan_dev_reset_state (self);

  FpiSsm *ssm =
    fpi_ssm_new (FP_DEVICE (self), stop_capture_run_state, STOP_CAPTURE_NUM_STATES);

  fpi_ssm_start (ssm, stop_capture_complete);
}

enum capture_states {
  CAPTURE_LED_ON,
  CAPTURE_WAIT_FINGER,
  CAPTURE_READ_DATA,
  CAPTURE_CHECK_ENOUGH_FRAMES,
  CAPTURE_NUM_STATES,
};

static void
capture_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpImageDevice *idev = FP_IMAGE_DEVICE (dev);
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);
  int r;

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_LED_ON:
      elan_run_cmd (ssm, dev, &led_on_cmd, ELAN_CMD_TIMEOUT);
      break;

    case CAPTURE_WAIT_FINGER:
      elan_run_cmd (ssm, dev, &pre_scan_cmd, -1);
      break;

    case CAPTURE_READ_DATA:
      /* 0x55 - finger present
       * 0xff - device not calibrated (probably) */
      if (self->last_read && self->last_read[0] == 0x55)
        {
          fpi_image_device_report_finger_status (idev, TRUE);
          elan_run_cmd (ssm, dev, &get_image_cmd, ELAN_CMD_TIMEOUT);
        }
      else
        {
          /* XXX: The timeout is emulated incorrectly, resulting in a zero byte read. */
          if (g_strcmp0 (g_getenv ("FP_DEVICE_EMULATION"), "1") == 0)
            fpi_ssm_mark_completed (ssm);
          else
            fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
        }
      break;

    case CAPTURE_CHECK_ENOUGH_FRAMES:
      r = elan_save_img_frame (self);
      if (r < 0)
        {
          fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_GENERAL));
        }
      else if (self->num_frames < ELAN_MAX_FRAMES)
        {
          /* quickly stop if finger is removed */
          self->cmd_timeout = ELAN_FINGER_TIMEOUT;
          fpi_ssm_jump_to_state (ssm, CAPTURE_WAIT_FINGER);
        }
      else
        {
          fpi_ssm_next_state (ssm);
        }
      break;
    }
}

static void
capture_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceElan *self = FPI_DEVICE_ELAN (_dev);

  G_DEBUG_HERE ();

  /* either max frames captured or timed out waiting for the next frame */
  if (!error ||
      (g_error_matches (error, G_USB_DEVICE_ERROR, G_USB_DEVICE_ERROR_TIMED_OUT) &&
       fpi_ssm_get_cur_state (ssm) == CAPTURE_WAIT_FINGER))
    {
      if (self->num_frames >= ELAN_MIN_FRAMES)
        {
          elan_submit_image (dev);
        }
      else
        {
          fp_dbg ("swipe too short: want >= %d frames, got %d",
                  ELAN_MIN_FRAMES, self->num_frames);
          fpi_image_device_retry_scan (dev, FP_DEVICE_RETRY_TOO_SHORT);
        }
      g_clear_error (&error);
    }
  else
    {
      fpi_image_device_session_error (dev, error);
    }

  /* Note: We always stop capturing even if that may not be needed always.
   * Doing this between captures appears to make it at least less likely for
   * devices to end up in a bad state.
   */
  elan_stop_capture (self);
}

static void
elan_capture (FpiDeviceElan *self)
{
  G_DEBUG_HERE ();

  elan_dev_reset_state (self);
  FpiSsm *ssm =
    fpi_ssm_new (FP_DEVICE (self), capture_run_state, CAPTURE_NUM_STATES);

  fpi_ssm_start (ssm, capture_complete);
}

/* this function needs to have elandev->background and elandev->last_read to be
 * the calibration mean */
static int
elan_need_calibration (FpiDeviceElan *elandev)
{
  G_DEBUG_HERE ();

  unsigned short calib_mean =
    elandev->last_read[0] * 0xff + elandev->last_read[1];
  unsigned int bg_mean = 0, delta;
  unsigned int frame_size = elandev->frame_width * elandev->frame_height;

  g_assert (frame_size != 0);

  if (elandev->dev_type == ELAN_0C42)
    {
      if (calib_mean > 5500 ||
          calib_mean < 2500)
        {
          fp_dbg ("Forcing needed recalibration");
          return 1;
        }
    }

  for (int i = 0; i < frame_size; i++)
    bg_mean += elandev->background[i];
  bg_mean /= frame_size;

  delta =
    bg_mean > calib_mean ? bg_mean - calib_mean : calib_mean - bg_mean;

  fp_dbg ("calibration mean: %d, bg mean: %d, delta: %d", calib_mean,
          bg_mean, delta);

  return delta > ELAN_CALIBRATION_MAX_DELTA ? 1 : 0;
}

enum calibrate_states {
  CALIBRATE_GET_BACKGROUND,
  CALIBRATE_SAVE_BACKGROUND,
  CALIBRATE_GET_MEAN,
  CALIBRATE_CHECK_NEEDED,
  CALIBRATE_GET_STATUS,
  CALIBRATE_CHECK_STATUS,
  CALIBRATE_REPEAT_STATUS,
  CALIBRATE_NUM_STATES,
};

static gboolean
elan_supports_calibration (FpiDeviceElan *elandev)
{
  if (elandev->dev_type == ELAN_0C42)
    return TRUE;

  return elandev->fw_ver >= ELAN_MIN_CALIBRATION_FW;
}

static void
calibrate_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CALIBRATE_GET_BACKGROUND:
      elan_run_cmd (ssm, dev, &get_image_cmd, ELAN_CMD_TIMEOUT);
      break;

    case CALIBRATE_SAVE_BACKGROUND:
      elan_save_background (self);
      if (!elan_supports_calibration (self))
        {
          fp_dbg ("FW does not support calibration");
          fpi_ssm_mark_completed (ssm);
        }
      else
        {
          fpi_ssm_next_state (ssm);
        }
      break;

    case CALIBRATE_GET_MEAN:
      elan_run_cmd (ssm, dev, &get_calib_mean_cmd, ELAN_CMD_TIMEOUT);
      break;

    case CALIBRATE_CHECK_NEEDED:
      if (elan_need_calibration (self))
        {
          self->calib_status = 0;
          fpi_ssm_next_state (ssm);
        }
      else
        {
          fpi_ssm_mark_completed (ssm);
        }
      break;

    case CALIBRATE_GET_STATUS:
      self->calib_atts_left -= 1;
      if (self->calib_atts_left)
        {
          elan_run_cmd (ssm, dev, &get_calib_status_cmd,
                        ELAN_CMD_TIMEOUT);
        }
      else
        {
          fp_dbg ("calibration failed");
          fpi_ssm_mark_failed (ssm,
                               fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                         "Calibration failed!"));
        }
      break;

    case CALIBRATE_CHECK_STATUS:
      /* 0x01 - retry, 0x03 - ok
       * It appears that when reading the response soon after 0x4023 the device
       * can return 0x03, and only after some time (up to 100 ms) the response
       * changes to 0x01. It stays that way for some time and then changes back
       * to 0x03. Because of this we don't just expect 0x03, we want to see 0x01
       * first. This is to make sure that a full calibration loop has completed */
      fp_dbg ("calibration status: 0x%02x", self->last_read[0]);
      if (self->calib_status == 0x01 &&
          self->last_read[0] == 0x03)
        {
          self->calib_status = 0x03;
          fpi_ssm_jump_to_state (ssm, CALIBRATE_GET_BACKGROUND);
        }
      else
        {
          if (self->calib_status == 0x00 &&
              self->last_read[0] == 0x01)
            self->calib_status = 0x01;
          fpi_ssm_next_state_delayed (ssm, 50, NULL);
        }
      break;

    case CALIBRATE_REPEAT_STATUS:
      fpi_ssm_jump_to_state (ssm, CALIBRATE_GET_STATUS);
      break;
    }
}

static void
calibrate_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  G_DEBUG_HERE ();

  if (error)
    {
      fpi_image_device_session_error (FP_IMAGE_DEVICE (dev), error);
      elan_stop_capture (FPI_DEVICE_ELAN (dev));
    }
  else
    {
      elan_capture (FPI_DEVICE_ELAN (dev));
    }
}

static void
elan_calibrate (FpiDeviceElan *self)
{
  G_DEBUG_HERE ();

  elan_dev_reset_state (self);

  g_return_if_fail (!self->active);
  self->active = TRUE;
  self->calib_atts_left = ELAN_CALIBRATION_ATTEMPTS;

  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (self), calibrate_run_state,
                             CALIBRATE_NUM_STATES);

  fpi_ssm_start (ssm, calibrate_complete);
}

enum activate_states {
  ACTIVATE_GET_FW_VER,
  ACTIVATE_SET_FW_VER,
  ACTIVATE_GET_SENSOR_DIM,
  ACTIVATE_SET_SENSOR_DIM,
  ACTIVATE_CMD_1,
  ACTIVATE_NUM_STATES,
};

static void
activate_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case ACTIVATE_GET_FW_VER:
      elan_run_cmd (ssm, dev, &get_fw_ver_cmd, ELAN_CMD_TIMEOUT);
      break;

    case ACTIVATE_SET_FW_VER:
      self->fw_ver =
        (self->last_read[0] << 8 | self->last_read[1]);
      fp_dbg ("FW ver 0x%04hx", self->fw_ver);
      fpi_ssm_next_state (ssm);
      break;

    case ACTIVATE_GET_SENSOR_DIM:
      elan_run_cmd (ssm, dev, &get_sensor_dim_cmd, ELAN_CMD_TIMEOUT);
      break;

    case ACTIVATE_SET_SENSOR_DIM:
      /* see elan_save_frame for details */
      if (self->dev_type & ELAN_NOT_ROTATED)
        {
          self->frame_width = self->last_read[0];
          self->frame_height = self->raw_frame_height =
            self->last_read[2];
        }
      else
        {
          self->frame_width = self->last_read[2];
          self->frame_height = self->raw_frame_height =
            self->last_read[0];
        }
      /* Work-around sensors returning the sizes as zero-based index
       * rather than the number of pixels. */
      if ((self->frame_width % 2 == 1) &&
          (self->frame_height % 2 == 1))
        {
          self->frame_width++;
          self->frame_height++;
          self->raw_frame_height = self->frame_height;
        }
      if (self->frame_height > ELAN_MAX_FRAME_HEIGHT)
        self->frame_height = ELAN_MAX_FRAME_HEIGHT;
      fp_dbg ("sensor dimensions, WxH: %dx%d", self->frame_width,
              self->raw_frame_height);
      fpi_ssm_next_state (ssm);
      break;

    case ACTIVATE_CMD_1:
      /* TODO: find out what this does, if we need it */
      elan_run_cmd (ssm, dev, &activate_cmd_1, ELAN_CMD_TIMEOUT);
      break;
    }
}

static void
activate_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpImageDevice *idev = FP_IMAGE_DEVICE (dev);

  G_DEBUG_HERE ();

  fpi_image_device_activate_complete (idev, error);

}

static void
elan_activate (FpImageDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();
  elan_dev_reset_state (self);

  FpiSsm *ssm =
    fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                 ACTIVATE_NUM_STATES);

  fpi_ssm_start (ssm, activate_complete);
}

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;
  FpiDeviceElan *self;

  G_DEBUG_HERE ();

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  self = FPI_DEVICE_ELAN (dev);

  /* common params */
  self->dev_type = fpi_device_get_driver_data (FP_DEVICE (dev));
  self->background = NULL;
  self->process_frame = elan_process_frame_thirds;

  switch (self->dev_type)
    {
    case ELAN_0907:
      self->process_frame = elan_process_frame_linear;
      break;
    }

  fpi_image_device_open_complete (dev, NULL);
}

static void
dev_deinit (FpImageDevice *dev)
{
  GError *error = NULL;
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  elan_dev_reset_state (self);
  g_free (self->background);
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);
  fpi_image_device_close_complete (dev, error);
}

static void
dev_activate (FpImageDevice *dev)
{
  G_DEBUG_HERE ();
  elan_activate (dev);
}

static void
dev_change_state (FpImageDevice *dev, FpiImageDeviceState state)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  /* Note: We always calibrate even if that may not be needed always.
   * Doing this for each capture appears to make it at least less likely for
   * devices to end up in a bad state.
   */
  if (state == FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON)
    elan_calibrate (self);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceElan *self = FPI_DEVICE_ELAN (dev);

  G_DEBUG_HERE ();

  if (!self->active)
    /* The device is inactive already, complete the operation immediately. */
    fpi_image_device_deactivate_complete (dev, NULL);
  else
    /* The device is not yet inactive, flag that we are deactivating (and
     * need to signal back deactivation).
     * Note that any running capture will be cancelled already if needed. */
    self->deactivating = TRUE;
}

static void
fpi_device_elan_init (FpiDeviceElan *self)
{
}
static void
fpi_device_elan_class_init (FpiDeviceElanClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "elan";
  dev_class->full_name = "ElanTech Fingerprint Sensor";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = elan_id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;
  img_class->change_state = dev_change_state;

  img_class->bz3_threshold = 24;
}
