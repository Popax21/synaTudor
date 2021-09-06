/*
 * UPEK TouchStrip driver for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 *
 * Based in part on libthinkfinger:
 * Copyright (C) 2006-2007 Timo Hoenig <thoenig@suse.de>
 * Copyright (C) 2006 Pavel Machek <pavel@suse.cz>
 *
 * LGPL CRC code copied from GStreamer-0.10.10:
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2004,2006 Thomas Vander Stichele <thomas at apestaart dot org>

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version
 * 2.1 of the License.
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

#define FP_COMPONENT "upekts"

#include "drivers_api.h"
#include "upek_proto.h"

#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)
#define TIMEOUT 5000

#define MSG_READ_BUF_SIZE 0x40

struct _FpiDeviceUpekts
{
  FpDevice parent;

  gboolean enroll_passed;
  gint     enroll_stage;
  gboolean first_verify_iteration;
  guint8   seq;     /* FIXME: improve/automate seq handling */
};

G_DECLARE_FINAL_TYPE (FpiDeviceUpekts, fpi_device_upekts, FPI,
                      DEVICE_UPEKTS, FpDevice);
G_DEFINE_TYPE (FpiDeviceUpekts, fpi_device_upekts, FP_TYPE_DEVICE);

/*
 * MESSAGE FORMAT
 *
 * Messages to and from the device have the same format.
 *
 * Byte-wise:
 *              'C' 'i' 'a' 'o' A B L <DATA> C1 C2
 *
 * Ciao prefixes all messages. The rightmost 4 bits of B become the uppermost
 * 4 bits of L, and when combined with the lower 8 bits listed as 'L', L is
 * the length of the data, <DATA> is L bytes long. C1 and C2 are the
 * UDF-CRC16 for the whole message minus the Ciao prefix.
 *
 * When the device wants to command the driver to do something, it sends
 * a message where B=0 and A!=0. The A value indicates the type of command.
 * If the system is expected to respond to the command, it sends a message back
 * with B=0 and A incremented.
 *
 * When the driver sends a command to the device, A=0 and B is used as a
 * sequence counter. It starts at 0, increments by 0x10 on each command, and
 * wraps around.
 * After each command is sent, the device responds with another message
 * indicating completion of the command including any data that was requested.
 * This message has the same A and B values.
 *
 * When the driver is sending commands as above, and when the device is
 * responding, the <DATA> seems to follow this structure:
 *
 *              28 L1 L2 0 0 S <INNERDATA>
 *
 * Where the length of <INNERDATA> is L-3, and S is some kind of subcommand
 * code. L1 is the least significant bits of L, L2 is the most significant. In
 * the device's response to a command, the subcommand code will be unchanged.
 *
 * After deducing and documenting the above, I found a few places where the
 * above doesn't hold true. Those are marked with FIXME's below.
 */

#define CMD_SEQ_INCREMENT 0x10

static FpiUsbTransfer *
alloc_send_cmd_transfer (FpDevice            *dev,
                         unsigned char        seq_a,
                         unsigned char        seq_b,
                         const unsigned char *data,
                         guint16              len)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (dev);
  guint16 crc;
  const char *ciao = "Ciao";

  /* 9 bytes extra for: 4 byte 'Ciao', 1 byte A, 1 byte B | lenHI,
   * 1 byte lenLO, 2 byte CRC */
  size_t urblen = len + 9;

  if (!data && len > 0)
    {
      fp_err ("len>0 but no data?");
      return NULL;
    }

  fpi_usb_transfer_fill_bulk (transfer, EP_OUT, urblen);

  /* Write header */
  memcpy (transfer->buffer, ciao, strlen (ciao));
  transfer->buffer[4] = seq_a;
  transfer->buffer[5] = seq_b | ((len & 0xf00) >> 8);
  transfer->buffer[6] = len & 0x00ff;

  /* Copy data */
  if (data)
    memcpy (transfer->buffer + 7, data, len);

  /* Append CRC */
  crc = udf_crc (transfer->buffer + 4, urblen - 6);
  transfer->buffer[urblen - 2] = crc & 0xff;
  transfer->buffer[urblen - 1] = crc >> 8;

  return transfer;
}

static FpiUsbTransfer *
alloc_send_cmd28_transfer (FpDevice            *dev,
                           unsigned char        subcmd,
                           const unsigned char *data,
                           guint16              innerlen)
{
  guint16 _innerlen = innerlen;
  size_t len = innerlen + 6;
  unsigned char *buf = g_malloc0 (len);
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);
  guint8 seq = upekdev->seq + CMD_SEQ_INCREMENT;
  FpiUsbTransfer *ret;

  fp_dbg ("seq=%02x subcmd=%02x with %d bytes of data", seq, subcmd, innerlen);

  _innerlen = innerlen + 3;
  buf[0] = 0x28;
  buf[1] = _innerlen & 0x00ff;
  buf[2] = (_innerlen & 0xff00) >> 8;
  buf[5] = subcmd;
  memcpy (buf + 6, data, innerlen);

  ret = alloc_send_cmd_transfer (dev, 0, seq, buf, len);
  upekdev->seq = seq;

  g_free (buf);
  return ret;
}

static FpiUsbTransfer *
alloc_send_cmdresponse_transfer (FpDevice            *dev,
                                 unsigned char        seq,
                                 const unsigned char *data,
                                 guint8               len)
{
  fp_dbg ("seq=%02x len=%d", seq, len);
  return alloc_send_cmd_transfer (dev, seq, 0, data, len);
}

