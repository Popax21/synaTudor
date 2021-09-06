/*
 * vfs301/vfs300 fingerprint reader driver
 * https://github.com/andree182/vfs301
 *
 * Copyright (c) 2011-2012 Andrej Krutak <dev@andree.sk>
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
 * TODO:
 * - async communication everywhere :)
 * - protocol decyphering
 *   - what is needed and what is redundant
 *   - is some part of the initial data the firmware?
 *   - describe some interesting structures better
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>

#include "fpi-usb-transfer.h"
#include "vfs301.h"
#include "vfs301_proto_fragments.h"

/************************** USB STUFF *****************************************/

#ifdef DEBUG
static void
usb_print_packet (int dir, GError *error, const guint8 *data, int length)
{
  fprintf (stderr, "%s, error %s, len %d\n", dir ? "send" : "recv", error ? error->message : "-", length);

#ifdef PRINT_VERBOSE
  int i;

  for (i = 0; i < MIN (length, 128); i++)
    {
      fprintf (stderr, "%.2X ", data[i]);
      if (i % 8 == 7)
        fprintf (stderr, " ");
      if (i % 32 == 31)
        fprintf (stderr, "\n");
    }
#endif

  fprintf (stderr, "\n");
}
#endif

static void
usb_recv (FpDeviceVfs301 *dev, guint8 endpoint, int max_bytes, FpiUsbTransfer **out, GError **error)
{
  GError *err = NULL;

  g_autoptr(FpiUsbTransfer) transfer = NULL;

  /* XXX: This function swallows any transfer errors, that is obviously
   *      quite bad (it used to assert on no-error)! */

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer, endpoint, max_bytes);

  fpi_usb_transfer_submit_sync (transfer, VFS301_DEFAULT_WAIT_TIMEOUT, &err);

#ifdef DEBUG
  usb_print_packet (0, err, transfer->buffer, transfer->actual_length);
#endif
  if (err)
    {
      if (!error)
        g_warning ("Unhandled receive error: %s", err->message);
      g_propagate_error (error, err);
    }

  if (out)
    *out = g_steal_pointer (&transfer);
}

FP_GNUC_ACCESS (read_only, 2, 3)
static void
usb_send (FpDeviceVfs301 *dev, const guint8 *data, gssize length, GError **error)
{
  GError *err = NULL;

  g_autoptr(FpiUsbTransfer) transfer = NULL;

  /* XXX: This function swallows any transfer errors, that is obviously
   *      quite bad (it used to assert on no-error)! */

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk_full (transfer, VFS301_SEND_ENDPOINT, (guint8 *) data, length, g_free);

  fpi_usb_transfer_submit_sync (transfer, VFS301_DEFAULT_WAIT_TIMEOUT, &err);

#ifdef DEBUG
  usb_print_packet (1, err, data, length);
#endif

  if (err)
    {
      g_warning ("Error while sending data, continuing anyway: %s", err->message);
      g_propagate_error (error, err);
    }
}

/************************** OUT MESSAGES GENERATION ***************************/

static guint8 *
vfs301_proto_generate_0B (int subtype, gssize *len)
{
  guint8 *res = g_malloc0 (39);
  guint8 *data = res;

  *data = 0x0B;
  *len = 1;
  data++;

  *len += 38;

  data[20] = subtype;

  switch (subtype)
    {
    case 0x04:
      data[34] = 0x9F;
      break;

    case 0x05:
      data[34] = 0xAB;
      /* NOTE: There was a len++ here, which could never do anything */
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  return res;
}

#define HEX_TO_INT(c) \
  (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) - 'A' + 10))

static guint8 *
translate_str (const char **srcL, gssize *len)
{
  guint8 *res = NULL;
  guint8 *dst;
  const char **src_pos;
  const char *src;
  gssize src_len = 0;

  for (src_pos = srcL; *src_pos; src_pos++)
    {
      gint tmp;

      src = *src_pos;
      tmp = strlen (src);
      g_assert (tmp % 2 == 0);
      src_len += tmp;
    }

  g_assert (src_len >= 2);
  *len = src_len / 2;
  res = g_malloc0 (*len);
  dst = res;

  for (src_pos = srcL; *src_pos; src_pos++)
    for (src = *src_pos; *src; src += 2, dst += 1)
      *dst = (guint8) ((HEX_TO_INT (src[0]) << 4) | (HEX_TO_INT (src[1])));

  return res;
}

