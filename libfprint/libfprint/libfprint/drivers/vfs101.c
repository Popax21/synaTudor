/*
 * Validity VFS101 driver for libfprint
 * Copyright (C) 2011 Sergio Cerlesi <sergio.cerlesi@gmail.com>
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

#define FP_COMPONENT "vfs101"

#include "drivers_api.h"

/* Input-Output usb endpoint */
#define EP_IN(n)        (n | FPI_USB_ENDPOINT_IN)
#define EP_OUT(n)       (n | FPI_USB_ENDPOINT_OUT)

/* Usb bulk timeout */
#define BULK_TIMEOUT 100

/* The device send back the image into block of 16 frames of 292 bytes */
#define VFS_FRAME_SIZE 292
#define VFS_BLOCK_SIZE 16 * VFS_FRAME_SIZE

/* Buffer height */
#define VFS_BUFFER_HEIGHT 5000

/* Buffer size */
#define VFS_BUFFER_SIZE (VFS_BUFFER_HEIGHT * VFS_FRAME_SIZE)

/* Image width */
#define VFS_IMG_WIDTH 200

/* Maximum image height */
#define VFS_IMG_MAX_HEIGHT 1023

/* Minimum image height */
#define VFS_IMG_MIN_HEIGHT 200

/* Scan level threshold */
#define VFS_IMG_SLT_BEGIN 768
#define VFS_IMG_SLT_END 64
#define VFS_IMG_SLT_LINES 4

/* Minimum image level */
#define VFS_IMG_MIN_IMAGE_LEVEL 144

/* Best image contrast */
#define VFS_IMG_BEST_CONTRAST 128

/* Device parameters address */
#define VFS_PAR_000E 0x000e
#define VFS_PAR_0011 0x0011
#define VFS_PAR_THRESHOLD 0x0057
#define VFS_PAR_STATE_3 0x005e
#define VFS_PAR_STATE_5 0x005f
#define VFS_PAR_INFO_RATE 0x0062
#define VFS_PAR_0076 0x0076
#define VFS_PAR_INFO_CONTRAST 0x0077
#define VFS_PAR_0078 0x0078

/* Device regiones address */
#define VFS_REG_IMG_EXPOSURE 0xff500e
#define VFS_REG_IMG_CONTRAST 0xff5038

/* Device settings */
#define VFS_VAL_000E 0x0001
#define VFS_VAL_0011 0x0008
#define VFS_VAL_THRESHOLD 0x0096
#define VFS_VAL_STATE_3 0x0064
#define VFS_VAL_STATE_5 0x00c8
#define VFS_VAL_INFO_RATE 0x0001
#define VFS_VAL_0076 0x0012
#define VFS_VAL_0078 0x2230
#define VFS_VAL_IMG_EXPOSURE 0x21c0

/* Structure for Validity device */
struct _FpDeviceVfs101
{
  FpImageDevice parent;

  /* Action state */
  gboolean active;
  gboolean deactivate;

  /* Sequential number */
  unsigned int seqnum;

  /* Buffer for input/output */
  unsigned char *buffer;

  /* Length of data to send or received */
  unsigned int length;

  /* Ignore usb error */
  int ignore_error;

  /* Loop counter */
  int counter;

  /* Image contrast */
  int contrast;

  /* Best contrast */
  int best_contrast;

  /* Best contrast level */
  int best_clevel;

  /* Bottom line of image */
  int bottom;

  /* Image height */
  int height;
};
G_DECLARE_FINAL_TYPE (FpDeviceVfs101, fpi_device_vfs101, FPI, DEVICE_VFS101,
                      FpImageDevice);
G_DEFINE_TYPE (FpDeviceVfs101, fpi_device_vfs101, FP_TYPE_IMAGE_DEVICE);

/* Return byte at specified position */
static inline unsigned char
byte (int position, int value)
{
  return (value >> (position * 8)) & 0xff;
}

/* Return sequential number */
static inline unsigned short
get_seqnum (int h, int l)
{
  return (h << 8) | l;
}

/* Check sequential number */
static inline int
check_seqnum (FpDeviceVfs101 *vdev)
{
  if ((byte (0, vdev->seqnum) == vdev->buffer[0]) &&
      (byte (1, vdev->seqnum) == vdev->buffer[1]))
    return 0;
  else
    return 1;
}

/* Internal result codes */
enum {
  RESULT_RETRY,
  RESULT_RETRY_SHORT,
  RESULT_RETRY_REMOVE,
  RESULT_COUNT,
};

/* Dump buffer for debug */
#define dump_buffer(buf) \
  fp_dbg ("%02x %02x %02x %02x %02x %02x %02x %02x", \
          buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13] \
         )