enum read_msg_type {
  READ_MSG_CMD,
  READ_MSG_RESPONSE,
};

typedef void (*read_msg_cb_fn)(FpDevice          *dev,
                               enum read_msg_type type,
                               guint8             seq,
                               unsigned char      subcmd,
                               unsigned char     *data,
                               size_t             data_len,
                               void              *user_data,
                               GError            *error);

struct read_msg_data
{
  gssize         buflen;
  guint8        *buffer;
  read_msg_cb_fn callback;
  void          *user_data;
};

static void __read_msg_async (FpDevice             *dev,
                              struct read_msg_data *udata);

#define READ_MSG_DATA_CB_ERR(dev, udata, error) (udata)->callback (dev, \
                                                                   READ_MSG_CMD, 0, 0, NULL, 0, (udata)->user_data, error)

static void
busy_ack_sent_cb (FpiUsbTransfer *transfer, FpDevice *device,
                  gpointer user_data, GError *error)
{
  struct read_msg_data *udata = user_data;

  if (error)
    {
      READ_MSG_DATA_CB_ERR (device, udata, error);
      g_free (udata->buffer);
      g_free (udata);
    }
  else
    {
      __read_msg_async (device, udata);
    }
}

static void
busy_ack_retry_read (FpDevice *device, struct read_msg_data *udata)
{
  FpiUsbTransfer *transfer;

  transfer = alloc_send_cmdresponse_transfer (device, 0x09, NULL, 0);
  transfer->short_is_error = TRUE;

  fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, busy_ack_sent_cb, udata);
}

/* Returns 0 if message was handled, 1 if it was a device-busy message, and
 * negative on error. */
static void
__handle_incoming_msg (FpDevice             *device,
                       struct read_msg_data *udata)
{
  GError *error = NULL;
  guint8 *buf = udata->buffer;
  guint16 len;
  guint16 computed_crc;
  guint16 msg_crc;
  unsigned char code_a, code_b;

  g_assert (udata->buflen >= 6);
  len = ((buf[5] & 0xf) << 8) | buf[6];

  g_assert (udata->buflen >= len + 9);
  computed_crc = udf_crc (buf + 4, len + 3);
  msg_crc = (buf[len + 8] << 8) | buf[len + 7];

  if (computed_crc != msg_crc)
    {
      fp_err ("CRC failed, got %04x expected %04x", msg_crc, computed_crc);
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                        "CRC check on message failed");
      goto err;
    }

  code_a = buf[4];
  code_b = buf[5] & 0xf0;
  len = ((buf[5] & 0xf) << 8) | buf[6];
  fp_dbg ("A=%02x B=%02x len=%d", code_a, code_b, len);

  if (code_a && !code_b)
    {
      /* device sends command to driver */
      fp_dbg ("cmd %x from device to driver", code_a);

      if (code_a == 0x08)
        {
          fp_dbg ("device busy, send busy-ack");
          busy_ack_retry_read (device, udata);
          return;
        }

      udata->callback (device, READ_MSG_CMD, code_a, 0, buf + 7, len,
                       udata->user_data, NULL);
      goto done;
    }
  else if (!code_a)
    {
      /* device sends response to a previously executed command */
      unsigned char *innerbuf = buf + 7;
      unsigned char _subcmd;
      guint16 innerlen;

      if (len < 6)
        {
          fp_warn ("cmd response too short (%d)", len);
          error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                            "CMD response too short (%d)", len);
          goto err;
        }
      if (innerbuf[0] != 0x28)
        {
          fp_warn ("cmd response without 28 byte?");
          error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                            "CMD response without 0x28 byte");
          goto err;
        }

      /* not really sure what these 2 bytes are. on most people's hardware,
       * these bytes are always 0. However, Alon Bar-Lev's hardware gives
       * 0xfb 0xff during the READ28_OB initsm stage. so don't error out
       * if they are different... */
      if (innerbuf[3] || innerbuf[4])
        fp_dbg ("non-zero bytes in cmd response");

      innerlen = innerbuf[1] | (innerbuf[2] << 8);
      innerlen = innerlen - 3;
      _subcmd = innerbuf[5];
      fp_dbg ("device responds to subcmd %x with %d bytes", _subcmd, innerlen);
      udata->callback (device, READ_MSG_RESPONSE, code_b, _subcmd,
                       innerbuf + 6, innerlen, udata->user_data, NULL);
      goto done;
    }
  else
    {
      fp_err ("don't know how to handle this message");
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                        "Message cannot be processed");
      goto err;
    }
  g_assert_not_reached ();

err:
  READ_MSG_DATA_CB_ERR (device, udata, error);
done:
  g_free (udata->buffer);
  g_free (udata);
}

static void
read_msg_extend_cb (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  struct read_msg_data *udata = user_data;

  if (error)
    {
      fp_err ("extended msg read failed: %s", error->message);
      READ_MSG_DATA_CB_ERR (device, udata, error);
      g_free (udata->buffer);
      g_free (udata);
      return;
    }

  __handle_incoming_msg (device, udata);
}