static guint8 *
vfs301_proto_generate (int type, int subtype, gssize *len)
{
  switch (type)
    {
    case 0x01:
    case 0x04:
    /* After cmd 0x04 is sent, a data is received on VALIDITY_RECEIVE_ENDPOINT_CTRL.
     * If it is 0x0000:
     *     additional 64B and 224B are read from _DATA, then vfs301_next_scan_FA00 is
     *     sent, 0000 received from _CTRL, and then continue with wait loop
     * If it is 0x1204:
     *     => reinit?
     */
    case 0x17:
    case 0x19:
    case 0x1A:
      {
        guint8 *data = g_malloc0 (1);
        *data = type;
        *len = 1;
        return data;
      }

    case 0x0B:
      return vfs301_proto_generate_0B (subtype, len);

    case 0x02D0:
      {
        const char **dataLs[] = {
          vfs301_02D0_01,
          vfs301_02D0_02,
          vfs301_02D0_03,
          vfs301_02D0_04,
          vfs301_02D0_05,
          vfs301_02D0_06,
          vfs301_02D0_07,
        };
        g_assert ((int) subtype <= G_N_ELEMENTS (dataLs));
        return translate_str (dataLs[subtype - 1], len);
      }

    case 0x0220:
      switch (subtype)
        {
        case 1:
          return translate_str (vfs301_0220_01, len);

        case 2:
          return translate_str (vfs301_0220_02, len);

        case 3:
          return translate_str (vfs301_0220_03, len);

        case 0xFA00:
        case 0x2C01:
        case 0x5E01: {
            guint8 *data;
            guint8 *field;

            data = translate_str (vfs301_next_scan_template, len);
            field = data + *len - (sizeof (S4_TAIL) - 1) / 2 - 4;

            g_assert (field >= data && field < data + *len);
            g_assert (field[0] == 0xDE);
            g_assert (field[1] == 0xAD);
            g_assert (field[2] == 0xDE);
            g_assert (field[3] == 0xAD);

            field[0] = (guint8) ((subtype >> 8) & 0xFF);
            field[1] = (guint8) (subtype & 0xFF);
            field[2] = field[0];
            field[3] = field[1];

            return data;
          }

        default:
          g_assert_not_reached ();
          break;
        }
      break;

    case 0x06:
    default:
      break;
    }

  g_assert_not_reached ();
  *len = 0;
  return NULL;
}

/************************** SCAN IMAGE PROCESSING *****************************/

#ifdef SCAN_FINISH_DETECTION
static int
img_is_finished_scan (fp_line_t *lines, int no_lines)
{
  int i;
  int j;
  int rv = 1;

  for (i = no_lines - VFS301_FP_SUM_LINES; i < no_lines; i++)
    {
      /* check the line for fingerprint data */
      for (j = 0; j < sizeof (lines[i].sum2); j++)
        if (lines[i].sum2[j] > (VFS301_FP_SUM_MEDIAN + VFS301_FP_SUM_EMPTY_RANGE))
          rv = 0;
    }

  return rv;
}
#endif

static int
scanline_diff (const guint8 *scanlines, int prev, int cur)
{
  const guint8 *line1 = scanlines + prev * VFS301_FP_OUTPUT_WIDTH;
  const guint8 *line2 = scanlines + cur * VFS301_FP_OUTPUT_WIDTH;
  int i;
  int diff;

#ifdef OUTPUT_RAW
  /* We only need the image, not the surrounding stuff. */
  line1 = ((vfs301_line_t *) line1)->scan;
  line2 = ((vfs301_line_t *) line2)->scan;
#endif

  /* TODO: This doesn't work too well when there are parallel lines in the
   * fingerprint. */
  for (diff = 0, i = 0; i < VFS301_FP_WIDTH; i++)
    {
      if (*line1 > *line2)
        diff += *line1 - *line2;
      else
        diff += *line2 - *line1;

      line1++;
      line2++;
    }

  return (diff / VFS301_FP_WIDTH) > VFS301_FP_LINE_DIFF_THRESHOLD;
}

/** Transform the input data to a normalized fingerprint scan */
void
vfs301_extract_image (FpDeviceVfs301 *vfs, guint8 *output, int *output_height
                     )
{
  const guint8 *scanlines = vfs->scanline_buf;
  int last_line;
  int i;

  g_assert (vfs->scanline_count >= 1);

  *output_height = 1;
  memcpy (output, scanlines, VFS301_FP_OUTPUT_WIDTH);
  last_line = 0;

  /* The following algorithm is quite trivial - it just picks lines that
   * differ more than VFS301_FP_LINE_DIFF_THRESHOLD.
   * TODO: A nicer approach would be to pick those lines and then do some kind
   * of bi/tri-linear resampling to get the output (so that we don't get so
   * many false edges etc.).
   */
  for (i = 1; i < vfs->scanline_count; i++)
    {
      if (scanline_diff (scanlines, last_line, i))
        {
          memcpy (
            output + VFS301_FP_OUTPUT_WIDTH * (*output_height),
            scanlines + VFS301_FP_OUTPUT_WIDTH * i,
            VFS301_FP_OUTPUT_WIDTH
                 );
          last_line = i;
          (*output_height)++;
        }
    }
}