/* Callback of asynchronous send */
static void
async_send_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  /* Skip error check if ignore_error is set */
  if (error)
    {
      if (!self->ignore_error)
        {
          fpi_ssm_mark_failed (transfer->ssm, error);
          return;
        }
      else
        {
          g_error_free (error);
          fp_dbg ("Ignoring send error: %s", error->message);
        }
    }
  /* Reset ignore_error flag */
  self->ignore_error = FALSE;

  /* Dump buffer for debug */
  dump_buffer (self->buffer);

  fpi_ssm_next_state (transfer->ssm);
}

/* Submit asynchronous send */
static void
async_send (FpiSsm        *ssm,
            FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpiUsbTransfer *transfer;

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  /* Put sequential number into the buffer */
  self->seqnum++;
  self->buffer[0] = byte (0, self->seqnum);
  self->buffer[1] = byte (1, self->seqnum);

  /* Prepare bulk transfer */
  fpi_usb_transfer_fill_bulk_full (transfer, EP_OUT (1),
                                   self->buffer, self->length, NULL);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           async_send_cb, NULL);
}

/* Callback of asynchronous recv */
static void
async_recv_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  /* Skip error check if ignore_error is set */
  if (!self->ignore_error)
    {
      if (error)
        {
          /* Transfer not completed, return IO error */
          fpi_ssm_mark_failed (transfer->ssm, error);
          return;
        }

      if (check_seqnum (self))
        {
          /* Sequential number received mismatch, return protocol error */
          fp_err ("seqnum mismatch, got %04x, expected %04x",
                  get_seqnum (self->buffer[1], self->buffer[0]),
                  self->seqnum);
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          return;
        }
    }

  g_clear_pointer (&error, g_error_free);

  /* Reset ignore_error flag */
  self->ignore_error = FALSE;

  /* Dump buffer for debug */
  dump_buffer (self->buffer);

  /* Set length of received data */
  self->length = transfer->actual_length;

  fpi_ssm_next_state (transfer->ssm);
}

/* Submit asynchronous recv */
static void
async_recv (FpiSsm        *ssm,
            FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpiUsbTransfer *transfer;

  /* Allocation of transfer */
  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  /* Prepare bulk transfer */
  fpi_usb_transfer_fill_bulk_full (transfer, EP_IN (1), self->buffer,
                                   0x0f, NULL);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           async_recv_cb, NULL);
}

static void async_load (FpiSsm        *ssm,
                        FpImageDevice *dev);

/* Callback of asynchronous load */
static void
async_load_cb (FpiUsbTransfer *transfer, FpDevice *device,
               gpointer user_data, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (device);
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  /* Skip error check if ignore_error is set */
  if (!self->ignore_error)
    {
      if (error)
        {
          /* Transfer not completed */
          fpi_ssm_mark_failed (transfer->ssm, error);
          return;
        }

      if (transfer->actual_length % VFS_FRAME_SIZE)
        {
          /* Received incomplete frame, return protocol error */
          fp_err ("received incomplete frame");
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          return;
        }
    }

  /* Any error has been ignored. */
  g_clear_pointer (&error, g_error_free);

  /* Increase image length */
  self->length += transfer->actual_length;

  if (transfer->actual_length == VFS_BLOCK_SIZE)
    {
      if ((VFS_BUFFER_SIZE - self->length) < VFS_BLOCK_SIZE)
        {
          /* Buffer full, image too large, return no memory error */
          fp_err ("buffer full, image too large");
          fpi_ssm_mark_failed (transfer->ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          return;
        }
      else
        {
          /* Image load not completed, submit another asynchronous load */
          async_load (transfer->ssm, dev);
        }
    }
  else
    {
      /* Reset ignore_error flag */
      self->ignore_error = FALSE;

      /* Image load completed, go to next state */
      self->height = self->length / VFS_FRAME_SIZE;
      fp_dbg ("image loaded, height = %d", self->height);
      fpi_ssm_next_state (transfer->ssm);
    }
}

/* Submit asynchronous load */
static void
async_load (FpiSsm        *ssm,
            FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpiUsbTransfer *transfer;
  unsigned char *buffer;

  /* Allocation of transfer */
  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  /* Append new data into the buffer */
  buffer = self->buffer + self->length;

  /* Prepare bulk transfer */
  fpi_usb_transfer_fill_bulk_full (transfer, EP_IN (2), buffer,
                                   VFS_BLOCK_SIZE, NULL);
  transfer->ssm = ssm;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           async_load_cb, NULL);
}