static void
read_msg_cb (FpiUsbTransfer *transfer, FpDevice *device,
             gpointer user_data, GError *error)
{
  struct read_msg_data *udata = user_data;
  guint16 payload_len;
  gsize packet_len;

  if (error)
    {
      fp_err ("async msg read failed: %s", error->message);
      goto err;
    }
  if (transfer->actual_length < 9)
    {
      fp_err ("async msg read too short (%d)",
              (gint) transfer->actual_length);
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                        "Packet from device was too short (%lu)",
                                        transfer->actual_length);
      goto err;
    }

  if (strncmp ((char *) udata->buffer, "Ciao", 4) != 0)
    {
      fp_err ("no Ciao for you!!");
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                        "Packet from device had incorrect header");
      goto err;
    }

  payload_len = ((udata->buffer[5] & 0xf) << 8) | udata->buffer[6];
  packet_len = payload_len + 9;
  if (transfer->actual_length != MSG_READ_BUF_SIZE &&
      packet_len > transfer->actual_length)
    {
      /* Check that the length claimed inside the message is in line with
       * the amount of data that was transferred over USB. */
      fp_err ("msg didn't include enough data, expected=%d recv=%d",
              (gint) packet_len, (gint) transfer->actual_length);
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                        "Packet from device didn't include data");
      goto err;
    }

  /* We use a 64 byte buffer for reading messages. However, sometimes
   * messages are longer, in which case we have to do another USB bulk read
   * to read the remainder. This is handled below. */
  if (packet_len > MSG_READ_BUF_SIZE)
    {
      int needed = packet_len - MSG_READ_BUF_SIZE;
      FpiUsbTransfer *etransfer = fpi_usb_transfer_new (device);

      fp_dbg ("didn't fit in buffer, need to extend by %d bytes", needed);
      udata->buffer = g_realloc ((gpointer) udata->buffer, packet_len);
      udata->buflen = packet_len;

      fpi_usb_transfer_fill_bulk_full (etransfer, EP_IN,
                                       udata->buffer + MSG_READ_BUF_SIZE,
                                       needed, NULL);
      etransfer->short_is_error = TRUE;
      fpi_usb_transfer_submit (etransfer, TIMEOUT,
                               NULL,
                               read_msg_extend_cb, udata);
      return;
    }

  __handle_incoming_msg (device, udata);

  return;
err:
  READ_MSG_DATA_CB_ERR (device, udata, error);
  g_free (udata->buffer);
  g_free (udata);
}

static void
__read_msg_async (FpDevice *device, struct read_msg_data *udata)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (device);

  if (udata->buflen != MSG_READ_BUF_SIZE)
    {
      udata->buffer = g_realloc (udata->buffer, MSG_READ_BUF_SIZE);
      udata->buflen = MSG_READ_BUF_SIZE;
    }

  fpi_usb_transfer_fill_bulk_full (transfer, EP_IN, udata->buffer, udata->buflen, NULL);
  fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, read_msg_cb, udata);
}

static void
read_msg_async (FpDevice      *dev,
                read_msg_cb_fn callback,
                void          *user_data)
{
  struct read_msg_data *udata = g_new0 (struct read_msg_data, 1);

  udata->buflen = 0;
  udata->buffer = NULL;
  udata->callback = callback;
  udata->user_data = user_data;
  __read_msg_async (dev, udata);
}

static const unsigned char init_resp03[] = {
  0x01, 0x00, 0xe8, 0x03, 0x00, 0x00, 0xff, 0x07
};
static const unsigned char init28_08[] = {
  0x04, 0x83, 0x00, 0x2c, 0x22, 0x23, 0x97, 0xc9, 0xa7, 0x15, 0xa0, 0x8a,
  0xab, 0x3c, 0xd0, 0xbf, 0xdb, 0xf3, 0x92, 0x6f, 0xae, 0x3b, 0x1e, 0x44,
  0xc4
};
static const unsigned char init28_0c[] = {
  0x04, 0x03, 0x00, 0x00, 0x00
};
static const unsigned char init28_0b[] = {
  0x04, 0x03, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x64, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x0a,
  0x00, 0x64, 0x00, 0xf4, 0x01, 0x32, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00
};

/* device initialisation state machine */

enum initsm_states {
  WRITE_CTRL400 = 0,
  READ_MSG03,
  SEND_RESP03,
  READ_MSG05,
  SEND28_06,
  READ28_06,
  SEND28_07,
  READ28_07,
  SEND28_08,
  READ28_08,
  SEND28_0C,
  READ28_0C,
  SEND28_0B,
  READ28_0B,
  INITSM_NUM_STATES,
};

static void
initsm_read_msg_response_cb (FpiSsm            *ssm,
                             FpDevice          *dev,
                             enum read_msg_type type,
                             guint8             seq,
                             unsigned char      expect_subcmd,
                             unsigned char      subcmd,
                             GError            *error)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
    }
  else if (type != READ_MSG_RESPONSE)
    {
      fp_err ("expected response, got %d seq=%x in state %d", type, seq,
              fpi_ssm_get_cur_state (ssm));
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Unexpected message type"));
    }
  else if (seq != upekdev->seq)
    {
      fp_warn ("expected response to subcmd 0x%02x, got response to %02x in "
               "state %d", expect_subcmd, subcmd,
               fpi_ssm_get_cur_state (ssm));
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Unexpected response subcommand"));
    }
  else
    {
      fpi_ssm_next_state (ssm);
    }
}

static void
read28_0b_cb (FpDevice *dev, enum read_msg_type type,
              guint8 seq, unsigned char subcmd,
              unsigned char *data, size_t data_len,
              void *user_data, GError *error)
{
  initsm_read_msg_response_cb ((FpiSsm *) user_data, dev, type, seq,
                               0x0b, subcmd, error);
}