static int
img_process_data (int first_block, FpDeviceVfs301 *dev, const guint8 *buf, int len)
{
  vfs301_line_t *lines = (vfs301_line_t *) buf;
  int no_lines = len / sizeof (vfs301_line_t);
  int i;
  /*int no_nonempty;*/
  guint8 *cur_line;
  int last_img_height;

#ifdef SCAN_FINISH_DETECTION
  int finished_scan;
#endif

  if (first_block)
    {
      last_img_height = 0;
      dev->scanline_count = no_lines;
    }
  else
    {
      last_img_height = dev->scanline_count;
      dev->scanline_count += no_lines;
    }

  dev->scanline_buf = g_realloc (dev->scanline_buf, dev->scanline_count * VFS301_FP_OUTPUT_WIDTH);

  for (cur_line = dev->scanline_buf + last_img_height * VFS301_FP_OUTPUT_WIDTH, i = 0;
       i < no_lines;
       i++, cur_line += VFS301_FP_OUTPUT_WIDTH
      )
    {
#ifndef OUTPUT_RAW
      memcpy (cur_line, lines[i].scan, VFS301_FP_OUTPUT_WIDTH);
#else
      memcpy (cur_line, &lines[i], VFS301_FP_OUTPUT_WIDTH);
#endif
    }

#ifdef SCAN_FINISH_DETECTION
  finished_scan = img_is_finished_scan (lines, no_lines);

  return !finished_scan;
#else /* SCAN_FINISH_DETECTION */
  return 1;       /* Just continue until data is coming */
#endif
}

/************************** PROTOCOL STUFF ************************************/

#define USB_RECV(from, len) \
  usb_recv (dev, from, len, NULL, NULL)

#define USB_SEND(type, subtype) \
  { \
    const guint8 *data; \
    gssize len; \
    data = vfs301_proto_generate (type, subtype, &len); \
    usb_send (dev, data, len, NULL); \
  }

#define RAW_DATA(x) g_memdup (x, sizeof (x)), sizeof (x)

#define IS_VFS301_FP_SEQ_START(b) ((b[0] == 0x01) && (b[1] == 0xfe))

static int
vfs301_proto_process_data (FpDeviceVfs301 *dev, int first_block, const guint8 *buf, gint len)
{
  int i;

  if (first_block)
    {
      g_assert (len >= VFS301_FP_FRAME_SIZE);

      /* Skip bytes until start_sequence is found */
      for (i = 0; i < VFS301_FP_FRAME_SIZE; i++, buf++, len--)
        if (IS_VFS301_FP_SEQ_START (buf))
          break;
    }

  return img_process_data (first_block, dev, buf, len);
}

void
vfs301_proto_request_fingerprint (FpDeviceVfs301 *dev)
{
  USB_SEND (0x0220, 0xFA00);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 000000000000 */
}

int
vfs301_proto_peek_event (FpDeviceVfs301 *dev)
{
  g_autoptr(GError) error = NULL;
  FpiUsbTransfer *transfer;

  const char no_event[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const char got_event[] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};

  USB_SEND (0x17, -1);
  usb_recv (dev, VFS301_RECEIVE_ENDPOINT_CTRL, 7, &transfer, &error);

  /* XXX: This is obviously not a sane error handling! */
  g_assert (!error);

  if (memcmp (transfer->buffer, no_event, sizeof (no_event)) == 0)
    return 0;
  else if (memcmp (transfer->buffer, got_event, sizeof (no_event)) == 0)
    return 1;
  else
    g_assert_not_reached ();
}

/* XXX: We sometimes need to receive data on from two endpoints at the same
 *      time. However, as this driver is currently all synchronous (yikes),
 *      we will run into timeouts randomly and need to then try again.
 */
#define PARALLEL_RECEIVE(e1, l1, e2, l2) \
  { \
    g_autoptr(GError) error = NULL; \
    usb_recv (dev, e1, l1, NULL, &error); \
    usb_recv (dev, e2, l2, NULL, NULL); \
    if (g_error_matches (error, G_USB_DEVICE_ERROR, G_USB_DEVICE_ERROR_TIMED_OUT)) \
    usb_recv (dev, e1, l1, NULL, NULL); \
  }