/* Swap ssm states */
enum {
  M_SWAP_SEND,
  M_SWAP_RECV,
  M_SWAP_NUM_STATES,
};

/* Exec swap sequential state machine */
static void
m_swap_state (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case M_SWAP_SEND:
      /* Send data */
      async_send (ssm, FP_IMAGE_DEVICE (dev));
      break;

    case M_SWAP_RECV:
      /* Recv response */
      async_recv (ssm, FP_IMAGE_DEVICE (dev));
      break;
    }
}

/* Start swap sequential state machine */
static void
m_swap (FpiSsm        *ssm,
        FpImageDevice *dev,
        unsigned char *data,
        size_t         length)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpiSsm *subsm;

  /* Prepare data for sending */
  memcpy (self->buffer, data, length);
  memset (self->buffer + length, 0, 16 - length);
  self->length = length;

  /* Start swap ssm */
  subsm = fpi_ssm_new (FP_DEVICE (dev), m_swap_state, M_SWAP_NUM_STATES);
  fpi_ssm_start_subsm (ssm, subsm);
}

/* Retrieve fingerprint image */
static void
vfs_get_print (FpiSsm        *ssm,
               FpImageDevice *dev,
               unsigned int   param,
               int            type)
{
  unsigned char data[2][0x0e] = {
    {       0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01        },
    {       0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
            0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01        }
  };

  fp_dbg ("param = %04x, type = %d", param, type);

  /* Prepare data for sending */
  data[type][6] = byte (0, param);
  data[type][7] = byte (1, param);

  /* Run swap sequential state machine */
  m_swap (ssm, dev, data[type], 0x0e);
}