static void
read28_0c_cb (FpDevice *dev, enum read_msg_type type,
              guint8 seq, unsigned char subcmd,
              unsigned char *data, size_t data_len,
              void *user_data, GError *error)
{
  initsm_read_msg_response_cb ((FpiSsm *) user_data, dev, type, seq,
                               0x0c, subcmd, error);
}

static void
read28_08_cb (FpDevice *dev, enum read_msg_type type,
              guint8 seq, unsigned char subcmd,
              unsigned char *data, size_t data_len,
              void *user_data, GError *error)
{
  initsm_read_msg_response_cb ((FpiSsm *) user_data, dev, type, seq,
                               0x08, subcmd, error);
}

static void
read28_07_cb (FpDevice *dev, enum read_msg_type type,
              guint8 seq, unsigned char subcmd,
              unsigned char *data, size_t data_len,
              void *user_data, GError *error)
{
  initsm_read_msg_response_cb ((FpiSsm *) user_data, dev, type, seq,
                               0x07, subcmd, error);
}

static void
read28_06_cb (FpDevice *dev, enum read_msg_type type,
              guint8 seq, unsigned char subcmd,
              unsigned char *data, size_t data_len,
              void *user_data, GError *error)
{
  initsm_read_msg_response_cb ((FpiSsm *) user_data, dev, type, seq,
                               0x06, subcmd, error);
}

static void
initsm_read_msg_cmd_cb (FpiSsm            *ssm,
                        FpDevice          *dev,
                        enum read_msg_type type,
                        guint8             seq,
                        guint8             expected_seq,
                        GError            *error)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
      return;
    }
  else if (type != READ_MSG_CMD)
    {
      fp_err ("expected command, got %d seq=%x in state %d", type, seq,
              fpi_ssm_get_cur_state (ssm));
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Expected command but got response"));
      return;
    }
  upekdev->seq = seq;
  if (seq != expected_seq)
    {
      fp_err ("expected seq=%x, got %x in state %d", expected_seq, seq,
              fpi_ssm_get_cur_state (ssm));
      fpi_ssm_mark_failed (ssm,
                           fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                     "Got unexpected sequence number"));
      return;
    }

  fpi_ssm_next_state (ssm);
}

static void
read_msg05_cb (FpDevice *dev, enum read_msg_type type,
               guint8 seq, unsigned char subcmd,
               unsigned char *data, size_t data_len,
               void *user_data, GError *error)
{
  initsm_read_msg_cmd_cb ((FpiSsm *) user_data, dev, type, 5, seq, error);
}

static void
read_msg03_cb (FpDevice *dev, enum read_msg_type type,
               guint8 seq, unsigned char subcmd,
               unsigned char *data, size_t data_len,
               void *user_data, GError *error)
{
  initsm_read_msg_cmd_cb ((FpiSsm *) user_data, dev, type, 3, seq, error);
}

static void
initsm_read_msg_handler (FpiSsm        *ssm,
                         FpDevice      *dev,
                         read_msg_cb_fn callback)
{
  read_msg_async (dev, callback, ssm);
}

static void
initsm_send_msg28_handler (FpiSsm              *ssm,
                           FpDevice            *dev,
                           unsigned char        subcmd,
                           const unsigned char *data,
                           guint16              innerlen)
{
  FpiUsbTransfer *transfer;

  transfer = alloc_send_cmd28_transfer (dev, subcmd, data, innerlen);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);
}

static void
initsm_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);
  FpiUsbTransfer *transfer;

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case WRITE_CTRL400:;
      transfer = fpi_usb_transfer_new (dev);
      fpi_usb_transfer_fill_control (transfer,
                                     G_USB_DEVICE_DIRECTION_HOST_TO_DEVICE,
                                     G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                     G_USB_DEVICE_RECIPIENT_DEVICE,
                                     0x0c, 0x100, 0x0400, 1);
      transfer->ssm = ssm;
      transfer->short_is_error = TRUE;
      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);
      break;

    case READ_MSG03:
      initsm_read_msg_handler (ssm, dev, read_msg03_cb);
      break;

    case SEND_RESP03:;
      transfer = alloc_send_cmdresponse_transfer (dev, ++upekdev->seq, init_resp03, sizeof (init_resp03));
      transfer->ssm = ssm;
      transfer->short_is_error = TRUE;
      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);
      break;

    case READ_MSG05:
      initsm_read_msg_handler (ssm, dev, read_msg05_cb);
      break;

    case SEND28_06:;
      unsigned char dummy28_06 = 0x04;
      upekdev->seq = 0xf0;
      initsm_send_msg28_handler (ssm, dev, 0x06, &dummy28_06, 1);
      break;

    case READ28_06:
      initsm_read_msg_handler (ssm, dev, read28_06_cb);
      break;

    case SEND28_07:;
      unsigned char dummy28_07 = 0x04;
      initsm_send_msg28_handler (ssm, dev, 0x07, &dummy28_07, 1);
      break;

    case READ28_07:
      initsm_read_msg_handler (ssm, dev, read28_07_cb);
      break;

    case SEND28_08:
      initsm_send_msg28_handler (ssm, dev, 0x08, init28_08, sizeof (init28_08));
      break;

    case READ28_08:
      initsm_read_msg_handler (ssm, dev, read28_08_cb);
      break;

    case SEND28_0C:
      initsm_send_msg28_handler (ssm, dev, 0x0c, init28_0c, sizeof (init28_0c));
      break;

    case READ28_0C:
      initsm_read_msg_handler (ssm, dev, read28_0c_cb);
      break;

    case SEND28_0B:
      initsm_send_msg28_handler (ssm, dev, 0x0b, init28_0b, sizeof (init28_0b));
      break;

    case READ28_0B:
      initsm_read_msg_handler (ssm, dev, read28_0b_cb);
      break;
    }
}