static void
vfs301_proto_process_event_cb (FpiUsbTransfer *transfer,
                               FpDevice *device,
                               gpointer user_data, GError *error)
{
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (device);

  if (error)
    {
      g_warning ("Error receiving data: %s", error->message);
      g_error_free (error);
      self->recv_progress = VFS301_FAILURE;
      return;
    }
  else if (transfer->actual_length < transfer->length)
    {
      /* TODO: process the data anyway? */
      self->recv_progress = VFS301_ENDED;
      return;
    }
  else
    {
      FpiUsbTransfer *new;
      if (!vfs301_proto_process_data (self,
                                      transfer->length == VFS301_FP_RECV_LEN_1,
                                      transfer->buffer,
                                      transfer->actual_length))
        {
          self->recv_progress = VFS301_ENDED;
          return;
        }

      new = fpi_usb_transfer_new (device);

      fpi_usb_transfer_fill_bulk (new, VFS301_RECEIVE_ENDPOINT_DATA, VFS301_FP_RECV_LEN_2);
      fpi_usb_transfer_submit (new, VFS301_FP_RECV_TIMEOUT, NULL,
                               vfs301_proto_process_event_cb, NULL);
      return;
    }
}

void
vfs301_proto_process_event_start (FpDeviceVfs301 *dev)
{
  FpiUsbTransfer *transfer;

  /*
   * Notes:
   *
   * seen next_scan order:
   *    o FA00
   *    o FA00
   *    o 2C01
   *    o FA00
   *    o FA00
   *    o 2C01
   *    o FA00
   *    o FA00
   *    o 2C01
   *    o 5E01 !?
   *    o FA00
   *    o FA00
   *    o 2C01
   *    o FA00
   *    o FA00
   *    o 2C01
   */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 64);

  /* now read the fingerprint data, while there are some */
  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  dev->recv_progress = VFS301_ONGOING;

  fpi_usb_transfer_fill_bulk (transfer, VFS301_RECEIVE_ENDPOINT_DATA, VFS301_FP_RECV_LEN_1);
  fpi_usb_transfer_submit (transfer, VFS301_FP_RECV_TIMEOUT, NULL,
                           vfs301_proto_process_event_cb, NULL);
}

int
vfs301_proto_process_event_poll (FpDeviceVfs301 *dev)
{
  if (dev->recv_progress != VFS301_ENDED)
    return dev->recv_progress;

  /* Finish the scan process... */

  USB_SEND (0x04, -1);
  /* the following may come in random order, data may not come at all, don't
   * try for too long... */
  PARALLEL_RECEIVE (
    VFS301_RECEIVE_ENDPOINT_CTRL, 2,             /* 1204 */
    VFS301_RECEIVE_ENDPOINT_DATA, 16384
                   );

  USB_SEND (0x0220, 2);
  PARALLEL_RECEIVE (
    VFS301_RECEIVE_ENDPOINT_DATA, 5760,             /* seems to always come */
    VFS301_RECEIVE_ENDPOINT_CTRL, 2             /* 0000 */
                   );

  return dev->recv_progress;
}

void
vfs301_proto_init (FpDeviceVfs301 *dev)
{
  USB_SEND (0x01, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 38);
  USB_SEND (0x0B, 0x04);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 6);      /* 000000000000 */
  USB_SEND (0x0B, 0x05);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 7);      /* 00000000000000 */
  USB_SEND (0x19, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 64);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 4);      /* 6BB4D0BC */
  usb_send (dev, RAW_DATA (vfs301_06_1), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */

  USB_SEND (0x01, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 38);
  USB_SEND (0x1A, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_06_2), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_SEND (0x0220, 1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 256);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 32);

  USB_SEND (0x1A, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_06_3), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */

  USB_SEND (0x01, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 38);
  USB_SEND (0x02D0, 1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 11648);      /* 56 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 2);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 53248);      /* 2 * 128 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 3);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 19968);      /* 96 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 4);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 5824);      /* 28 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 5);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 6656);      /* 32 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 6);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 6656);      /* 32 * vfs301_init_line_t[] */
  USB_SEND (0x02D0, 7);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 832);
  usb_send (dev, RAW_DATA (vfs301_12), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */

  USB_SEND (0x1A, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_06_2), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  USB_SEND (0x0220, 2);
  PARALLEL_RECEIVE (
    VFS301_RECEIVE_ENDPOINT_CTRL, 2,             /* 0000 */
    VFS301_RECEIVE_ENDPOINT_DATA, 5760
                   );

  USB_SEND (0x1A, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_06_1), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */

  USB_SEND (0x1A, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_06_4), NULL);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */
  usb_send (dev, RAW_DATA (vfs301_24), NULL);     /* turns on white */
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2);      /* 0000 */

  USB_SEND (0x01, -1);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 38);
  USB_SEND (0x0220, 3);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 2368);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_CTRL, 36);
  USB_RECV (VFS301_RECEIVE_ENDPOINT_DATA, 5760);
}

void
vfs301_proto_deinit (FpDeviceVfs301 *dev)
{
}