/* Set a parameter value on the device */
static void
vfs_set_param (FpiSsm        *ssm,
               FpImageDevice *dev,
               unsigned int   param,
               unsigned int   value)
{
  unsigned char data[0x0a] = { 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

  fp_dbg ("param = %04x, value = %04x", param, value);

  /* Prepare data for sending */
  data[6] = byte (0, param);
  data[7] = byte (1, param);
  data[8] = byte (0, value);
  data[9] = byte (1, value);

  /* Run swap sequential state machine */
  m_swap (ssm, dev, data, 0x0a);
}

/* Abort previous print */
static void
vfs_abort_print (FpiSsm        *ssm,
                 FpImageDevice *dev)
{
  unsigned char data[0x06] = { 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00 };

  G_DEBUG_HERE ();

  /* Run swap sequential state machine */
  m_swap (ssm, dev, data, 0x06);
}

/* Poke a value on a region */
static void
vfs_poke (FpiSsm        *ssm,
          FpImageDevice *dev,
          unsigned int   addr,
          unsigned int   value,
          unsigned int   size)
{
  unsigned char data[0x0f] = { 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  fp_dbg ("addr = %04x, value = %04x", addr, value);

  /* Prepare data for sending */
  data[6] = byte (0, addr);
  data[7] = byte (1, addr);
  data[8] = byte (2, addr);
  data[9] = byte (3, addr);
  data[10] = byte (0, value);
  data[11] = byte (1, value);
  data[12] = byte (2, value);
  data[13] = byte (3, value);
  data[14] = byte (0, size);

  /* Run swap sequential state machine */
  m_swap (ssm, dev, data, 0x0f);
}

/* Get current finger state */
static void
vfs_get_finger_state (FpiSsm        *ssm,
                      FpImageDevice *dev)
{
  unsigned char data[0x06] = { 0x00, 0x00, 0x00, 0x00, 0x16, 0x00 };

  G_DEBUG_HERE ();

  /* Run swap sequential state machine */
  m_swap (ssm, dev, data, 0x06);
}

/* Load raw image from reader */
static void
vfs_img_load (FpiSsm        *ssm,
              FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  G_DEBUG_HERE ();

  /* Reset buffer length */
  self->length = 0;

  /* Reset image properties */
  self->bottom = 0;
  self->height = -1;

  /* Asynchronous load */
  async_load (ssm, dev);
}

#define offset(x, y)    ((x) + ((y) * VFS_FRAME_SIZE))

/* Screen image to remove noise and find bottom line and height of image */
static void
img_screen (FpDeviceVfs101 *vdev)
{
  int y, x, count, top;
  long int level;
  int last_line = vdev->height - 1;

  fp_dbg ("image height before screen = %d", vdev->height);

  count = 0;

  /* Image returned from sensor can contain many empty lines,
   * for remove these lines compare byte 282-283 (scan level information)
   * with two different thresholds, one for the begin of finger image and
   * one for the end. To increase stability of the code use a counter
   * of lines that satisfy the threshold.
   */
  for (y = last_line, top = last_line; y >= 0; y--)
    {
      /* Take image scan level */
      level = vdev->buffer[offset (283, y)] * 256 +
              vdev->buffer[offset (282, y)];

      fp_dbg ("line = %d, scan level = %ld", y, level);

      if (level >= VFS_IMG_SLT_BEGIN && top == last_line)
        {
          /* Begin threshold satisfied */
          if (count < VFS_IMG_SLT_LINES)
            {
              /* Increase count */
              count++;
            }
          else
            {
              /* Found top fingerprint line */
              top = y + VFS_IMG_SLT_LINES;
              count = 0;
            }
        }
      else if ((level < VFS_IMG_SLT_END || level >= 65535) &&
               top != last_line)
        {
          /* End threshold satisfied */
          if (count < VFS_IMG_SLT_LINES)
            {
              /* Increase count */
              count++;
            }
          else
            {
              /* Found bottom fingerprint line */
              vdev->bottom = y + VFS_IMG_SLT_LINES + 1;
              break;
            }
        }
      else
        {
          /* Not threshold satisfied, reset count */
          count = 0;
        }
    }

  vdev->height = top - vdev->bottom + 1;

  /* Check max height */
  if (vdev->height > VFS_IMG_MAX_HEIGHT)
    vdev->height = VFS_IMG_MAX_HEIGHT;

  fp_dbg ("image height after screen = %d", vdev->height);

  /* Scan image and remove noise */
  for (y = vdev->bottom; y <= top; y++)
    for (x = 6; x < VFS_IMG_WIDTH + 6; x++)
      if (vdev->buffer[offset (x, y)] > VFS_IMG_MIN_IMAGE_LEVEL)
        vdev->buffer[offset (x, y)] = 255;
};

/* Copy image from reader buffer and put it into image data */
static void
img_copy (FpDeviceVfs101 *self, FpImage *img)
{
  unsigned int line;
  unsigned char *img_buffer = img->data;
  unsigned char *vdev_buffer = self->buffer + (self->bottom * VFS_FRAME_SIZE) + 6;

  for (line = 0; line < img->height; line++)
    {
      /* Copy image line from reader buffer to image data */
      memcpy (img_buffer, vdev_buffer, VFS_IMG_WIDTH);

      /* Next line of reader buffer */
      vdev_buffer = vdev_buffer + VFS_FRAME_SIZE;

      /* Next line of image buffer */
      img_buffer = img_buffer + VFS_IMG_WIDTH;
    }
}

/* Extract fingerpint image from raw data */
static void
img_extract (FpiSsm        *ssm,
             FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpImage *img;

  /* Screen image to remove noise and find top and bottom line */
  img_screen (self);

  /* Check image height */
  if (self->height < VFS_IMG_MIN_HEIGHT)
    {
      /* Image too short */
      self->height = 0;
      fpi_image_device_retry_scan (dev, FP_DEVICE_RETRY_TOO_SHORT);
      return;
    }

  /* Create new image */
  img = fp_image_new (self->height, VFS_IMG_WIDTH);
  img->width = VFS_IMG_WIDTH;
  img->height = self->height;
  img->flags = FPI_IMAGE_V_FLIPPED;

  /* Copy data into image */
  img_copy (self, img);

  /* Notify image captured */
  fpi_image_device_image_captured (dev, img);
};

/* Finger states */
enum {
  VFS_FINGER_EMPTY,
  VFS_FINGER_PRESENT,
  VFS_FINGER_UNKNOWN,
};

/* Return finger state */
static inline int
vfs_finger_state (FpDeviceVfs101 *vdev)
{
  /* Check finger state */
  switch (vdev->buffer[0x0a])
    {
    case 0x00:
    case 0x01:
      /* Finger is empty */
      return VFS_FINGER_EMPTY;
      break;

    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
      /* Finger is present */
      return VFS_FINGER_PRESENT;
      break;

    default:
      return VFS_FINGER_UNKNOWN;
    }
};

/* Check contrast of image */
static void
vfs_check_contrast (FpDeviceVfs101 *vdev)
{
  int y;
  long int count = 0;

  /* Check difference from byte 4 to byte 5 for verify contrast of image */
  for (y = 0; y < vdev->height; y++)
    count = count + vdev->buffer[offset (5, y)] - vdev->buffer[offset (4, y)];
  count = count / vdev->height;

  if (count < 16)
    {
      /* Contrast not valid, retry */
      vdev->contrast++;
      return;
    }

  fp_dbg ("contrast = %d, level = %ld", vdev->contrast, count);

  if (labs (count - VFS_IMG_BEST_CONTRAST) < abs (vdev->best_clevel - VFS_IMG_BEST_CONTRAST))
    {
      /* Better contrast found, use it */
      vdev->best_contrast = vdev->contrast;
      vdev->best_clevel = count;
    }
}

/* Loop ssm states */
enum {
  /* Step 0 - Scan finger */
  M_LOOP_0_GET_PRINT,
  M_LOOP_0_SLEEP,
  M_LOOP_0_GET_STATE,
  M_LOOP_0_LOAD_IMAGE,
  M_LOOP_0_EXTRACT_IMAGE,
  M_LOOP_0_CHECK_ACTION,

  /* Step 1 - Scan failed */
  M_LOOP_1_GET_STATE,
  M_LOOP_1_CHECK_STATE,
  M_LOOP_1_GET_PRINT,
  M_LOOP_1_LOAD_IMAGE,
  M_LOOP_1_LOOP,
  M_LOOP_1_SLEEP,

  /* Step 2 - Abort print */
  M_LOOP_2_ABORT_PRINT,
  M_LOOP_2_LOAD_IMAGE,

  /* Step 3 - Wait aborting */
  M_LOOP_3_GET_PRINT,
  M_LOOP_3_LOAD_IMAGE,
  M_LOOP_3_CHECK_IMAGE,
  M_LOOP_3_LOOP,

  /* Number of states */
  M_LOOP_NUM_STATES,
};

/* Exec loop sequential state machine */
static void
m_loop_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (_dev);

  /* Complete if deactivation was requested */
  if (self->deactivate)
    {
      fpi_ssm_mark_completed (ssm);
      return;
    }

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case M_LOOP_0_GET_PRINT:
      /* Send get print command to the reader */
      vfs_get_print (ssm, dev, VFS_BUFFER_HEIGHT, 1);
      break;

    case M_LOOP_0_SLEEP:
      /* Wait fingerprint scanning */
      fpi_ssm_next_state_delayed (ssm, 50, NULL);
      break;

    case M_LOOP_0_GET_STATE:
      /* Get finger state */
      vfs_get_finger_state (ssm, dev);
      break;

    case M_LOOP_0_LOAD_IMAGE:
      /* Check finger state */
      switch (vfs_finger_state (self))
        {
        case VFS_FINGER_EMPTY:
          fpi_image_device_report_finger_status (dev, FALSE);

          /* Finger isn't present, loop */
          fpi_ssm_jump_to_state (ssm, M_LOOP_0_SLEEP);
          break;

        case VFS_FINGER_PRESENT:
          fpi_image_device_report_finger_status (dev, TRUE);

          /* Load image from reader */
          self->ignore_error = TRUE;
          vfs_img_load (ssm, dev);
          break;

        default:
          fpi_image_device_report_finger_status (dev, FALSE);

          /* Unknown state */
          fp_err ("unknown device state 0x%02x",
                  self->buffer[0x0a]);
          fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          break;
        }
      break;

    case M_LOOP_0_EXTRACT_IMAGE:
      /* Fingerprint is loaded, extract image from raw data */
      img_extract (ssm, dev);

      /* Wait handling image */
      fpi_ssm_next_state_delayed (ssm, 10, NULL);
      break;

    case M_LOOP_0_CHECK_ACTION:
      /* Action not completed */
      if (self->height > 0)
        /* Continue loop */
        fpi_ssm_jump_to_state (ssm, M_LOOP_2_ABORT_PRINT);
      else
        /* Error found */
        fpi_ssm_next_state (ssm);
      break;

    case M_LOOP_1_GET_STATE:
      /* Get finger state */
      vfs_get_finger_state (ssm, dev);
      break;

    case M_LOOP_1_CHECK_STATE:
      /* Check finger state */
      if (vfs_finger_state (self) == VFS_FINGER_PRESENT)
        {
          fpi_image_device_report_finger_status (dev, TRUE);
          fpi_ssm_next_state_delayed (ssm, 250, NULL);
        }
      else
        {
          /* Finger not present */
          fpi_image_device_report_finger_status (dev, FALSE);

          /* Continue */
          fpi_ssm_jump_to_state (ssm, M_LOOP_1_SLEEP);
        }
      break;

    case M_LOOP_1_GET_PRINT:
      /* Send get print command to the reader */
      vfs_get_print (ssm, dev, VFS_BUFFER_HEIGHT, 1);
      break;

    case M_LOOP_1_LOAD_IMAGE:
      /* Load image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_LOOP_1_LOOP:
      /* Loop */
      fpi_ssm_jump_to_state (ssm, M_LOOP_1_GET_STATE);
      break;

    case M_LOOP_1_SLEEP:
      /* Wait fingerprint scanning */
      fpi_ssm_next_state_delayed (ssm, 10, NULL);
      break;

    case M_LOOP_2_ABORT_PRINT:
      /* Abort print command */
      vfs_abort_print (ssm, dev);
      break;

    case M_LOOP_2_LOAD_IMAGE:
      /* Load abort image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_LOOP_3_GET_PRINT:
      /* Get empty image */
      vfs_get_print (ssm, dev, 0x000a, 0);
      break;

    case M_LOOP_3_LOAD_IMAGE:
      /* Load abort image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_LOOP_3_CHECK_IMAGE:
      if (self->height == 10)
        {
          /* Image load correctly, jump to step 0 */
          self->counter = 0;
          fpi_ssm_jump_to_state (ssm, M_LOOP_0_GET_PRINT);
        }
      else if (self->counter < 10)
        {
          /* Wait aborting */
          self->counter++;
          fpi_ssm_next_state_delayed (ssm, 100, NULL);
        }
      else
        {
          /* reach max loop counter, return protocol error */
          fp_err ("waiting abort reach max loop counter");
          fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
        }
      break;

    case M_LOOP_3_LOOP:
      /* Loop */
      fpi_ssm_jump_to_state (ssm, M_LOOP_3_GET_PRINT);
      break;
    }
}