static FpiSsm *
initsm_new (FpDevice *dev)
{
  return fpi_ssm_new (dev, initsm_run_state, INITSM_NUM_STATES);
}

enum deinitsm_states {
  SEND_RESP07 = 0,
  READ_MSG01,
  DEINITSM_NUM_STATES,
};

static void
read_msg01_cb (FpDevice *dev, enum read_msg_type type,
               guint8 seq, unsigned char subcmd,
               unsigned char *data, size_t data_len,
               void *user_data, GError *error)
{
  FpiSsm *ssm = user_data;
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
      return;
    }
  else if (type != READ_MSG_CMD)
    {
      fp_err ("expected command, got %d seq=%x", type, seq);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                          "Expected command but got response"));
      return;
    }
  upekdev->seq = seq;
  if (seq != 1)
    {
      fp_err ("expected seq=1, got %x", seq);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                          "Got wrong sequence number (%x)",
                                                          seq));
      return;
    }

  fpi_ssm_next_state (ssm);
}

static void
deinitsm_state_handler (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case SEND_RESP07:;
      FpiUsbTransfer *transfer;
      unsigned char dummy = 0;

      transfer = alloc_send_cmdresponse_transfer (dev, 0x07, &dummy, 1);
      transfer->short_is_error = TRUE;
      transfer->ssm = ssm;
      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);
      break;

    case READ_MSG01:;
      read_msg_async (dev, read_msg01_cb, ssm);
      break;
    }
}

static void
initsm_done (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  if (error)
    g_usb_device_release_interface (fpi_device_get_usb_device (dev), 0, 0, NULL);

  fpi_device_open_complete (dev, error);
}

static FpiSsm *
deinitsm_new (FpDevice *dev, void *user_data)
{
  return fpi_ssm_new (dev, deinitsm_state_handler, DEINITSM_NUM_STATES);
}

static void
dev_init (FpDevice *dev)
{
  FpiSsm *ssm;
  GError *error = NULL;
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (dev), 0, 0, &error))
    {
      fpi_device_open_complete (dev, error);
      return;
    }

  upekdev->seq = 0xf0;       /* incremented to 0x00 before first cmd */

  ssm = fpi_ssm_new (dev, initsm_run_state, INITSM_NUM_STATES);
  fpi_ssm_start (ssm, initsm_done);
}

static void
dev_exit (FpDevice *dev)
{
  GError *error = NULL;

  g_usb_device_release_interface (fpi_device_get_usb_device (dev), 0, 0, &error);

  fpi_device_close_complete (dev, error);
}

static const unsigned char enroll_init[] = {
  0x02, 0xc0, 0xd4, 0x01, 0x00, 0x04, 0x00, 0x08
};
static const unsigned char scan_comp[] = {
  0x12, 0xff, 0xff, 0xff, 0xff       /* scan completion, prefixes print data */
};

/* used for enrollment and verification */
static const unsigned char poll_data[] = { 0x30, 0x01 };

enum enroll_start_sm_states {
  RUN_INITSM = 0,
  ENROLL_INIT,
  READ_ENROLL_MSG28,
  ENROLL_START_NUM_STATES,
};

static void
enroll_start_sm_cb_msg28 (FpDevice *dev,
                          enum read_msg_type type, guint8 seq,
                          unsigned char subcmd,
                          unsigned char *data, size_t data_len,
                          void *user_data,
                          GError *error)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);
  FpiSsm *ssm = user_data;

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
    }
  else if (type != READ_MSG_RESPONSE)
    {
      fp_err ("expected response, got %d seq=%x", type, seq);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                          "Unexpected response type"));
    }
  else if (subcmd != 0)
    {
      fp_warn ("expected response to subcmd 0, got response to %02x",
               subcmd);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                          "Got response to wrong subcommand"));
    }
  else if (seq != upekdev->seq)
    {
      fp_err ("expected response to cmd seq=%02x, got response to %02x",
              upekdev->seq, seq);
      fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                          "Got response with wrong sequence number"));
    }
  else
    {
      fpi_ssm_next_state (ssm);
    }
}

static void
enroll_start_sm_run_state (FpiSsm *ssm, FpDevice *dev)
{
  switch (fpi_ssm_get_cur_state (ssm))
    {
    case RUN_INITSM:;
      FpiSsm *initsm = initsm_new (dev);
      fpi_ssm_start_subsm (ssm, initsm);
      break;

    case ENROLL_INIT:;
      FpiUsbTransfer *transfer;
      transfer = alloc_send_cmd28_transfer (dev, 0x02, enroll_init, sizeof (enroll_init));
      transfer->short_is_error = TRUE;
      transfer->ssm = ssm;

      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);
      break;

    case READ_ENROLL_MSG28:;
      /* FIXME: protocol misunderstanding here. device receives response
       * to subcmd 0 after submitting subcmd 2? */
      /* actually this is probably a poll response? does the above cmd
       * include a 30 01 poll somewhere? */
      read_msg_async (dev, enroll_start_sm_cb_msg28, ssm);
      break;
    }
}

typedef struct
{
  FpPrint *print;
  GError  *error;
} EnrollStopData;

static void
enroll_stop_data_free (EnrollStopData *data)
{
  g_clear_object (&data->print);
  g_clear_error (&data->error);
  g_free (data);
}

