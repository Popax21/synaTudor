/*
 * AuthenTec AES1610 driver for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2007 Cyrille Bagard
 * Copyright (C) 2007 Vasily Khoruzhick
 * Copyright (C) 2009 Guido Grazioli <guido.grazioli@gmail.com>
 * Copyright (C) 2012 Vasily Khoruzhick <anarsoul@gmail.com>
 *
 * Based on code from libfprint aes2501 driver.
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

#define FP_COMPONENT "aes1610"

#include "drivers_api.h"
#include "aeslib.h"

static void start_capture (FpImageDevice *dev);
static void complete_deactivation (FpImageDevice *dev);
static int adjust_gain (unsigned char *buffer,
                        int            status);

#define FIRST_AES1610_REG 0x1B
#define LAST_AES1610_REG 0xFF

#define GAIN_STATUS_FIRST 1
#define GAIN_STATUS_NORMAL 2

/* FIXME these need checking */
#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)

#define BULK_TIMEOUT 4000

#define FINGER_DETECTION_LEN 19
#define STRIP_CAPTURE_LEN 665

/*
 * The AES1610 is an imaging device using a swipe-type sensor. It samples
 * the finger at preprogrammed intervals, sending a 128x8 frame to the
 * computer.
 * Unless the user is scanning their finger unreasonably fast, the frames
 * *will* overlap. The implementation below detects this overlap and produces
 * a contiguous image as the end result.
 * The fact that the user determines the length of the swipe (and hence the
 * number of useful frames) and also the fact that overlap varies means that
 * images returned from this driver vary in height.
 */

#define FRAME_WIDTH 128
#define FRAME_HEIGHT 8
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT)
#define IMAGE_WIDTH (FRAME_WIDTH + (FRAME_WIDTH / 2))
/* maximum number of frames to read during a scan */
/* FIXME reduce substantially */
#define MAX_FRAMES 350

/****** GENERAL FUNCTIONS ******/

struct _FpiDeviceAes1610
{
  FpImageDevice parent;

  guint8        read_regs_retry_count;
  GSList       *strips;
  gsize         strips_len;
  gboolean      deactivating;
  guint8        blanks_count;
};
G_DECLARE_FINAL_TYPE (FpiDeviceAes1610, fpi_device_aes1610, FPI, DEVICE_AES1610,
                      FpImageDevice);
G_DEFINE_TYPE (FpiDeviceAes1610, fpi_device_aes1610, FP_TYPE_IMAGE_DEVICE);

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = FRAME_WIDTH,
  .frame_height = FRAME_HEIGHT,
  .image_width = IMAGE_WIDTH,
  .get_pixel = aes_get_pixel,
};

typedef void (*aes1610_read_regs_cb)(FpImageDevice *dev,
                                     int            status,
                                     unsigned char *regs,
                                     void          *user_data);

struct aes1610_read_regs
{
  FpImageDevice       *dev;
  aes1610_read_regs_cb callback;
  struct aes_regwrite *regwrite;
  void                *user_data;
};

/* FIXME: what to do here? */
static void
stub_capture_stop_cb (FpImageDevice *dev, GError *error,
                      void *user_data)
{
  if (error)
    {
      fp_warn ("Error stopping capture: %s", error->message);
      g_error_free (error);
    }
}

static void
generic_write_regv_cb (FpImageDevice *dev, GError *error,
                       void *user_data)
{
  FpiSsm *ssm = user_data;

  if (!error)
    fpi_ssm_next_state (ssm);
  else
    fpi_ssm_mark_failed (ssm, error);
}

/* read the specified number of bytes from the IN endpoint but throw them
 * away, then increment the SSM */
static void
generic_read_ignore_data (FpiSsm *ssm, FpDevice *dev,
                          size_t bytes)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);

  fpi_usb_transfer_fill_bulk (transfer, EP_IN, bytes);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           fpi_ssm_usb_transfer_cb, NULL);
}

/****** FINGER PRESENCE DETECTION ******/


