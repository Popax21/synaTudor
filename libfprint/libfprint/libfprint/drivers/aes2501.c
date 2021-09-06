/*
 * AuthenTec AES2501 driver for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2007 Cyrille Bagard
 * Copyright (C) 2007-2008, 2012 Vasily Khoruzhick <anarsoul@gmail.com>
 *
 * Based on code from http://home.gna.org/aes2501, relicensed with permission
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

#define FP_COMPONENT "aes2501"

#include "drivers_api.h"
#include "aeslib.h"
#include "aes2501.h"

static void start_capture (FpImageDevice *dev);
static void complete_deactivation (FpImageDevice *dev);

/* FIXME these need checking */
#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)

#define BULK_TIMEOUT 4000

#define FINGER_DETECTION_LEN 20
#define READ_REGS_LEN 126
#define READ_REGS_RESP_LEN 159
#define STRIP_CAPTURE_LEN 1705

/*
 * The AES2501 is an imaging device using a swipe-type sensor. It samples
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
#define FRAME_HEIGHT 16
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT)
#define IMAGE_WIDTH (FRAME_WIDTH + (FRAME_WIDTH / 2))
/* maximum number of frames to read during a scan */
/* FIXME reduce substantially */
#define MAX_FRAMES 150

/****** GENERAL FUNCTIONS ******/

struct _FpiDeviceAes2501
{
  FpImageDevice parent;

  guint8        read_regs_retry_count;
  GSList       *strips;
  size_t        strips_len;
  gboolean      deactivating;
  int           no_finger_cnt;
};
G_DECLARE_FINAL_TYPE (FpiDeviceAes2501, fpi_device_aes2501, FPI, DEVICE_AES2501,
                      FpImageDevice);
G_DEFINE_TYPE (FpiDeviceAes2501, fpi_device_aes2501, FP_TYPE_IMAGE_DEVICE);

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = FRAME_WIDTH,
  .frame_height = FRAME_HEIGHT,
  .image_width = IMAGE_WIDTH,
  .get_pixel = aes_get_pixel,
};

typedef void (*aes2501_read_regs_cb)(FpImageDevice *dev,
                                     GError        *error,
                                     unsigned char *regs,
                                     void          *user_data);

struct aes2501_read_regs
{
  FpImageDevice       *dev;
  aes2501_read_regs_cb callback;
  struct aes_regwrite *regwrite;
  void                *user_data;
};

static void
read_regs_data_cb (FpiUsbTransfer *transfer, FpDevice *dev,
                   gpointer user_data, GError *error)
{
  struct aes2501_read_regs *rdata = user_data;

  rdata->callback (rdata->dev, error, transfer->buffer, rdata->user_data);
  g_free (rdata);
}

static void
read_regs_rq_cb (FpImageDevice *dev, GError *error, void *user_data)
{
  struct aes2501_read_regs *rdata = user_data;
  FpiUsbTransfer *transfer;

  g_free (rdata->regwrite);
  if (error)
    {
      rdata->callback (dev, error, NULL, rdata->user_data);
      g_free (rdata);
      return;
    }

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer, EP_IN, READ_REGS_LEN);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           read_regs_data_cb, rdata);
}

static void
read_regs (FpImageDevice *dev, aes2501_read_regs_cb callback,
           void *user_data)
{
  /* FIXME: regwrite is dynamic because of asynchronity. is this really
   * required? */
  struct aes_regwrite *regwrite = g_malloc (sizeof (*regwrite));
  struct aes2501_read_regs *rdata = g_malloc (sizeof (*rdata));

  G_DEBUG_HERE ();
  regwrite->reg = AES2501_REG_CTRL2;
  regwrite->value = AES2501_CTRL2_READ_REGS;
  rdata->dev = dev;
  rdata->callback = callback;
  rdata->user_data = user_data;
  rdata->regwrite = regwrite;

  aes_write_regv (dev, (const struct aes_regwrite *) regwrite, 1,
                  read_regs_rq_cb, rdata);
}