static void
enroll_stop_deinit_cb (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  EnrollStopData *data = fpi_ssm_get_data (ssm);

  /* don't really care about errors */
  if (error)
    fp_warn ("Error deinitializing: %s", error->message);

  fpi_device_enroll_complete (dev,
                              g_steal_pointer (&data->print),
                              g_steal_pointer (&data->error));
}

static void
do_enroll_stop (FpDevice *dev, FpPrint *print, GError *error)
{
  EnrollStopData *data = g_new0 (EnrollStopData, 1);
  FpiSsm *ssm = deinitsm_new (dev, data);

  data->print = print;
  data->error = error;

  fpi_ssm_start (ssm, enroll_stop_deinit_cb);
  fpi_ssm_set_data (ssm, data, (GDestroyNotify) enroll_stop_data_free);
}

static void enroll_iterate (FpDevice *dev);

static void
e_handle_resp00 (FpDevice *dev, unsigned char *data,
                 size_t data_len)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);
  unsigned char status;

  if (data_len != 14)
    {
      fp_err ("received 3001 poll response of %lu bytes?", data_len);
      do_enroll_stop (dev, NULL,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "received 3001 response with wrong length"));
      return;
    }

  status = data[5];
  fp_dbg ("poll result = %02x", status);

  switch (status)
    {
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x26:
    case 0x27:
    case 0x2e:
      /* if we previously completed a non-last enrollment stage, we'll
       * get this code to indicate successful stage completion */
      if (upekdev->enroll_passed)
        {
          upekdev->enroll_passed = FALSE;
          upekdev->enroll_stage += 1;

          fpi_device_enroll_progress (dev, upekdev->enroll_stage, NULL, NULL);
        }
      /* otherwise it just means "no news" so we poll again */
      break;

    case 0x1c:     /* FIXME what does this one mean? */
    case 0x0b:     /* FIXME what does this one mean? */
    case 0x23:     /* FIXME what does this one mean? */
      fpi_device_enroll_progress (dev,
                                  upekdev->enroll_stage,
                                  NULL,
                                  fpi_device_retry_new (FP_DEVICE_RETRY_GENERAL));
      break;

    case 0x0f:     /* scan taking too long, remove finger and try again */
      fpi_device_enroll_progress (dev,
                                  upekdev->enroll_stage,
                                  NULL,
                                  fpi_device_retry_new (FP_DEVICE_RETRY_REMOVE_FINGER));
      break;

    case 0x1e:     /* swipe too short */
      fpi_device_enroll_progress (dev,
                                  upekdev->enroll_stage,
                                  NULL,
                                  fpi_device_retry_new (FP_DEVICE_RETRY_TOO_SHORT));
      break;

    case 0x24:     /* finger not centered */
      fpi_device_enroll_progress (dev,
                                  upekdev->enroll_stage,
                                  NULL,
                                  fpi_device_retry_new (FP_DEVICE_RETRY_CENTER_FINGER));
      break;

    case 0x20:
      /* finger scanned successfully */
      /* need to look at the next poll result to determine if enrollment is
       * complete or not */
      upekdev->enroll_passed = TRUE;
      break;

    case 0x00:     /* enrollment complete */
      /* we can now expect the enrollment data on the next poll, so we
       * have nothing to do here */
      break;

    default:
      do_enroll_stop (dev,
                      NULL,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Unrecognised scan status code"));
      /* Stop iteration. */
      return;
    }
  enroll_iterate (dev);

  /* FIXME: need to extend protocol research to handle the case when
   * enrolment fails, e.g. you scan a different finger on each stage */
  /* FIXME: should do proper tracking of when we expect cmd0 results and
   * cmd2 results and enforce it */
}

static void
e_handle_resp02 (FpDevice *dev, unsigned char *data,
                 size_t data_len)
{
  FpPrint *print = NULL;
  GError *error = NULL;

  if (data_len < sizeof (scan_comp))
    {
      fp_err ("fingerprint data too short (%lu bytes)", data_len);
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO, "fingerprint data too short");
    }
  else if (memcmp (data, scan_comp, sizeof (scan_comp)) != 0)
    {
      fp_err ("unrecognised data prefix %x %x %x %x %x",
              data[0], data[1], data[2], data[3], data[4]);
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO, "fingerprint data has wrong prefix");
    }
  else
    {
      GVariant *fp_data;

      fpi_device_get_enroll_data (dev, &print);

      fp_data = g_variant_new_fixed_array (G_VARIANT_TYPE_BYTE,
                                           data + sizeof (scan_comp),
                                           data_len - sizeof (scan_comp),
                                           1);

      fpi_print_set_type (print, FPI_PRINT_RAW);
      g_object_set (print, "fpi-data", fp_data, NULL);
      g_object_ref (print);
    }

  do_enroll_stop (dev, print, error);
}