/* Complete loop sequential state machine */
static void
m_loop_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  /* When the loop completes, we have (successfully) deactivated */
  if (self->active)
    fpi_image_device_deactivate_complete (FP_IMAGE_DEVICE (dev),
                                          error);

  self->active = FALSE;

}

/* Init ssm states */
enum {
  /* Step 0 - Cleanup device buffer */
  M_INIT_0_RECV_DIRTY,
  M_INIT_0_ABORT_PRINT,
  M_INIT_0_LOAD_IMAGE,

  /* Step 1 - Wait aborting */
  M_INIT_1_GET_PRINT,
  M_INIT_1_LOAD_IMAGE,
  M_INIT_1_CHECK_IMAGE,
  M_INIT_1_LOOP,

  /* Step 2 - Handle unexpected finger presence */
  M_INIT_2_GET_STATE,
  M_INIT_2_CHECK_STATE,
  M_INIT_2_GET_PRINT,
  M_INIT_2_LOAD_IMAGE,
  M_INIT_2_LOOP,

  /* Step 3 - Set parameters */
  M_INIT_3_SET_000E,
  M_INIT_3_SET_0011,
  M_INIT_3_SET_0076,
  M_INIT_3_SET_0078,
  M_INIT_3_SET_THRESHOLD,
  M_INIT_3_SET_STATE3_COUNT,
  M_INIT_3_SET_STATE5_COUNT,
  M_INIT_3_SET_INFO_CONTRAST,
  M_INIT_3_SET_INFO_RATE,