/* Read the value of a specific register from a register dump */
static int
regval_from_dump (unsigned char *data, guint8 target)
{
  if (*data != FIRST_AES2501_REG)
    {
      fp_err ("not a register dump");
      return -1;
    }

  if (!(FIRST_AES2501_REG <= target && target <= LAST_AES2501_REG))
    {
      fp_err ("out of range");
      return -1;
    }

  target -= FIRST_AES2501_REG;
  target *= 2;
  return data[target + 1];
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
  FpiUsbTransfer *transfer;

  transfer = fpi_usb_transfer_new (dev);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer, EP_IN, bytes);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           fpi_ssm_usb_transfer_cb, NULL);
}

/****** IMAGE PROCESSING ******/

static int
sum_histogram_values (unsigned char *data, guint8 threshold)
{
  int r = 0;
  int i;
  guint16 *histogram = (guint16 *) (data + 1);

  if (*data != 0xde)
    return -1;

  if (threshold > 0x0f)
    return -1;

  /* FIXME endianness */
  for (i = threshold; i < 16; i++)
    r += histogram[i];

  return r;
}

/****** FINGER PRESENCE DETECTION ******/

static const struct aes_regwrite finger_det_reqs[] = {
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_DETCTRL,
    AES2501_DETCTRL_DRATE_CONTINUOUS | AES2501_DETCTRL_SDELAY_31_MS },
  { AES2501_REG_COLSCAN, AES2501_COLSCAN_SRATE_128_US },
  { AES2501_REG_MEASDRV, AES2501_MEASDRV_MDRIVE_0_325 | AES2501_MEASDRV_MEASURE_SQUARE },
  { AES2501_REG_MEASFREQ, AES2501_MEASFREQ_2M },
  { AES2501_REG_DEMODPHASE1, DEMODPHASE_NONE },
  { AES2501_REG_DEMODPHASE2, DEMODPHASE_NONE },
  { AES2501_REG_CHANGAIN,
    AES2501_CHANGAIN_STAGE2_4X | AES2501_CHANGAIN_STAGE1_16X },
  { AES2501_REG_ADREFHI, 0x44 },
  { AES2501_REG_ADREFLO, 0x34 },
  { AES2501_REG_STRTCOL, 0x16 },
  { AES2501_REG_ENDCOL, 0x16 },
  { AES2501_REG_DATFMT, AES2501_DATFMT_BIN_IMG | 0x08 },
  { AES2501_REG_TREG1, 0x70 },
  { 0xa2, 0x02 },
  { 0xa7, 0x00 },
  { AES2501_REG_TREGC, AES2501_TREGC_ENABLE },
  { AES2501_REG_TREGD, 0x1a },
  { 0, 0 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
  { AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
  { AES2501_REG_LPONT, AES2501_LPONT_MIN_VALUE },
};

static void start_finger_detection (FpImageDevice *dev);

static void
finger_det_data_cb (FpiUsbTransfer *transfer, FpDevice *_dev,
                    gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  unsigned char *data = transfer->buffer;
  int i;
  int sum = 0;

  if (error)
    {
      fpi_image_device_session_error (dev, error);
      return;
    }

  /* examine histogram to determine finger presence */
  for (i = 1; i < 9; i++)
    sum += (data[i] & 0xf) + (data[i] >> 4);
  if (sum > 20)
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
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer, EP_IN, FINGER_DETECTION_LEN);
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           finger_det_data_cb, NULL);
}

static void
start_finger_detection (FpImageDevice *dev)
{
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);

  G_DEBUG_HERE ();

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  aes_write_regv (dev, finger_det_reqs, G_N_ELEMENTS (finger_det_reqs),
                  finger_det_reqs_cb, NULL);
}

/****** CAPTURE ******/