static const struct aes_regwrite finger_det_reqs[] = {
  { 0x80, 0x01 },
  { 0x80, 0x12 },
  { 0x85, 0x00 },
  { 0x8A, 0x00 },
  { 0x8B, 0x0E },
  { 0x8C, 0x90 },
  { 0x8D, 0x83 },
  { 0x8E, 0x07 },
  { 0x8F, 0x07 },
  { 0x96, 0x00 },
  { 0x97, 0x48 },
  { 0xA1, 0x00 },
  { 0xA2, 0x50 },
  { 0xA6, 0xE4 },
  { 0xAD, 0x08 },
  { 0xAE, 0x5B },
  { 0xAF, 0x54 },
  { 0xB1, 0x28 },
  { 0xB5, 0xAB },
  { 0xB6, 0x0E },
  { 0x1B, 0x2D },
  { 0x81, 0x04 }
};

static void start_finger_detection (FpImageDevice *dev);

static void
finger_det_data_cb (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  unsigned char *data = transfer->buffer;
  int i;
  int sum = 0;

  if (error)
    {
      fpi_image_device_session_error (dev, error);
      return;
    }

  /* examine histogram to determine finger presence */
  for (i = 3; i < 17; i++)
    sum += (data[i] & 0xf) + (data[i] >> 4);
  if (sum > 20)
    {
      /* reset default gain */
      adjust_gain (data, GAIN_STATUS_FIRST);
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
finger_det_reqs_cb (FpImageDevice *dev, GError *error,
                    void *user_data)
{
  FpiUsbTransfer *transfer;

  if (error)
    {
      fpi_image_device_session_error (dev, error);
      return;
    }

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  fpi_usb_transfer_fill_bulk (transfer, EP_IN, FINGER_DETECTION_LEN);
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_data_cb, NULL);
}

static void
start_finger_detection (FpImageDevice *dev)
{
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  aes_write_regv (dev, finger_det_reqs, G_N_ELEMENTS (finger_det_reqs), finger_det_reqs_cb, NULL);

}

/****** CAPTURE ******/

static struct aes_regwrite capture_reqs[] = {
  { 0x80, 0x01 },
  { 0x80, 0x12 },
  { 0x84, 0x01 },
  { 0x85, 0x00 },
  { 0x89, 0x64 },
  { 0x8A, 0x00 },
  { 0x8B, 0x0E },
  { 0x8C, 0x90 },
  { 0xBE, 0x23 },
  { 0x29, 0x04 },
  { 0x2A, 0xFF },
  { 0x96, 0x00 },
  { 0x98, 0x03 },
  { 0x99, 0x00 },
  { 0x9C, 0xA5 },
  { 0x9D, 0x40 },
  { 0x9E, 0xC6 },
  { 0x9F, 0x8E },
  { 0xA2, 0x50 },
  { 0xA3, 0xF0 },
  { 0xAD, 0x08 },
  { 0xBD, 0x4F },
  { 0xAF, 0x54 },
  { 0xB1, 0x08 },
  { 0xB5, 0xAB },
  { 0x1B, 0x2D },
  { 0xB6, 0x4E },
  { 0xB8, 0x70 },
  { 0x2B, 0xB3 },
  { 0x2C, 0x5D },
  { 0x2D, 0x98 },
  { 0x2E, 0xB0 },
  { 0x2F, 0x20 },
  { 0xA2, 0xD0 },
  { 0x1D, 0x21 },
  { 0x1E, 0xBE },
  { 0x1C, 0x00 },
  { 0x1D, 0x30 },
  { 0x1E, 0x29 },
  { 0x1C, 0x01 },
  { 0x1D, 0x00 },
  { 0x1E, 0x9E },
  { 0x1C, 0x02 },
  { 0x1D, 0x30 },
  { 0x1E, 0xBB },
  { 0x1C, 0x03 },
  { 0x1D, 0x00 },
  { 0x1E, 0x9D },
  { 0x1C, 0x04 },
  { 0x1D, 0x22 },
  { 0x1E, 0xFF },
  { 0x1C, 0x05 },
  { 0x1D, 0x1B },
  { 0x1E, 0x4E },
  { 0x1C, 0x06 },
  { 0x1D, 0x16 },
  { 0x1E, 0x28 },
  { 0x1C, 0x07 },
  { 0x1D, 0x22 },
  { 0x1E, 0xFF },
  { 0x1C, 0x08 },
  { 0x1D, 0x15 },
  { 0x1E, 0xF1 },
  { 0x1C, 0x09 },
  { 0x1D, 0x30 },
  { 0x1E, 0xD5 },
  { 0x1C, 0x0A },
  { 0x1D, 0x00 },
  { 0x1E, 0x9E },
  { 0x1C, 0x0B },
  { 0x1D, 0x17 },
  { 0x1E, 0x9D },
  { 0x1C, 0x0C },
  { 0x1D, 0x28 },
  { 0x1E, 0xD7 },
  { 0x1C, 0x0D },
  { 0x1D, 0x17 },
  { 0x1E, 0xD7 },
  { 0x1C, 0x0E },
  { 0x1D, 0x0A },
  { 0x1E, 0xCB },
  { 0x1C, 0x0F },
  { 0x1D, 0x24 },
  { 0x1E, 0x14 },
  { 0x1C, 0x10 },
  { 0x1D, 0x17 },
  { 0x1E, 0x85 },
  { 0x1C, 0x11 },
  { 0x1D, 0x15 },
  { 0x1E, 0x71 },
  { 0x1C, 0x12 },
  { 0x1D, 0x2B },
  { 0x1E, 0x36 },
  { 0x1C, 0x13 },
  { 0x1D, 0x12 },
  { 0x1E, 0x06 },
  { 0x1C, 0x14 },
  { 0x1D, 0x30 },
  { 0x1E, 0x97 },
  { 0x1C, 0x15 },
  { 0x1D, 0x21 },
  { 0x1E, 0x32 },
  { 0x1C, 0x16 },
  { 0x1D, 0x06 },
  { 0x1E, 0xE6 },
  { 0x1C, 0x17 },
  { 0x1D, 0x16 },
  { 0x1E, 0x06 },
  { 0x1C, 0x18 },
  { 0x1D, 0x30 },
  { 0x1E, 0x01 },
  { 0x1C, 0x19 },
  { 0x1D, 0x21 },
  { 0x1E, 0x37 },
  { 0x1C, 0x1A },
  { 0x1D, 0x00 },
  { 0x1E, 0x08 },
  { 0x1C, 0x1B },
  { 0x1D, 0x80 },
  { 0x1E, 0xD5 },
  { 0xA2, 0x50 },
  { 0xA2, 0x50 },
  { 0x81, 0x01 }
};

static struct aes_regwrite strip_scan_reqs[] = {
  { 0xBE, 0x23 },
  { 0x29, 0x04 },
  { 0x2A, 0xFF },
  { 0xBD, 0x4F },
  { 0xFF, 0x00 }
};

static const struct aes_regwrite capture_stop[] = {
  { 0x81, 0x00 }
};

/*
 * The different possible values for 0xBE register */
static unsigned char list_BE_values[10] = {
  0x23, 0x43, 0x63, 0x64, 0x65, 0x67, 0x6A, 0x6B
};

/*
 * The different possible values for 0xBD register */
static unsigned char list_BD_values[10] = {
  0x28, 0x2b, 0x30, 0x3b, 0x45, 0x49, 0x4B
};

/*
 * Adjust the gain according to the histogram data
 * 0xbd, 0xbe, 0x29 and 0x2A registers are affected
 * Returns 0 if no problem occurred
 * TODO: This is a basic support for gain. It needs testing/tweaking.  */
static int
adjust_gain (unsigned char *buffer, int status)
{
  // The position in the array of possible values for 0xBE and 0xBD registers
  static int pos_list_BE = 0;
  static int pos_list_BD = 0;

  // This is the first adjustment (we begin acquisition)
  // We adjust strip_scan_reqs for future strips and capture_reqs that is sent just after this step
  if (status == GAIN_STATUS_FIRST)
    {
      if (buffer[1] > 0x78)             // maximum gain needed
        {
          strip_scan_reqs[0].value = 0x6B;
          strip_scan_reqs[1].value = 0x06;
          strip_scan_reqs[2].value = 0x35;
          strip_scan_reqs[3].value = 0x4B;
        }
      else if (buffer[1] > 0x55)
        {
          strip_scan_reqs[0].value = 0x63;
          strip_scan_reqs[1].value = 0x15;
          strip_scan_reqs[2].value = 0x35;
          strip_scan_reqs[3].value = 0x3b;
        }
      else if (buffer[1] > 0x40 || buffer[16] > 0x19)
        {
          strip_scan_reqs[0].value = 0x43;
          strip_scan_reqs[1].value = 0x13;
          strip_scan_reqs[2].value = 0x35;
          strip_scan_reqs[3].value = 0x30;
        }
      else             // minimum gain needed
        {
          strip_scan_reqs[0].value = 0x23;
          strip_scan_reqs[1].value = 0x07;
          strip_scan_reqs[2].value = 0x35;
          strip_scan_reqs[3].value = 0x28;
        }

      // Now copy this values in capture_reqs
      capture_reqs[8].value = strip_scan_reqs[0].value;
      capture_reqs[9].value = strip_scan_reqs[1].value;
      capture_reqs[10].value = strip_scan_reqs[2].value;
      capture_reqs[21].value = strip_scan_reqs[3].value;

      fp_dbg ("first gain: %x %x %x %x %x %x %x %x", strip_scan_reqs[0].reg, strip_scan_reqs[0].value, strip_scan_reqs[1].reg, strip_scan_reqs[1].value, strip_scan_reqs[2].reg, strip_scan_reqs[2].value, strip_scan_reqs[3].reg, strip_scan_reqs[3].value);
    }
  // Every 2/3 strips
  // We try to soften big changes of the gain (at least for 0xBE and 0xBD
  // FIXME: This softenning will need testing and tweaking too
  else if (status == GAIN_STATUS_NORMAL)
    {
      if (buffer[514] > 0x78)             // maximum gain needed
        {
          if (pos_list_BE < 7)
            pos_list_BE++;

          if (pos_list_BD < 6)
            pos_list_BD++;

          strip_scan_reqs[1].value = 0x04;
          strip_scan_reqs[2].value = 0x35;
        }
      else if (buffer[514] > 0x55)
        {
          if (pos_list_BE < 2)
            pos_list_BE++;
          else if (pos_list_BE > 2)
            pos_list_BE--;

          if (pos_list_BD < 2)
            pos_list_BD++;
          else if (pos_list_BD > 2)
            pos_list_BD--;

          strip_scan_reqs[1].value = 0x15;
          strip_scan_reqs[2].value = 0x35;
        }
      else if (buffer[514] > 0x40 || buffer[529] > 0x19)
        {
          if (pos_list_BE < 1)
            pos_list_BE++;
          else if (pos_list_BE > 1)
            pos_list_BE--;

          if (pos_list_BD < 1)
            pos_list_BD++;
          else if (pos_list_BD > 1)
            pos_list_BD--;

          strip_scan_reqs[1].value = 0x13;
          strip_scan_reqs[2].value = 0x35;
        }
      else             // minimum gain needed
        {
          if (pos_list_BE > 0)
            pos_list_BE--;

          if (pos_list_BD > 0)
            pos_list_BD--;

          strip_scan_reqs[1].value = 0x07;
          strip_scan_reqs[2].value = 0x35;
        }

      strip_scan_reqs[0].value = list_BE_values[pos_list_BE];
      strip_scan_reqs[3].value = list_BD_values[pos_list_BD];

      fp_dbg ("gain: %x %x %x %x %x %x %x %x", strip_scan_reqs[0].reg, strip_scan_reqs[0].value, strip_scan_reqs[1].reg, strip_scan_reqs[1].value, strip_scan_reqs[2].reg, strip_scan_reqs[2].value, strip_scan_reqs[3].reg, strip_scan_reqs[3].value);
    }
  // Unknown status
  else
    {
      fp_err ("Unexpected gain status.");
      return 1;
    }

  return 0;
}

/*
 * Restore the default gain values */
static void
restore_gain (void)
{
  strip_scan_reqs[0].value = list_BE_values[0];
  strip_scan_reqs[1].value = 0x04;
  strip_scan_reqs[2].value = 0xFF;
  strip_scan_reqs[3].value = list_BD_values[0];

  capture_reqs[8].value = list_BE_values[0];
  capture_reqs[9].value = 0x04;
  capture_reqs[10].value = 0xFF;
  capture_reqs[21].value = list_BD_values[0];
}


/* capture SM movement:
 * request and read strip,
 * jump back to request UNLESS there's no finger, in which case exit SM,
 * report lack of finger presence, and move to finger detection */

enum capture_states {
  CAPTURE_WRITE_REQS,
  CAPTURE_READ_DATA,
  CAPTURE_REQUEST_STRIP,
  CAPTURE_READ_STRIP,
  CAPTURE_NUM_STATES,
};

static void
capture_read_strip_cb (FpiUsbTransfer *transfer, FpDevice *device,
                       gpointer user_data, GError *error)
{
  unsigned char *stripdata;
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);
  unsigned char *data = transfer->buffer;
  gint sum, i;

  if (error)
    {
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  /* FIXME: would preallocating strip buffers be a decent optimization? */

  sum = 0;
  for (i = 516; i < 530; i++)
    /* histogram[i] = number of pixels of value i
       Only the pixel values from 10 to 15 are used to detect finger. */
    sum += data[i];

  fp_dbg ("sum=%d", sum);
  if (sum > 0)
    {
      /* FIXME: would preallocating strip buffers be a decent optimization? */
      struct fpi_frame *stripe = g_malloc (FRAME_WIDTH * (FRAME_HEIGHT / 2) + sizeof (struct fpi_frame));
      stripe->delta_x = 0;
      stripe->delta_y = 0;
      stripdata = stripe->data;
      memcpy (stripdata, data + 1, FRAME_WIDTH * (FRAME_HEIGHT / 2));
      self->strips = g_slist_prepend (self->strips, stripe);
      self->strips_len++;
      self->blanks_count = 0;
    }
  else
    {
      /* FIXME: 0 might be too low as a threshold */
      /* FIXME: sometimes we get 0 in the middle of a scan, should we wait for
       * a few consecutive zeroes? */

      /* sum cannot be negative, so is 0 */
      self->blanks_count++;
      fp_dbg ("got blank frame");
    }


  /* use histogram data above for gain calibration (0xbd, 0xbe, 0x29 and 0x2A ) */
  adjust_gain (data, GAIN_STATUS_NORMAL);

  /* stop capturing if MAX_FRAMES is reached */
  if (self->blanks_count > 10 || g_slist_length (self->strips) >= MAX_FRAMES)
    {
      FpImage *img;

      fp_dbg ("sending stop capture.... blanks=%d  frames=%d",
              self->blanks_count, g_slist_length (self->strips));
      /* send stop capture bits */
      aes_write_regv (dev, capture_stop, G_N_ELEMENTS (capture_stop), stub_capture_stop_cb, NULL);
      self->strips = g_slist_reverse (self->strips);
      fpi_do_movement_estimation (&assembling_ctx, self->strips);
      img = fpi_assemble_frames (&assembling_ctx, self->strips);
      img->flags |= FPI_IMAGE_PARTIAL;

      g_slist_free_full (self->strips, g_free);
      self->strips = NULL;
      self->strips_len = 0;
      self->blanks_count = 0;
      fpi_image_device_image_captured (dev, img);
      fpi_image_device_report_finger_status (dev, FALSE);
      /* marking machine complete will re-trigger finger detection loop */
      fpi_ssm_mark_completed (transfer->ssm);
      /* Acquisition finished: restore default gain values */
      restore_gain ();
    }
  else
    {
      /* obtain next strip */
      fpi_ssm_jump_to_state (transfer->ssm, CAPTURE_REQUEST_STRIP);
    }
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_WRITE_REQS:
      fp_dbg ("write reqs");
      aes_write_regv (dev, capture_reqs, G_N_ELEMENTS (capture_reqs),
                      generic_write_regv_cb, ssm);
      break;

    case CAPTURE_READ_DATA:
      fp_dbg ("read data");
      generic_read_ignore_data (ssm, _dev, STRIP_CAPTURE_LEN);
      break;

    case CAPTURE_REQUEST_STRIP:
      fp_dbg ("request strip");
      if (self->deactivating)
        fpi_ssm_mark_completed (ssm);
      else
        aes_write_regv (dev, strip_scan_reqs, G_N_ELEMENTS (strip_scan_reqs),
                        generic_write_regv_cb, ssm);
      break;

    case CAPTURE_READ_STRIP:;
      FpiUsbTransfer *transfer = fpi_usb_transfer_new (_dev);

      fpi_usb_transfer_fill_bulk (transfer, EP_IN, STRIP_CAPTURE_LEN);
      transfer->ssm = ssm;
      transfer->short_is_error = TRUE;
      fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                               capture_read_strip_cb, NULL);
      break;
    }
  ;
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (_dev);

  G_DEBUG_HERE ();
  if (self->deactivating)
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
      start_finger_detection (dev);
    }
}