  /* Step 4 - Autocalibrate contrast */
  M_INIT_4_SET_EXPOSURE,
  M_INIT_4_SET_CONTRAST,
  M_INIT_4_GET_PRINT,
  M_INIT_4_LOAD_IMAGE,
  M_INIT_4_CHECK_CONTRAST,

  /* Step 5 - Set info line parameters */
  M_INIT_5_SET_EXPOSURE,
  M_INIT_5_SET_CONTRAST,
  M_INIT_5_SET_INFO_CONTRAST,
  M_INIT_5_SET_INFO_RATE,

  /* Number of states */
  M_INIT_NUM_STATES,
};

/* Exec init sequential state machine */
static void
m_init_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (_dev);

  /* Mark as cancelled when activation collides with deactivation. */
  if (self->deactivate)
    {
      fpi_ssm_mark_failed (ssm,
                           g_error_new (G_IO_ERROR,
                                        G_IO_ERROR_CANCELLED,
                                        "Initialisation was cancelled"));
      return;
    }

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case M_INIT_0_RECV_DIRTY:
      /* Recv eventually dirty data */
      self->ignore_error = TRUE;
      async_recv (ssm, dev);
      break;

    case M_INIT_0_ABORT_PRINT:
      /* Abort print command */
      vfs_abort_print (ssm, dev);
      break;

    case M_INIT_0_LOAD_IMAGE:
      /* Load abort image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_INIT_1_GET_PRINT:
      /* Get empty image */
      vfs_get_print (ssm, dev, 0x000a, 0);
      break;

    case M_INIT_1_LOAD_IMAGE:
      /* Load abort image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_INIT_1_CHECK_IMAGE:
      if (self->height == 10)
        {
          /* Image load correctly, jump to step 2 */
          self->counter = 0;
          fpi_ssm_jump_to_state (ssm, M_INIT_2_GET_STATE);
        }
      else if (self->counter < 10)
        {
          /* Wait aborting */
          self->counter++;
          fpi_ssm_next_state_delayed (ssm, 100, NULL);
        }
      else
        {
          /* reach max loop counter, return protocol error */
          fp_err ("waiting abort reach max loop counter");
          fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
        }
      break;

    case M_INIT_1_LOOP:
      /* Loop */
      fpi_ssm_jump_to_state (ssm, M_INIT_1_GET_PRINT);
      break;

    case M_INIT_2_GET_STATE:
      /* Get finger state */
      vfs_get_finger_state (ssm, dev);
      break;

    case M_INIT_2_CHECK_STATE:
      /* Check finger state */
      if (vfs_finger_state (self) == VFS_FINGER_PRESENT)
        {
          /* Wait a bit for finger removal; if it doesn't happen, prompt */
          if (self->counter < 2)
            {
              /* Wait removing finger */
              self->counter++;
              fpi_ssm_next_state_delayed (ssm, 250, NULL);
            }
          else
            {
              /* The user should remove their finger from the scanner */
              fp_warn ("unexpected finger find, remove finger from the scanner");
              fpi_ssm_mark_failed (ssm, fpi_device_retry_new (FP_DEVICE_RETRY_REMOVE_FINGER));
            }
        }
      else
        {
          /* Finger not present */
          if (self->counter == 0)
            {
              /* Continue */
              fpi_ssm_jump_to_state (ssm, M_INIT_3_SET_000E);
            }
          else
            {
              /* Finger removed, jump to abort */
              self->counter = 0;
              fpi_ssm_jump_to_state (ssm, M_INIT_0_ABORT_PRINT);
            }
        }
      break;

    case M_INIT_2_GET_PRINT:
      /* Send get print command to the reader */
      vfs_get_print (ssm, dev, VFS_BUFFER_HEIGHT, 1);
      break;

    case M_INIT_2_LOAD_IMAGE:
      /* Load unexpected image */
      self->ignore_error = TRUE;
      vfs_img_load (ssm, dev);
      break;

    case M_INIT_2_LOOP:
      /* Loop */
      fpi_ssm_jump_to_state (ssm, M_INIT_2_GET_STATE);
      break;

    case M_INIT_3_SET_000E:
      /* Set param 0x000e, required for take image */
      vfs_set_param (ssm, dev, VFS_PAR_000E, VFS_VAL_000E);
      break;

    case M_INIT_3_SET_0011:
      /* Set param 0x0011, required for take image */
      vfs_set_param (ssm, dev, VFS_PAR_0011, VFS_VAL_0011);
      break;

    case M_INIT_3_SET_0076:
      /* Set param 0x0076, required for use info line */
      vfs_set_param (ssm, dev, VFS_PAR_0076, VFS_VAL_0076);
      break;

    case M_INIT_3_SET_0078:
      /* Set param 0x0078, required for use info line */
      vfs_set_param (ssm, dev, VFS_PAR_0078, VFS_VAL_0078);
      break;

    case M_INIT_3_SET_THRESHOLD:
      /* Set threshold */
      vfs_set_param (ssm, dev, VFS_PAR_THRESHOLD, VFS_VAL_THRESHOLD);
      break;

    case M_INIT_3_SET_STATE3_COUNT:
      /* Set state 3 count */
      vfs_set_param (ssm, dev, VFS_PAR_STATE_3, VFS_VAL_STATE_3);
      break;

    case M_INIT_3_SET_STATE5_COUNT:
      /* Set state 5 count */
      vfs_set_param (ssm, dev, VFS_PAR_STATE_5, VFS_VAL_STATE_5);
      break;

    case M_INIT_3_SET_INFO_CONTRAST:
      /* Set info line contrast */
      vfs_set_param (ssm, dev, VFS_PAR_INFO_CONTRAST, 10);
      break;

    case M_INIT_3_SET_INFO_RATE:
      /* Set info line rate */
      vfs_set_param (ssm, dev, VFS_PAR_INFO_RATE, 32);
      break;

    case M_INIT_4_SET_EXPOSURE:
      /* Set exposure level of reader */
      vfs_poke (ssm, dev, VFS_REG_IMG_EXPOSURE, 0x4000, 0x02);
      self->counter = 1;
      break;

    case M_INIT_4_SET_CONTRAST:
      /* Set contrast level of reader */
      vfs_poke (ssm, dev, VFS_REG_IMG_CONTRAST, self->contrast, 0x01);
      break;

    case M_INIT_4_GET_PRINT:
      /* Get empty image */
      vfs_get_print (ssm, dev, 0x000a, 0);
      break;

    case M_INIT_4_LOAD_IMAGE:
      /* Load empty image */
      vfs_img_load (ssm, dev);
      break;

    case M_INIT_4_CHECK_CONTRAST:
      /* Check contrast */
      vfs_check_contrast (self);

      if (self->contrast <= 6 || self->counter >= 12)
        {
          /* End contrast scan, continue */
          self->contrast = self->best_contrast;
          self->counter = 0;
          fp_dbg ("use contrast value = %d", self->contrast);
          fpi_ssm_next_state (ssm);
        }
      else
        {
          /* Continue contrast scan, loop */
          self->contrast--;
          self->counter++;
          fpi_ssm_jump_to_state (ssm, M_INIT_4_SET_CONTRAST);
        }
      break;

    case M_INIT_5_SET_EXPOSURE:
      /* Set exposure level of reader */
      vfs_poke (ssm, dev, VFS_REG_IMG_EXPOSURE, VFS_VAL_IMG_EXPOSURE, 0x02);
      break;

    case M_INIT_5_SET_CONTRAST:
      /* Set contrast level of reader */
      vfs_poke (ssm, dev, VFS_REG_IMG_CONTRAST, self->contrast, 0x01);
      break;

    case M_INIT_5_SET_INFO_CONTRAST:
      /* Set info line contrast */
      vfs_set_param (ssm, dev, VFS_PAR_INFO_CONTRAST, self->contrast);
      break;

    case M_INIT_5_SET_INFO_RATE:
      /* Set info line rate */
      vfs_set_param (ssm, dev, VFS_PAR_INFO_RATE, VFS_VAL_INFO_RATE);
      break;
    }
}