static const struct aes_regwrite capture_reqs_1[] = {
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { 0, 0 },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_DETCTRL,
    AES2501_DETCTRL_SDELAY_31_MS | AES2501_DETCTRL_DRATE_CONTINUOUS },
  { AES2501_REG_COLSCAN, AES2501_COLSCAN_SRATE_128_US },
  { AES2501_REG_DEMODPHASE2, 0x7c },
  { AES2501_REG_MEASDRV,
    AES2501_MEASDRV_MEASURE_SQUARE | AES2501_MEASDRV_MDRIVE_0_325 },
  { AES2501_REG_DEMODPHASE1, 0x24 },
  { AES2501_REG_CHWORD1, 0x00 },
  { AES2501_REG_CHWORD2, 0x6c },
  { AES2501_REG_CHWORD3, 0x09 },
  { AES2501_REG_CHWORD4, 0x54 },
  { AES2501_REG_CHWORD5, 0x78 },
  { 0xa2, 0x02 },
  { 0xa7, 0x00 },
  { 0xb6, 0x26 },
  { 0xb7, 0x1a },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
  { AES2501_REG_IMAGCTRL,
    AES2501_IMAGCTRL_TST_REG_ENABLE | AES2501_IMAGCTRL_HISTO_DATA_ENABLE |
    AES2501_IMAGCTRL_IMG_DATA_DISABLE },
  { AES2501_REG_STRTCOL, 0x10 },
  { AES2501_REG_ENDCOL, 0x1f },
  { AES2501_REG_CHANGAIN,
    AES2501_CHANGAIN_STAGE1_2X | AES2501_CHANGAIN_STAGE2_2X },
  { AES2501_REG_ADREFHI, 0x70 },
  { AES2501_REG_ADREFLO, 0x20 },
  { AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
  { AES2501_REG_LPONT, AES2501_LPONT_MIN_VALUE },
};