static void
enroll_iterate_msg_cb (FpDevice *dev,
                       enum read_msg_type msgtype, guint8 seq,
                       unsigned char subcmd,
                       unsigned char *data, size_t data_len,
                       void *user_data,
                       GError *error)
{
  if (error)
    {
      do_enroll_stop (dev, NULL, error);
      return;
    }
  else if (msgtype != READ_MSG_RESPONSE)
    {
      fp_err ("expected response, got %d seq=%x", msgtype, seq);
      do_enroll_stop (dev, NULL,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Expected message response, not command"));
      return;
    }
  if (subcmd == 0)
    {
      e_handle_resp00 (dev, data, data_len);
    }
  else if (subcmd == 2)
    {
      e_handle_resp02 (dev, data, data_len);
    }
  else
    {
      fp_err ("unexpected subcmd %d", subcmd);
      do_enroll_stop (dev, NULL,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Unexpected subcommand"));
    }
}

static void
enroll_iterate_cmd_cb (FpiUsbTransfer *transfer, FpDevice *device,
                       gpointer user_data, GError *error)
{
  if (error)
    do_enroll_stop (device, NULL, error);
  else
    read_msg_async (device, enroll_iterate_msg_cb, NULL);
}

static void
enroll_iterate (FpDevice *dev)
{
  FpiUsbTransfer *transfer;

  if (fpi_device_action_is_cancelled (dev))
    {
      do_enroll_stop (dev, NULL, g_error_new_literal (G_IO_ERROR, G_IO_ERROR_CANCELLED, "Cancelled"));
      return;
    }

  transfer = alloc_send_cmd28_transfer (dev, 0x00,
                                        poll_data, sizeof (poll_data));
  transfer->short_is_error = TRUE;

  fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, enroll_iterate_cmd_cb, NULL);
}

static void
enroll_started (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  if (error)
    do_enroll_stop (dev, NULL, error);
  else
    enroll_iterate (dev);

}

static void
enroll (FpDevice *dev)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  /* do_init state machine first */
  FpiSsm *ssm = fpi_ssm_new (dev, enroll_start_sm_run_state,
                             ENROLL_START_NUM_STATES);

  upekdev->enroll_passed = FALSE;
  upekdev->enroll_stage = 0;
  fpi_ssm_start (ssm, enroll_started);
}

typedef struct
{
  GError *error;
} VerifyStopData;

static void
verify_stop_data_free (VerifyStopData *data)
{
  g_clear_error (&data->error);
  g_free (data);
}

static void
verify_stop_deinit_cb (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  VerifyStopData *data = fpi_ssm_get_data (ssm);

  if (error)
    fp_warn ("Error deinitializing: %s", error->message);

  if (data->error)
    fpi_device_verify_complete (dev, g_steal_pointer (&data->error));
  else
    fpi_device_verify_complete (dev, g_steal_pointer (&error));

  g_clear_error (&error);
}

static void
do_verify_stop (FpDevice *dev, FpiMatchResult res, GError *error)
{
  VerifyStopData *data = g_new0 (VerifyStopData, 1);
  FpiSsm *ssm = deinitsm_new (dev, data);

  /* Report the error immediately if possible, otherwise delay it. */
  if (error && error->domain == FP_DEVICE_RETRY)
    fpi_device_verify_report (dev, res, NULL, error);
  else
    data->error = error;

  fpi_ssm_start (ssm, verify_stop_deinit_cb);
  fpi_ssm_set_data (ssm, data, (GDestroyNotify) verify_stop_data_free);
}

static const unsigned char verify_hdr[] = {
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0xd4, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00
};

enum {
  VERIFY_RUN_INITSM = 0,
  VERIFY_INIT,
  VERIFY_NUM_STATES,
};

static void
verify_start_sm_run_state (FpiSsm *ssm, FpDevice *dev)
{
  FpPrint *print;

  g_autoptr(GVariant) fp_data = NULL;
  FpiUsbTransfer *transfer;
  gsize data_len;
  const guint8 *data;
  guint8 *msg;
  gsize msg_len;

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case VERIFY_RUN_INITSM:;
      FpiSsm *initsm = initsm_new (dev);
      fpi_ssm_start_subsm (ssm, initsm);
      break;

    case VERIFY_INIT:
      fpi_device_get_verify_data (dev, &print);
      g_object_get (print, "fpi-data", &fp_data, NULL);

      data = g_variant_get_fixed_array (fp_data, &data_len, 1);

      msg_len = sizeof (verify_hdr) + data_len;
      msg = g_malloc (msg_len);

      memcpy (msg, verify_hdr, sizeof (verify_hdr));
      memcpy (msg + sizeof (verify_hdr), data, data_len);

      transfer = alloc_send_cmd28_transfer (dev, 0x03, data, data_len);

      g_free (msg);

      transfer->short_is_error = TRUE;
      transfer->ssm = ssm;
      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, fpi_ssm_usb_transfer_cb, NULL);

      break;
    }
}

static void verify_iterate (FpDevice *dev);

static void
v_handle_resp00 (FpDevice *dev, unsigned char *data,
                 size_t data_len)
{
  unsigned char status;
  GError *error = NULL;

  if (data_len != 14)
    {
      fp_warn ("received 3001 poll response of %lu bytes?", data_len);
      error = fpi_device_error_new (FP_DEVICE_ERROR_PROTO);
      goto out;
    }

  status = data[5];
  fp_dbg ("poll result = %02x", status);

  /* These codes indicate that we're waiting for a finger scan, so poll
   * again */
  switch (status)
    {
    case 0x0c:     /* no news, poll again */
      break;

    case 0x20:
      fp_dbg ("processing scan for verification");
      break;

    case 0x00:
      fp_dbg ("good image");
      break;

    case 0x1c:     /* FIXME what does this one mean? */
    case 0x0b:     /* FIXME what does this one mean? */
    case 0x23:     /* FIXME what does this one mean? */
      error = fpi_device_retry_new (FP_DEVICE_RETRY_GENERAL);
      break;

    case 0x0f:     /* scan taking too long, remove finger and try again */
      error = fpi_device_retry_new (FP_DEVICE_RETRY_REMOVE_FINGER);
      break;

    case 0x1e:     /* swipe too short */
      error = fpi_device_retry_new (FP_DEVICE_RETRY_TOO_SHORT);
      break;

    case 0x24:     /* finger not centered */
      error = fpi_device_retry_new (FP_DEVICE_RETRY_CENTER_FINGER);
      break;

    default:
      fp_err ("unrecognised verify status code %02x", status);
      error = fpi_device_retry_new (FP_DEVICE_RETRY_GENERAL);
    }

out:
  if (error)
    do_verify_stop (dev, FPI_MATCH_ERROR, error);
  else
    verify_iterate (dev);
}