static void
start_capture (FpImageDevice *dev)
{
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);
  FpiSsm *ssm;

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state,
                     CAPTURE_NUM_STATES);
  G_DEBUG_HERE ();
  fpi_ssm_start (ssm, capture_sm_complete);
}

/****** INITIALIZATION/DEINITIALIZATION ******/

static const struct aes_regwrite init[] = {
  { 0x82, 0x00 }
};

/*
   static const struct aes_regwrite stop_reader[] = {
        { 0xFF, 0x00 }
   };
 */

enum activate_states {
  WRITE_INIT,
  ACTIVATE_NUM_STATES,
};

static void
activate_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);

  /* activation on aes1610 seems much more straightforward compared to aes2501 */
  /* verify there's anything missing here */
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case WRITE_INIT:
      fp_dbg ("write init");
      aes_write_regv (dev, init, G_N_ELEMENTS (init), generic_write_regv_cb, ssm);
      break;
    }
}

/* jump to finger detection */
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
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  self->read_regs_retry_count = 0;
  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);

  /* FIXME: audit cancellation points, probably need more, specifically
   * in error handling paths? */
  self->deactivating = TRUE;
}

static void
complete_deactivation (FpImageDevice *dev)
{
  FpiDeviceAes1610 *self = FPI_DEVICE_AES1610 (dev);

  G_DEBUG_HERE ();

  /* FIXME: if we're in the middle of a scan, we should cancel the scan.
   * maybe we can do this with a master reset, unconditionally? */

  self->deactivating = FALSE;
  g_slist_free (self->strips);
  self->strips = NULL;
  self->strips_len = 0;
  self->blanks_count = 0;
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;

  /* FIXME check endpoints */

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
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
  { .vid = 0x08ff,  .pid = 0x1600, },/* AES1600 */
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_aes1610_init (FpiDeviceAes1610 *self)
{
}
static void
fpi_device_aes1610_class_init (FpiDeviceAes1610Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "aes1610";
  dev_class->full_name = "AuthenTec AES1610";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 20;

  img_class->img_width = IMAGE_WIDTH;
  img_class->img_height = -1;
}