static const struct aes_regwrite capture_reqs_2[] = {
  { AES2501_REG_IMAGCTRL,
    AES2501_IMAGCTRL_TST_REG_ENABLE | AES2501_IMAGCTRL_HISTO_DATA_ENABLE |
    AES2501_IMAGCTRL_IMG_DATA_DISABLE },
  { AES2501_REG_STRTCOL, 0x10 },
  { AES2501_REG_ENDCOL, 0x1f },
  { AES2501_REG_CHANGAIN, AES2501_CHANGAIN_STAGE1_16X },
  { AES2501_REG_ADREFHI, 0x70 },
  { AES2501_REG_ADREFLO, 0x20 },
  { AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
};

static struct aes_regwrite strip_scan_reqs[] = {
  { AES2501_REG_IMAGCTRL,
    AES2501_IMAGCTRL_TST_REG_ENABLE | AES2501_IMAGCTRL_HISTO_DATA_ENABLE },
  { AES2501_REG_STRTCOL, 0x00 },
  { AES2501_REG_ENDCOL, 0x2f },
  { AES2501_REG_CHANGAIN, AES2501_CHANGAIN_STAGE1_16X },
  { AES2501_REG_ADREFHI, AES2501_ADREFHI_MAX_VALUE },
  { AES2501_REG_ADREFLO, 0x20 },
  { AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
};

/* capture SM movement:
 * write reqs and read data 1 + 2,
 * request and read strip,
 * jump back to request UNLESS there's no finger, in which case exit SM,
 * report lack of finger presence, and move to finger detection */

enum capture_states {
  CAPTURE_WRITE_REQS_1,
  CAPTURE_READ_DATA_1,
  CAPTURE_WRITE_REQS_2,
  CAPTURE_READ_DATA_2,
  CAPTURE_REQUEST_STRIP,
  CAPTURE_READ_STRIP,
  CAPTURE_NUM_STATES,
};

static void
capture_read_strip_cb (FpiUsbTransfer *transfer, FpDevice *_dev,
                       gpointer user_data, GError *error)
{
  FpiSsm *ssm = transfer->ssm;
  unsigned char *stripdata;
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (_dev);
  unsigned char *data = transfer->buffer;
  int sum;
  int threshold;

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
      return;
    }

  threshold = regval_from_dump (data + 1 + 192 * 8 + 1 + 16 * 2 + 1 + 8,
                                AES2501_REG_DATFMT);
  if (threshold < 0)
    {
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
      return;
    }

  sum = sum_histogram_values (data + 1 + 192 * 8, threshold & 0x0f);
  if (sum < 0)
    {
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
      return;
    }
  fp_dbg ("sum=%d", sum);

  if (sum < AES2501_SUM_LOW_THRESH)
    {
      strip_scan_reqs[4].value -= 0x8;
      if (strip_scan_reqs[4].value < AES2501_ADREFHI_MIN_VALUE)
        strip_scan_reqs[4].value = AES2501_ADREFHI_MIN_VALUE;
    }
  else if (sum > AES2501_SUM_HIGH_THRESH)
    {
      strip_scan_reqs[4].value += 0x8;
      if (strip_scan_reqs[4].value > AES2501_ADREFHI_MAX_VALUE)
        strip_scan_reqs[4].value = AES2501_ADREFHI_MAX_VALUE;
    }
  fp_dbg ("ADREFHI is %.2x", strip_scan_reqs[4].value);

  /* Sum is 0, maybe finger was removed? Wait for 3 empty frames
   * to ensure
   */
  if (sum == 0)
    {
      self->no_finger_cnt++;
      if (self->no_finger_cnt == 3)
        {
          FpImage *img;

          self->strips = g_slist_reverse (self->strips);
          fpi_do_movement_estimation (&assembling_ctx, self->strips);
          img = fpi_assemble_frames (&assembling_ctx,
                                     self->strips);
          img->flags |= FPI_IMAGE_PARTIAL;
          g_slist_free_full (self->strips, g_free);
          self->strips = NULL;
          self->strips_len = 0;
          fpi_image_device_image_captured (dev, img);
          fpi_image_device_report_finger_status (dev, FALSE);
          /* marking machine complete will re-trigger finger detection loop */
          fpi_ssm_mark_completed (ssm);
        }
      else
        {
          fpi_ssm_jump_to_state (ssm, CAPTURE_REQUEST_STRIP);
        }
    }
  else
    {
      /* obtain next strip */
      /* FIXME: would preallocating strip buffers be a decent optimization? */
      struct fpi_frame *stripe = g_malloc (FRAME_WIDTH * FRAME_HEIGHT / 2 + sizeof (struct fpi_frame));
      stripe->delta_x = 0;
      stripe->delta_y = 0;
      stripdata = stripe->data;
      memcpy (stripdata, data + 1, 192 * 8);
      self->no_finger_cnt = 0;
      self->strips = g_slist_prepend (self->strips, stripe);
      self->strips_len++;

      fpi_ssm_jump_to_state (ssm, CAPTURE_REQUEST_STRIP);
    }
}

static void
capture_run_state (FpiSsm *ssm, FpDevice *device)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (device);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case CAPTURE_WRITE_REQS_1:
      aes_write_regv (dev, capture_reqs_1, G_N_ELEMENTS (capture_reqs_1),
                      generic_write_regv_cb, ssm);
      break;

    case CAPTURE_READ_DATA_1:
      generic_read_ignore_data (ssm, device, READ_REGS_RESP_LEN);
      break;

    case CAPTURE_WRITE_REQS_2:
      aes_write_regv (dev, capture_reqs_2, G_N_ELEMENTS (capture_reqs_2),
                      generic_write_regv_cb, ssm);
      break;

    case CAPTURE_READ_DATA_2:
      generic_read_ignore_data (ssm, device, READ_REGS_RESP_LEN);
      break;

    case CAPTURE_REQUEST_STRIP:
      if (self->deactivating)
        fpi_ssm_mark_completed (ssm);
      else
        aes_write_regv (dev, strip_scan_reqs, G_N_ELEMENTS (strip_scan_reqs),
                        generic_write_regv_cb, ssm);
      break;

    case CAPTURE_READ_STRIP: {
        FpiUsbTransfer *transfer;

        transfer = fpi_usb_transfer_new (device);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_fill_bulk (transfer, EP_IN, STRIP_CAPTURE_LEN);
        fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                                 capture_read_strip_cb, NULL);
        break;
      }
    }
  ;
}