static void
v_handle_resp03 (FpDevice *dev, unsigned char *data,
                 size_t data_len)
{
  FpiMatchResult r;
  GError *error = NULL;

  if (data_len < 2)
    {
      fp_warn ("verify result abnormally short!");
      r = FPI_MATCH_ERROR;
      error = fpi_device_error_new (FP_DEVICE_ERROR_PROTO);
    }
  else if (data[0] != 0x12)
    {
      fp_warn ("unexpected verify header byte %02x", data[0]);
      r = FPI_MATCH_ERROR;
      error = fpi_device_error_new (FP_DEVICE_ERROR_PROTO);
    }
  else if (data[1] == 0x00)
    {
      r = FPI_MATCH_FAIL;
    }
  else if (data[1] == 0x01)
    {
      r = FPI_MATCH_SUCCESS;
    }
  else
    {
      fp_warn ("unrecognised verify result %02x", data[1]);
      r = FPI_MATCH_ERROR;
      error = fpi_device_error_new (FP_DEVICE_ERROR_PROTO);
    }
  do_verify_stop (dev, r, error);
}

static void
verify_rd2800_cb (FpDevice *dev, enum read_msg_type msgtype,
                  guint8 seq, unsigned char subcmd,
                  unsigned char *data, size_t data_len,
                  void *user_data,
                  GError *error)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (error)
    {
      do_verify_stop (dev, FPI_MATCH_ERROR, error);
      return;
    }

  if (msgtype != READ_MSG_RESPONSE)
    {
      fp_warn ("expected response, got %d seq=%x", msgtype, seq);
      do_verify_stop (dev,
                      FPI_MATCH_ERROR,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Expected message response"));
      return;
    }

  if (seq != upekdev->seq)
    {
      fp_warn ("expected response to cmd seq=%02x, got response to %02x",
               upekdev->seq, seq);
      do_verify_stop (dev,
                      FPI_MATCH_ERROR,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Response hat wrong command sequence"));
      return;
    }

  if (subcmd == 0)
    {
      v_handle_resp00 (dev, data, data_len);
    }
  else if (subcmd == 3)
    {
      v_handle_resp03 (dev, data, data_len);
    }
  else
    {
      do_verify_stop (dev,
                      FPI_MATCH_ERROR,
                      fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO,
                                                "Response had wrong subcommand type"));
    }
}

static void
verify_wr2800_cb (FpiUsbTransfer *transfer, FpDevice *device,
                  gpointer user_data, GError *error)
{
  if (error)
    {
      do_verify_stop (device,
                      FPI_MATCH_ERROR,
                      error);
    }
  else
    {
      read_msg_async (device, verify_rd2800_cb, NULL);
    }
}

static void
verify_iterate (FpDevice *dev)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (fpi_device_action_is_cancelled (dev))
    {
      do_verify_stop (dev, FPI_MATCH_ERROR, g_error_new_literal (G_IO_ERROR, G_IO_ERROR_CANCELLED, "Cancelled"));
      return;
    }

  /* FIXME: this doesn't flow well, should the first cmd be moved from
   * verify init to here? */
  if (upekdev->first_verify_iteration)
    {
      read_msg_async (dev, verify_rd2800_cb, NULL);
      upekdev->first_verify_iteration = FALSE;
    }
  else
    {
      FpiUsbTransfer *transfer = alloc_send_cmd28_transfer (dev,
                                                            0x00, poll_data, sizeof (poll_data));
      transfer->short_is_error = TRUE;

      fpi_usb_transfer_submit (transfer, TIMEOUT, NULL, verify_wr2800_cb, NULL);
    }
}

static void
verify_started (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpiDeviceUpekts *upekdev = FPI_DEVICE_UPEKTS (dev);

  if (error)
    {
      do_verify_stop (dev, FPI_MATCH_ERROR, error);
      return;
    }

  upekdev->first_verify_iteration = TRUE;
  verify_iterate (dev);

}

static void
verify (FpDevice *dev)
{
  FpiSsm *ssm = fpi_ssm_new (dev, verify_start_sm_run_state, VERIFY_NUM_STATES);

  fpi_ssm_start (ssm, verify_started);
}

static const FpIdEntry id_table[] = {
  { .vid = 0x0483, .pid = 0x2016, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },       /* terminating entry */
};

static void
fpi_device_upekts_init (FpiDeviceUpekts *self)
{
}

static void
fpi_device_upekts_class_init (FpiDeviceUpektsClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);

  dev_class->id = FP_COMPONENT;
  dev_class->full_name = "UPEK TouchStrip";

  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;
  dev_class->id_table = id_table;
  dev_class->nr_enroll_stages = 3;

  dev_class->open = dev_init;
  dev_class->close = dev_exit;
  dev_class->verify = verify;
  dev_class->enroll = enroll;
  /* dev_class->cancel = cancel; */
}