/* Complete init sequential state machine */
static void
m_init_complete (FpiSsm *ssm, FpDevice *_dev, GError *error)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);

  /* Notify activate complete */
  fpi_image_device_activate_complete (dev, error);

  if (!error)
    {
      FpiSsm *ssm_loop;

      /* Start loop ssm */
      ssm_loop = fpi_ssm_new (FP_DEVICE (dev), m_loop_state, M_LOOP_NUM_STATES);
      fpi_ssm_start (ssm_loop, m_loop_complete);
    }

  /* Free sequential state machine */
}

/* Activate device */
static void
dev_activate (FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  FpiSsm *ssm;

  /* Check if already active */
  g_assert (!self->active);

  /* Set active state */
  self->active = TRUE;
  self->deactivate = FALSE;

  /* Set contrast */
  self->contrast = 15;
  self->best_clevel = -1;

  /* Reset loop counter */
  self->counter = 0;

  /* Start init ssm */
  ssm = fpi_ssm_new (FP_DEVICE (dev), m_init_state, M_INIT_NUM_STATES);
  fpi_ssm_start (ssm, m_init_complete);
}

/* Deactivate device */
static void
dev_deactivate (FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);

  /* Device already deactivated, likely due to an error */
  if (!self->active)
    {
      fpi_image_device_deactivate_complete (dev, NULL);
      return;
    }

  /* Signal deactivation, deactivation will happen from the SSM
   * completion handler. */
  self->deactivate = TRUE;
}

/* Open device */
static void
dev_open (FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  GError *error = NULL;

  /* Claim usb interface */
  g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error);

  /* Initialize private structure */
  self->seqnum = -1;
  self->buffer = g_malloc0 (VFS_BUFFER_SIZE);

  /* Notify open complete */
  fpi_image_device_open_complete (dev, error);
}

/* Close device */
static void
dev_close (FpImageDevice *dev)
{
  FpDeviceVfs101 *self = FPI_DEVICE_VFS101 (dev);
  GError *error = NULL;

  /* Release usb interface */
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);

  g_clear_pointer (&self->buffer, g_free);

  /* Notify close complete */
  fpi_image_device_close_complete (dev, error);
}

/* Usb id table of device */
static const FpIdEntry id_table[] = {
  { .vid = 0x138a,  .pid = 0x0001, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_vfs101_init (FpDeviceVfs101 *self)
{
}

static void
fpi_device_vfs101_class_init (FpDeviceVfs101Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "vfs101";
  dev_class->full_name = "Validity VFS101";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_open;
  img_class->img_close = dev_close;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 24;

  img_class->img_width = VFS_IMG_WIDTH;
  img_class->img_height = -1;
}