static void
capture_sm_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (_dev);

  G_DEBUG_HERE ();

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
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);
  FpiSsm *ssm;

  if (self->deactivating)
    {
      complete_deactivation (dev);
      return;
    }

  self->no_finger_cnt = 0;
  /* Reset gain */
  strip_scan_reqs[4].value = AES2501_ADREFHI_MAX_VALUE;
  ssm = fpi_ssm_new (FP_DEVICE (dev), capture_run_state,
                     CAPTURE_NUM_STATES);
  G_DEBUG_HERE ();
  fpi_ssm_start (ssm, capture_sm_complete);
}

/****** INITIALIZATION/DEINITIALIZATION ******/

static const struct aes_regwrite init_1[] = {
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { 0, 0 },
  { 0xb0, 0x27 },       /* Reserved? */
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { 0xff, 0x00 },       /* Reserved? */
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_DETCTRL,
    AES2501_DETCTRL_DRATE_CONTINUOUS | AES2501_DETCTRL_SDELAY_31_MS },
  { AES2501_REG_COLSCAN, AES2501_COLSCAN_SRATE_128_US },
  { AES2501_REG_MEASDRV,
    AES2501_MEASDRV_MDRIVE_0_325 | AES2501_MEASDRV_MEASURE_SQUARE },
  { AES2501_REG_MEASFREQ, AES2501_MEASFREQ_2M },
  { AES2501_REG_DEMODPHASE1, DEMODPHASE_NONE },
  { AES2501_REG_DEMODPHASE2, DEMODPHASE_NONE },
  { AES2501_REG_CHANGAIN,
    AES2501_CHANGAIN_STAGE2_4X | AES2501_CHANGAIN_STAGE1_16X },
  { AES2501_REG_ADREFHI, 0x44 },
  { AES2501_REG_ADREFLO, 0x34 },
  { AES2501_REG_STRTCOL, 0x16 },
  { AES2501_REG_ENDCOL, 0x16 },
  { AES2501_REG_DATFMT, AES2501_DATFMT_BIN_IMG | 0x08 },
  { AES2501_REG_TREG1, 0x70 },
  { 0xa2, 0x02 },
  { 0xa7, 0x00 },
  { AES2501_REG_TREGC, AES2501_TREGC_ENABLE },
  { AES2501_REG_TREGD, 0x1a },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
  { AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
  { AES2501_REG_LPONT, AES2501_LPONT_MIN_VALUE },
};

static const struct aes_regwrite init_2[] = {
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_AUTOCALOFFSET, 0x41 },
  { AES2501_REG_EXCITCTRL, 0x42 },
  { AES2501_REG_DETCTRL, 0x53 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
};

static const struct aes_regwrite init_3[] = {
  { 0xff, 0x00 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_AUTOCALOFFSET, 0x41 },
  { AES2501_REG_EXCITCTRL, 0x42 },
  { AES2501_REG_DETCTRL, 0x53 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
};

static const struct aes_regwrite init_4[] = {
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { 0xb0, 0x27 },
  { AES2501_REG_ENDROW, 0x0a },
  { AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
  { AES2501_REG_DETCTRL, 0x45 },
  { AES2501_REG_AUTOCALOFFSET, 0x41 },
};

static const struct aes_regwrite init_5[] = {
  { 0xb0, 0x27 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { 0xff, 0x00 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
  { AES2501_REG_EXCITCTRL, 0x40 },
  { AES2501_REG_CTRL1, AES2501_CTRL1_SCAN_RESET },
  { AES2501_REG_CTRL1, AES2501_CTRL1_SCAN_RESET },
};

enum activate_states {
  WRITE_INIT_1,
  READ_DATA_1,
  WRITE_INIT_2,
  READ_REGS,
  WRITE_INIT_3,
  WRITE_INIT_4,
  WRITE_INIT_5,
  ACTIVATE_NUM_STATES,
};

static void
activate_read_regs_cb (FpImageDevice *dev, GError *error,
                       unsigned char *regs, void *user_data)
{
  FpiSsm *ssm = user_data;
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
    }
  else
    {
      fp_dbg ("reg 0xaf = %x", regs[0x5f]);
      if (regs[0x5f] != 0x6b || ++self->read_regs_retry_count == 13)
        fpi_ssm_jump_to_state (ssm, WRITE_INIT_4);
      else
        fpi_ssm_next_state (ssm);
    }
}

static void
activate_init3_cb (FpImageDevice *dev, GError *error,
                   void *user_data)
{
  FpiSsm *ssm = user_data;

  if (!error)
    fpi_ssm_jump_to_state (ssm, READ_REGS);
  else
    fpi_ssm_mark_failed (ssm, error);
}

static void
activate_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);

  /* This state machine isn't as linear as it may appear. After doing init1
   * and init2 register configuration writes, we have to poll a register
   * waiting for a specific value. READ_REGS checks the register value, and
   * if we're ready to move on, we jump to init4. Otherwise, we write init3
   * and then jump back to READ_REGS. In a synchronous model:

     [...]
     aes_write_regv(init_2);
     read_regs(into buffer);
     i = 0;
     while (buffer[0x5f] == 0x6b) {
         aes_write_regv(init_3);
             read_regs(into buffer);
         if (++i == 13)
             break;
     }
     aes_write_regv(init_4);
   */

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case WRITE_INIT_1:
      aes_write_regv (dev, init_1, G_N_ELEMENTS (init_1),
                      generic_write_regv_cb, ssm);
      break;

    case READ_DATA_1:
      fp_dbg ("read data 1");
      generic_read_ignore_data (ssm, _dev, FINGER_DETECTION_LEN);
      break;

    case WRITE_INIT_2:
      aes_write_regv (dev, init_2, G_N_ELEMENTS (init_2),
                      generic_write_regv_cb, ssm);
      break;

    case READ_REGS:
      read_regs (dev, activate_read_regs_cb, ssm);
      break;

    case WRITE_INIT_3:
      aes_write_regv (dev, init_3, G_N_ELEMENTS (init_3),
                      activate_init3_cb, ssm);
      break;

    case WRITE_INIT_4:
      aes_write_regv (dev, init_4, G_N_ELEMENTS (init_4),
                      generic_write_regv_cb, ssm);
      break;

    case WRITE_INIT_5:
      aes_write_regv (dev, init_5, G_N_ELEMENTS (init_5),
                      generic_write_regv_cb, ssm);
      break;
    }
}

static void
activate_sm_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  fpi_image_device_activate_complete (FP_IMAGE_DEVICE (dev), error);

  if (!error)
    start_finger_detection (FP_IMAGE_DEVICE (dev));
}

static void
dev_activate (FpImageDevice *dev)
{
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (dev), activate_run_state,
                             ACTIVATE_NUM_STATES);

  self->read_regs_retry_count = 0;
  fpi_ssm_start (ssm, activate_sm_complete);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);

  /* FIXME: audit cancellation points, probably need more, specifically
   * in error handling paths? */
  self->deactivating = TRUE;
}

static void
complete_deactivation (FpImageDevice *dev)
{
  FpiDeviceAes2501 *self = FPI_DEVICE_AES2501 (dev);

  G_DEBUG_HERE ();

  /* FIXME: if we're in the middle of a scan, we should cancel the scan.
   * maybe we can do this with a master reset, unconditionally? */

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

  /* FIXME check endpoints */

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
  { .vid = 0x08ff,  .pid = 0x2500, },/* AES2500 */
  { .vid = 0x08ff,  .pid = 0x2580, },/* AES2501 */
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_aes2501_init (FpiDeviceAes2501 *self)
{
}
static void
fpi_device_aes2501_class_init (FpiDeviceAes2501Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "aes2501";
  dev_class->full_name = "AuthenTec AES2501";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->img_width = IMAGE_WIDTH;
  img_class->img_height = -1;
}
