/*
 * Digital Persona U.are.U 4000/4000B/4500 driver for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2012 Timo Teräs <timo.teras@iki.fi>
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

#define FP_COMPONENT "uru4000"

#include <nss.h>
#include <pk11pub.h>

#include "drivers_api.h"

#define EP_INTR (1 | FPI_USB_ENDPOINT_IN)
#define EP_DATA (2 | FPI_USB_ENDPOINT_IN)
#define USB_RQ 0x04
#define CTRL_IN (LIBUSB_REQUEST_TYPE_VENDOR | FPI_USB_ENDPOINT_IN)
#define CTRL_OUT (LIBUSB_REQUEST_TYPE_VENDOR | FPI_USB_ENDPOINT_OUT)
#define CTRL_TIMEOUT 5000
#define BULK_TIMEOUT 5000
#define IRQ_LENGTH 64
#define CR_LENGTH 16

#define IMAGE_HEIGHT 290
#define IMAGE_WIDTH 384

#define ENC_THRESHOLD 5000

enum {
  IRQDATA_SCANPWR_ON = 0x56aa,
  IRQDATA_FINGER_ON = 0x0101,
  IRQDATA_FINGER_OFF = 0x0200,
  IRQDATA_DEATH = 0x0800,
};

enum {
  REG_HWSTAT = 0x07,
  REG_SCRAMBLE_DATA_INDEX = 0x33,
  REG_SCRAMBLE_DATA_KEY = 0x34,
  REG_MODE = 0x4e,
  REG_DEVICE_INFO = 0xf0,
  /* firmware starts at 0x100 */
  REG_RESPONSE = 0x2000,
  REG_CHALLENGE = 0x2010,
};

enum {
  MODE_INIT = 0x00,
  MODE_AWAIT_FINGER_ON = 0x10,
  MODE_AWAIT_FINGER_OFF = 0x12,
  MODE_CAPTURE = 0x20,
  MODE_CAPTURE_AUX = 0x30,
  MODE_OFF = 0x70,
  MODE_READY = 0x80,
};

enum {
  MS_KBD,
  MS_INTELLIMOUSE,
  MS_STANDALONE,
  MS_STANDALONE_V2,
  DP_URU4000,
  DP_URU4000B,
};

static const struct uru4k_dev_profile
{
  const char *name;
  gboolean    auth_cr;
  gboolean    image_not_flipped;
} uru4k_dev_info[] = {
  [MS_KBD] = {
    .name = "Microsoft Keyboard with Fingerprint Reader",
    .auth_cr = FALSE,
  },
  [MS_INTELLIMOUSE] = {
    .name = "Microsoft Wireless IntelliMouse with Fingerprint Reader",
    .auth_cr = FALSE,
  },
  [MS_STANDALONE] = {
    .name = "Microsoft Fingerprint Reader",
    .auth_cr = FALSE,
  },
  [MS_STANDALONE_V2] = {
    .name = "Microsoft Fingerprint Reader v2",
    .auth_cr = TRUE,
  },
  [DP_URU4000] = {
    .name = "Digital Persona U.are.U 4000",
    .auth_cr = FALSE,
  },
  [DP_URU4000B] = {
    .name = "Digital Persona U.are.U 4000B",
    .auth_cr = FALSE,
    .image_not_flipped = TRUE, /* See comment in the code where it is used. */
  },
};

typedef void (*irq_cb_fn)(FpImageDevice *dev,
                          GError        *error,
                          uint16_t       type,
                          void          *user_data);
typedef void (*irqs_stopped_cb_fn)(FpImageDevice *dev);

struct _FpiDeviceUru4000
{
  FpImageDevice                   parent;

  const struct uru4k_dev_profile *profile;
  uint8_t                         interface;
  FpiImageDeviceState             activate_state;
  unsigned char                   last_reg_rd[16];
  unsigned char                   last_hwstat;

  GCancellable                   *irq_cancellable;
  FpiUsbTransfer                 *img_transfer;
  void                           *img_data;
  int                             img_data_actual_length;
  uint16_t                        img_lines_done, img_block;
  uint32_t                        img_enc_seed;

  irq_cb_fn                       irq_cb;
  void                           *irq_cb_data;
  irqs_stopped_cb_fn              irqs_stopped_cb;

  int                             rebootpwr_ctr;
  int                             powerup_ctr;
  unsigned char                   powerup_hwstat;

  int                             scanpwr_irq_timeouts;
  GSource                        *scanpwr_irq_timeout;

  int                             fwfixer_offset;
  unsigned char                   fwfixer_value;

  CK_MECHANISM_TYPE               cipher;
  PK11SlotInfo                   *slot;
  PK11SymKey                     *symkey;
  SECItem                        *param;
};
G_DECLARE_FINAL_TYPE (FpiDeviceUru4000, fpi_device_uru4000, FPI, DEVICE_URU4000,
                      FpImageDevice);
G_DEFINE_TYPE (FpiDeviceUru4000, fpi_device_uru4000, FP_TYPE_IMAGE_DEVICE);

/* For 2nd generation MS devices */
static const unsigned char crkey[] = {
  0x79, 0xac, 0x91, 0x79, 0x5c, 0xa1, 0x47, 0x8e,
  0x98, 0xe0, 0x0f, 0x3c, 0x59, 0x8f, 0x5f, 0x4b,
};

/***** REGISTER I/O *****/

static void
write_regs (FpImageDevice *dev, uint16_t first_reg,
            uint16_t num_regs, unsigned char *values,
            FpiUsbTransferCallback callback,
            void *user_data)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_control (transfer,
                                 G_USB_DEVICE_DIRECTION_HOST_TO_DEVICE,
                                 G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                 G_USB_DEVICE_RECIPIENT_DEVICE,
                                 USB_RQ, first_reg, 0,
                                 num_regs);
  memcpy (transfer->buffer, values, num_regs);
  fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL, callback, user_data);
}

static void
write_reg (FpImageDevice *dev, uint16_t reg,
           unsigned char value,
           FpiUsbTransferCallback callback,
           void *user_data)
{
  write_regs (dev, reg, 1, &value, callback, user_data);
}

static void
read_regs (FpImageDevice *dev, uint16_t first_reg,
           uint16_t num_regs,
           FpiUsbTransferCallback callback,
           void *user_data)
{
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  fpi_usb_transfer_fill_control (transfer,
                                 G_USB_DEVICE_DIRECTION_DEVICE_TO_HOST,
                                 G_USB_DEVICE_REQUEST_TYPE_VENDOR,
                                 G_USB_DEVICE_RECIPIENT_DEVICE,
                                 USB_RQ, first_reg, 0, num_regs);
  fpi_usb_transfer_submit (transfer, CTRL_TIMEOUT, NULL, callback, user_data);
}

/*
 * HWSTAT
 *
 * This register has caused me a lot of headaches. It pretty much defines
 * code flow, and if you don't get it right, the pretty lights don't come on.
 * I think the situation is somewhat complicated by the fact that writing it
 * doesn't affect the read results in the way you'd expect -- but then again
 * it does have some obvious effects. Here's what we know
 *
 * BIT 7: LOW POWER MODE
 * When this bit is set, the device is partially turned off or something. Some
 * things, like firmware upload, need to be done in this state. But generally
 * we want to clear this bit during late initialization, which can sometimes
 * be tricky.
 *
 * BIT 2: SOMETHING WENT WRONG
 * Not sure about this, but see the init function, as when we detect it,
 * we reboot the device. Well, we mess with hwstat until this evil bit gets
 * cleared.
 *
 * BIT 1: IRQ PENDING
 * Just had a brainwave. This bit is set when the device is trying to deliver
 * an interrupt to the host. Maybe?
 */

static void
response_cb (FpiUsbTransfer *transfer, FpDevice *dev, void *user_data, GError *error)
{
  /* NOTE: We could use the SSM function instead if we attached the ssm to the transfer! */
  FpiSsm *ssm = user_data;

  if (!error)
    fpi_ssm_next_state (ssm);
  else
    fpi_ssm_mark_failed (ssm, error);
}

static void
challenge_cb (FpiUsbTransfer *transfer, FpDevice *dev, void *user_data, GError *error)
{
  FpiSsm *ssm = user_data;
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);
  unsigned char respdata[CR_LENGTH];
  PK11Context *ctx;
  int outlen;

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
      return;
    }

  /* submit response */
  /* produce response from challenge */
  ctx = PK11_CreateContextBySymKey (self->cipher, CKA_ENCRYPT,
                                    self->symkey, self->param);
  if (PK11_CipherOp (ctx, respdata, &outlen, CR_LENGTH, transfer->buffer, CR_LENGTH) != SECSuccess ||
      PK11_Finalize (ctx) != SECSuccess)
    {
      fp_err ("Failed to encrypt challenge data");
      error = fpi_device_error_new_msg (FP_DEVICE_ERROR_PROTO, "Failed to encrypt challenge data");
    }
  PK11_DestroyContext (ctx, PR_TRUE);

  if (!error)
    write_regs (FP_IMAGE_DEVICE (dev), REG_RESPONSE, CR_LENGTH, respdata, response_cb, ssm);
  else
    fpi_ssm_mark_failed (ssm, error);
}

/*
 * 2nd generation MS devices added an AES-based challenge/response
 * authentication scheme, where the device challenges the authenticity of the
 * driver.
 */
static void
sm_do_challenge_response (FpiSsm        *ssm,
                          FpImageDevice *dev)
{
  G_DEBUG_HERE ();
  read_regs (dev, REG_CHALLENGE, CR_LENGTH, challenge_cb, ssm);
}

/***** INTERRUPT HANDLING *****/

#define IRQ_HANDLER_IS_RUNNING(urudev) ((urudev)->irq_cancellable)

static void start_irq_handler (FpImageDevice *dev);

static void
irq_handler (FpiUsbTransfer *transfer,
             FpDevice       *dev,
             void           *user_data,
             GError         *error)
{
  FpImageDevice *imgdev = FP_IMAGE_DEVICE (dev);
  FpiDeviceUru4000 *urudev = FPI_DEVICE_URU4000 (dev);
  unsigned char *data = transfer->buffer;
  uint16_t type;

  g_clear_object (&urudev->irq_cancellable);

  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
      fp_dbg ("cancelled");
      if (urudev->irqs_stopped_cb)
        urudev->irqs_stopped_cb (imgdev);
      urudev->irqs_stopped_cb = NULL;
      return;
    }
  else if (error)
    {
      if (urudev->irq_cb)
        {
          urudev->irq_cb (imgdev, error, 0, urudev->irq_cb_data);
        }
      else
        {
          fp_dbg ("ignoring interrupt error: %s", error->message);
          g_clear_error (&error);
        }
      return;
    }

  start_irq_handler (imgdev);

  type = GUINT16_FROM_BE (*((uint16_t *) data));
  fp_dbg ("recv irq type %04x", type);

  /* The 0800 interrupt seems to indicate imminent failure (0 bytes transfer)
   * of the next scan. It still appears on occasion. */
  if (type == IRQDATA_DEATH)
    fp_warn ("oh no! got the interrupt OF DEATH! expect things to go bad");

  if (urudev->irq_cb)
    urudev->irq_cb (imgdev, NULL, type, urudev->irq_cb_data);
  else
    fp_dbg ("ignoring interrupt");
}

static void
start_irq_handler (FpImageDevice *dev)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);
  FpiUsbTransfer *transfer;

  g_assert (self->irq_cancellable == NULL);
  self->irq_cancellable = g_cancellable_new ();
  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->ssm = NULL;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_fill_bulk (transfer,
                              EP_INTR,
                              IRQ_LENGTH);
  fpi_usb_transfer_submit (transfer, 0, self->irq_cancellable, irq_handler, NULL);
}

static void
stop_irq_handler (FpImageDevice *dev, irqs_stopped_cb_fn cb)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);

  if (self->irq_cancellable)
    {
      g_cancellable_cancel (self->irq_cancellable);
      self->irqs_stopped_cb = cb;
    }
  else
    {
      cb (dev);
    }
}

/***** STATE CHANGING *****/

static void execute_state_change (FpImageDevice *dev);

static void
finger_presence_irq_cb (FpImageDevice *dev,
                        GError        *error,
                        uint16_t       type,
                        void          *user_data)
{
  if (error)
    fpi_image_device_session_error (dev, error);
  else if (type == IRQDATA_FINGER_ON)
    fpi_image_device_report_finger_status (dev, TRUE);
  else if (type == IRQDATA_FINGER_OFF)
    fpi_image_device_report_finger_status (dev, FALSE);
  else
    fp_warn ("ignoring unexpected interrupt %04x", type);
}

static void
change_state_write_reg_cb (FpiUsbTransfer *transfer,
                           FpDevice       *dev,
                           void           *user_data,
                           GError         *error)
{
  if (error)
    fpi_image_device_session_error (FP_IMAGE_DEVICE (dev), error);
}

static void
dev_change_state (FpImageDevice *dev, FpiImageDeviceState state)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);

  self->activate_state = state;
  if (self->img_transfer != NULL)
    return;

  execute_state_change (dev);
}

/***** GENERIC STATE MACHINE HELPER FUNCTIONS *****/

static void
sm_write_reg_cb (FpiUsbTransfer *transfer,
                 FpDevice       *dev,
                 void           *user_data,
                 GError         *error)
{
  FpiSsm *ssm = user_data;

  if (error)
    fpi_ssm_mark_failed (ssm, error);
  else
    fpi_ssm_next_state (ssm);
}

static void
sm_write_regs (FpiSsm        *ssm,
               FpImageDevice *dev,
               uint16_t       first_reg,
               uint16_t       num_regs,
               void          *data)
{
  write_regs (dev, first_reg, num_regs, data, sm_write_reg_cb, ssm);
}

static void
sm_write_reg (FpiSsm        *ssm,
              FpImageDevice *dev,
              uint16_t       reg,
              unsigned char  value)
{
  sm_write_regs (ssm, dev, reg, 1, &value);
}

static void
sm_read_reg_cb (FpiUsbTransfer *transfer,
                FpDevice       *dev,
                void           *user_data,
                GError         *error)
{
  FpiSsm *ssm = user_data;
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
    }
  else
    {
      memcpy (self->last_reg_rd, transfer->buffer, transfer->actual_length);
      fp_dbg ("reg value %x", self->last_reg_rd[0]);
      fpi_ssm_next_state (ssm);
    }
}

#define member_size(type, member) sizeof (((type *) 0)->member)

static void
sm_read_regs (FpiSsm        *ssm,
              FpImageDevice *dev,
              uint16_t       reg,
              uint16_t       num_regs)
{
  g_assert (num_regs <= member_size (FpiDeviceUru4000, last_reg_rd));

  fp_dbg ("read %d regs at %x", num_regs, reg);
  read_regs (dev, reg, num_regs, sm_read_reg_cb, ssm);
}

static void
sm_read_reg (FpiSsm        *ssm,
             FpImageDevice *dev,
             uint16_t       reg)
{
  sm_read_regs (ssm, dev, reg, 1);
}

static void
sm_set_hwstat (FpiSsm        *ssm,
               FpImageDevice *dev,
               unsigned char  value)
{
  fp_dbg ("set %02x", value);
  sm_write_reg (ssm, dev, REG_HWSTAT, value);
}

/***** IMAGING LOOP *****/

enum imaging_states {
  IMAGING_CAPTURE,
  IMAGING_SEND_INDEX,
  IMAGING_READ_KEY,
  IMAGING_DECODE,
  IMAGING_REPORT_IMAGE,
  IMAGING_NUM_STATES
};

struct uru4k_image
{
  uint8_t  unknown_00[4];
  uint16_t num_lines;
  uint8_t  key_number;
  uint8_t  unknown_07[9];
  struct
  {
    uint8_t flags;
    uint8_t num_lines;
  } block_info[15];
  uint8_t unknown_2E[18];
  uint8_t data[IMAGE_HEIGHT][IMAGE_WIDTH];
};

static void
image_transfer_cb (FpiUsbTransfer *transfer, FpDevice *dev,
                   gpointer user_data, GError *error)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);
  FpiSsm *ssm = transfer->ssm;

  if (error)
    {
      fp_dbg ("error");
      fpi_ssm_mark_failed (ssm, error);
    }
  else
    {
      self->img_data = g_memdup (transfer->buffer, sizeof (struct uru4k_image));
      self->img_data_actual_length = transfer->actual_length;
      fpi_ssm_next_state (ssm);
    }
}

enum {
  BLOCKF_CHANGE_KEY       = 0x80,
  BLOCKF_NO_KEY_UPDATE    = 0x04,
  BLOCKF_ENCRYPTED                = 0x02,
  BLOCKF_NOT_PRESENT      = 0x01,
};

static uint32_t
update_key (uint32_t key)
{
  /* linear feedback shift register
   * taps at bit positions 1 3 4 7 11 13 20 23 26 29 32 */
  uint32_t bit = key & 0x9248144d;

  bit ^= bit << 16;
  bit ^= bit << 8;
  bit ^= bit << 4;
  bit ^= bit << 2;
  bit ^= bit << 1;
  return (bit & 0x80000000) | (key >> 1);
}

static uint32_t
do_decode (uint8_t *data, int num_bytes, uint32_t key)
{
  uint8_t xorbyte;
  int i;

  for (i = 0; i < num_bytes - 1; i++)
    {
      /* calculate xor byte and update key */
      xorbyte  = ((key >>  4) & 1) << 0;
      xorbyte |= ((key >>  8) & 1) << 1;
      xorbyte |= ((key >> 11) & 1) << 2;
      xorbyte |= ((key >> 14) & 1) << 3;
      xorbyte |= ((key >> 18) & 1) << 4;
      xorbyte |= ((key >> 21) & 1) << 5;
      xorbyte |= ((key >> 24) & 1) << 6;
      xorbyte |= ((key >> 29) & 1) << 7;
      key = update_key (key);

      /* decrypt data */
      data[i] = data[i + 1] ^ xorbyte;
    }

  /* the final byte is implicitly zero */
  data[i] = 0;
  return update_key (key);
}

static int
calc_dev2 (struct uru4k_image *img)
{
  uint8_t *b[2] = { NULL, NULL };
  int res = 0, mean = 0, i, r, j, idx;

  for (i = r = idx = 0; i < G_N_ELEMENTS (img->block_info) && idx < 2; i++)
    {
      if (img->block_info[i].flags & BLOCKF_NOT_PRESENT)
        continue;
      for (j = 0; j < img->block_info[i].num_lines && idx < 2; j++)
        b[idx++] = img->data[r++];
    }
  if (!b[0] || !b[1])
    {
      fp_dbg ("NULL! %p %p", b[0], b[1]);
      return 0;
    }
  for (i = 0; i < IMAGE_WIDTH; i++)
    mean += (int) b[0][i] + (int) b[1][i];

  mean /= IMAGE_WIDTH;

  for (i = 0; i < IMAGE_WIDTH; i++)
    {
      int dev = (int) b[0][i] + (int) b[1][i] - mean;
      res += dev * dev;
    }

  return res / IMAGE_WIDTH;
}

static void
imaging_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (_dev);
  struct uru4k_image *img = self->img_data;
  FpImage *fpimg;
  uint32_t key;
  uint8_t flags, num_lines;
  int i, r, to, dev2;
  unsigned char buf[5];

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case IMAGING_CAPTURE:
      self->img_lines_done = 0;
      self->img_block = 0;
      fpi_usb_transfer_submit (fpi_usb_transfer_ref (self->img_transfer),
                               0,
                               NULL,
                               image_transfer_cb,
                               NULL);

      break;

    case IMAGING_SEND_INDEX:
      fp_dbg ("hw header lines %d", img->num_lines);

      if (img->num_lines >= IMAGE_HEIGHT ||
          self->img_data_actual_length < img->num_lines * IMAGE_WIDTH + 64)
        {
          fp_err ("bad captured image (%d lines) or size mismatch %d < %d",
                  img->num_lines,
                  self->img_data_actual_length,
                  img->num_lines * IMAGE_WIDTH + 64);
          fpi_ssm_jump_to_state (ssm, IMAGING_CAPTURE);
          return;
        }

      /* Detect whether image is encrypted (by checking how noisy it is) */
      dev2 = calc_dev2 (img);
      fp_dbg ("dev2: %d", dev2);
      if (dev2 < ENC_THRESHOLD)
        {
          fpi_ssm_jump_to_state (ssm, IMAGING_REPORT_IMAGE);
          return;
        }
      fp_info ("image seems to be encrypted");

      buf[0] = img->key_number;
      buf[1] = self->img_enc_seed;
      buf[2] = self->img_enc_seed >> 8;
      buf[3] = self->img_enc_seed >> 16;
      buf[4] = self->img_enc_seed >> 24;
      sm_write_regs (ssm, dev, REG_SCRAMBLE_DATA_INDEX, 5, buf);
      break;

    case IMAGING_READ_KEY:
      sm_read_regs (ssm, dev, REG_SCRAMBLE_DATA_KEY, 4);
      break;

    case IMAGING_DECODE:
      key  = self->last_reg_rd[0];
      key |= self->last_reg_rd[1] << 8;
      key |= self->last_reg_rd[2] << 16;
      key |= self->last_reg_rd[3] << 24;
      key ^= self->img_enc_seed;

      fp_dbg ("encryption id %02x -> key %08x", img->key_number, key);
      while (self->img_block < G_N_ELEMENTS (img->block_info) &&
             self->img_lines_done < img->num_lines)
        {
          flags = img->block_info[self->img_block].flags;
          num_lines = img->block_info[self->img_block].num_lines;
          if (num_lines == 0)
            break;

          fp_dbg ("%d %02x %d", self->img_block, flags,
                  num_lines);
          if (flags & BLOCKF_CHANGE_KEY)
            {
              fp_dbg ("changing encryption keys.");
              img->block_info[self->img_block].flags &= ~BLOCKF_CHANGE_KEY;
              img->key_number++;
              self->img_enc_seed = rand ();
              fpi_ssm_jump_to_state (ssm, IMAGING_SEND_INDEX);
              return;
            }
          switch (flags & (BLOCKF_NO_KEY_UPDATE | BLOCKF_ENCRYPTED))
            {
            case BLOCKF_ENCRYPTED:
              fp_dbg ("decoding %d lines", num_lines);
              key = do_decode (&img->data[self->img_lines_done][0],
                               IMAGE_WIDTH * num_lines, key);
              break;

            case 0:
              fp_dbg ("skipping %d lines", num_lines);
              for (r = 0; r < IMAGE_WIDTH * num_lines; r++)
                key = update_key (key);
              break;
            }
          if ((flags & BLOCKF_NOT_PRESENT) == 0)
            self->img_lines_done += num_lines;
          self->img_block++;
        }
      fpi_ssm_next_state (ssm);
      break;

    case IMAGING_REPORT_IMAGE:
      fpimg = fp_image_new (IMAGE_WIDTH, IMAGE_HEIGHT);

      to = r = 0;
      for (i = 0; i < G_N_ELEMENTS (img->block_info) && r < img->num_lines; i++)
        {
          flags = img->block_info[i].flags;
          num_lines = img->block_info[i].num_lines;
          if (num_lines == 0)
            break;
          memcpy (&fpimg->data[to], &img->data[r][0],
                  num_lines * IMAGE_WIDTH);
          if (!(flags & BLOCKF_NOT_PRESENT))
            r += num_lines;
          to += num_lines * IMAGE_WIDTH;
        }

      fpimg->flags = FPI_IMAGE_COLORS_INVERTED;
      /* NOTE: For some reason all but U4000B (or rather U4500?) flipped the
       * image, we retain this behaviour here, but it is not clear whether it
       * is correct.
       * It may be that there are different models with the same USB ID that
       * behave differently.
       */
      if (self->profile->image_not_flipped)
        fpimg->flags |= FPI_IMAGE_V_FLIPPED | FPI_IMAGE_H_FLIPPED;
      fpi_image_device_image_captured (dev, fpimg);

      if (self->activate_state == FPI_IMAGE_DEVICE_STATE_CAPTURE)
        fpi_ssm_jump_to_state (ssm, IMAGING_CAPTURE);
      else
        fpi_ssm_mark_completed (ssm);
      break;
    }
}

static void
imaging_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);


  /* Report error before exiting imaging loop - the error handler
   * can request state change, which needs to be postponed to end of
   * this function. */
  if (error)
    fpi_image_device_session_error (FP_IMAGE_DEVICE (dev), error);

  g_clear_pointer (&self->img_transfer, fpi_usb_transfer_unref);

  g_free (self->img_data);
  self->img_data = NULL;
  self->img_data_actual_length = 0;

  execute_state_change (FP_IMAGE_DEVICE (dev));
}

/***** INITIALIZATION *****/

/* After closing an app and setting hwstat to 0x80, my ms keyboard gets in a
 * confused state and returns hwstat 0x85. On next app run, we don't get the
 * 56aa interrupt. This is the best way I've found to fix it: mess around
 * with hwstat until it starts returning more recognisable values. This
 * doesn't happen on my other devices: uru4000, uru4000b, ms fp rdr v2
 *
 * The windows driver copes with this OK, but then again it uploads firmware
 * right after reading the 0x85 hwstat, allowing some time to pass before it
 * attempts to tweak hwstat again...
 *
 * This is implemented with a reboot power state machine. the ssm runs during
 * initialization if bits 2 and 7 are set in hwstat. it masks off the 4 high
 * hwstat bits then checks that bit 1 is set. if not, it pauses before reading
 * hwstat again. machine completes when reading hwstat shows bit 1 is set,
 * and fails after 100 tries. */

enum rebootpwr_states {
  REBOOTPWR_SET_HWSTAT = 0,
  REBOOTPWR_GET_HWSTAT,
  REBOOTPWR_CHECK_HWSTAT,
  REBOOTPWR_PAUSE,
  REBOOTPWR_NUM_STATES,
};

static void
rebootpwr_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case REBOOTPWR_SET_HWSTAT:
      self->rebootpwr_ctr = 100;
      sm_set_hwstat (ssm, dev, self->last_hwstat & 0xf);
      break;

    case REBOOTPWR_GET_HWSTAT:
      sm_read_reg (ssm, dev, REG_HWSTAT);
      break;

    case REBOOTPWR_CHECK_HWSTAT:
      self->last_hwstat = self->last_reg_rd[0];
      if (self->last_hwstat & 0x1)
        fpi_ssm_mark_completed (ssm);
      else
        fpi_ssm_next_state (ssm);
      break;

    case REBOOTPWR_PAUSE:
      if (!--self->rebootpwr_ctr)
        {
          fp_err ("could not reboot device power");
          fpi_ssm_mark_failed (ssm,
                               fpi_device_error_new_msg (FP_DEVICE_ERROR,
                                                         "Could not reboot device"));
        }
      else
        {
          fpi_ssm_jump_to_state_delayed (ssm, 10, REBOOTPWR_GET_HWSTAT, NULL);
        }
      break;
    }
}

/* After messing with the device firmware in its low-power state, we have to
 * power it back up and wait for interrupt notification. It's not quite as easy
 * as that: the combination of both modifying firmware *and* doing C-R auth on
 * my ms fp v2 device causes us not to get the 56aa interrupt and
 * for the hwstat write not to take effect. We have to loop a few times,
 * authenticating each time, until the device wakes up.
 *
 * This is implemented as the powerup state machine below. Pseudo-code:

        status = get_hwstat();
        for (i = 0; i < 100; i++) {
                set_hwstat(status & 0xf);
                if ((get_hwstat() & 0x80) == 0)
                        break;

                usleep(10000);
                if (need_auth_cr)
                        auth_cr();
        }

        if (tmp & 0x80)
                error("could not power up device");

 */

enum powerup_states {
  POWERUP_INIT = 0,
  POWERUP_SET_HWSTAT,
  POWERUP_GET_HWSTAT,
  POWERUP_CHECK_HWSTAT,
  POWERUP_PAUSE,
  POWERUP_CHALLENGE_RESPONSE,
  POWERUP_CHALLENGE_RESPONSE_SUCCESS,
  POWERUP_NUM_STATES,
};

static void
powerup_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case POWERUP_INIT:
      self->powerup_ctr = 100;
      self->powerup_hwstat = self->last_hwstat & 0xf;
      fpi_ssm_next_state (ssm);
      break;

    case POWERUP_SET_HWSTAT:
      sm_set_hwstat (ssm, dev, self->powerup_hwstat);
      break;

    case POWERUP_GET_HWSTAT:
      sm_read_reg (ssm, dev, REG_HWSTAT);
      break;

    case POWERUP_CHECK_HWSTAT:
      self->last_hwstat = self->last_reg_rd[0];
      if ((self->last_reg_rd[0] & 0x80) == 0)
        fpi_ssm_mark_completed (ssm);
      else
        fpi_ssm_next_state (ssm);
      break;

    case POWERUP_PAUSE:
      if (!--self->powerup_ctr)
        {
          fp_err ("could not power device up");
          fpi_ssm_mark_failed (ssm,
                               fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                         "could not power device up"));
        }
      else if (!self->profile->auth_cr)
        {
          fpi_ssm_jump_to_state_delayed (ssm, POWERUP_SET_HWSTAT, 10, NULL);
        }
      else
        {
          fpi_ssm_next_state_delayed (ssm, 10, NULL);
        }
      break;

    case POWERUP_CHALLENGE_RESPONSE:
      sm_do_challenge_response (ssm, dev);
      break;

    case POWERUP_CHALLENGE_RESPONSE_SUCCESS:
      fpi_ssm_jump_to_state (ssm, POWERUP_SET_HWSTAT);
      break;
    }
}

/*
 * This is the main initialization state machine. As pseudo-code:

        status = get_hwstat();

        // correct device power state
        if ((status & 0x84) == 0x84)
                run_reboot_sm();

        // power device down
        if ((status & 0x80) == 0)
                set_hwstat(status | 0x80);

        // power device up
        run_powerup_sm();
        await_irq(IRQDATA_SCANPWR_ON);
 */

enum init_states {
  INIT_GET_HWSTAT = 0,
  INIT_CHECK_HWSTAT_REBOOT,
  INIT_REBOOT_POWER,
  INIT_CHECK_HWSTAT_POWERDOWN,
  INIT_POWERUP,
  INIT_AWAIT_SCAN_POWER,
  INIT_DONE,
  INIT_GET_VERSION,
  INIT_REPORT_VERSION,
  INIT_NUM_STATES,
};

static void
init_scanpwr_irq_cb (FpImageDevice *dev, GError *error,
                     uint16_t type, void *user_data)
{
  FpiSsm *ssm = user_data;
  FpiDeviceUru4000 *urudev = FPI_DEVICE_URU4000 (dev);

  if (error)
    {
      fpi_ssm_mark_failed (ssm, error);
    }
  else if (type != IRQDATA_SCANPWR_ON)
    {
      fp_dbg ("ignoring interrupt");
    }
  else if (fpi_ssm_get_cur_state (ssm) != INIT_AWAIT_SCAN_POWER)
    {
      fp_dbg ("early scanpwr interrupt");
      urudev->scanpwr_irq_timeouts = -1;
    }
  else
    {
      fp_dbg ("late scanpwr interrupt");
      fpi_ssm_next_state (ssm);
    }
}

static void
init_scanpwr_timeout (FpDevice *dev,
                      void     *user_data)
{
  FpiSsm *ssm = user_data;
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);

  fp_warn ("powerup timed out");
  self->irq_cb = NULL;
  self->scanpwr_irq_timeout = NULL;

  if (++self->scanpwr_irq_timeouts >= 3)
    {
      fp_err ("powerup timed out 3 times, giving up");
      fpi_ssm_mark_failed (ssm,
                           g_error_new_literal (G_USB_DEVICE_ERROR,
                                                G_USB_DEVICE_ERROR_TIMED_OUT,
                                                "Powerup timed out 3 times, giving up"));
    }
  else
    {
      fpi_ssm_jump_to_state (ssm, INIT_GET_HWSTAT);
    }
}

static void
init_run_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case INIT_GET_HWSTAT:
      sm_read_reg (ssm, dev, REG_HWSTAT);
      break;

    case INIT_CHECK_HWSTAT_REBOOT:
      self->last_hwstat = self->last_reg_rd[0];
      if ((self->last_hwstat & 0x84) == 0x84)
        fpi_ssm_next_state (ssm);
      else
        fpi_ssm_jump_to_state (ssm, INIT_CHECK_HWSTAT_POWERDOWN);
      break;

    case INIT_REBOOT_POWER:;
      FpiSsm *rebootsm = fpi_ssm_new (FP_DEVICE (dev),
                                      rebootpwr_run_state,
                                      REBOOTPWR_NUM_STATES);
      fpi_ssm_start_subsm (ssm, rebootsm);
      break;

    case INIT_CHECK_HWSTAT_POWERDOWN:
      if ((self->last_hwstat & 0x80) == 0)
        sm_set_hwstat (ssm, dev, self->last_hwstat | 0x80);
      else
        fpi_ssm_next_state (ssm);
      break;

    case INIT_POWERUP:
      if (!IRQ_HANDLER_IS_RUNNING (self))
        {
          fpi_ssm_mark_failed (ssm, fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                              "IRQ handler should be running but is not"));
          return;
        }
      self->irq_cb_data = ssm;
      self->irq_cb = init_scanpwr_irq_cb;

      FpiSsm *powerupsm = fpi_ssm_new (FP_DEVICE (dev),
                                       powerup_run_state,
                                       POWERUP_NUM_STATES);
      fpi_ssm_start_subsm (ssm, powerupsm);
      break;

    case INIT_AWAIT_SCAN_POWER:
      if (self->scanpwr_irq_timeouts < 0)
        {
          fpi_ssm_next_state (ssm);
          break;
        }

      /* sometimes the 56aa interrupt that we are waiting for never arrives,
       * so we include this timeout loop to retry the whole process 3 times
       * if we don't get an irq any time soon. */
      self->scanpwr_irq_timeout = fpi_device_add_timeout (_dev,
                                                          300,
                                                          init_scanpwr_timeout,
                                                          ssm, NULL);
      break;

    case INIT_DONE:
      if (self->scanpwr_irq_timeout)
        {
          g_source_destroy (self->scanpwr_irq_timeout);
          self->scanpwr_irq_timeout = NULL;
        }
      self->irq_cb_data = NULL;
      self->irq_cb = NULL;
      fpi_ssm_next_state (ssm);
      break;

    case INIT_GET_VERSION:
      sm_read_regs (ssm, dev, REG_DEVICE_INFO, 16);
      break;

    case INIT_REPORT_VERSION:
      /* Likely hardware revision, and firmware version.
       * Not sure which is which. */
      fp_info ("Versions %02x%02x and %02x%02x",
               self->last_reg_rd[10], self->last_reg_rd[11],
               self->last_reg_rd[4],  self->last_reg_rd[5]);
      fpi_ssm_mark_completed (ssm);
      break;
    }
}

static void
activate_initsm_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  fpi_image_device_activate_complete (FP_IMAGE_DEVICE (dev), error);
}

static void
dev_activate (FpImageDevice *dev)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);
  FpiSsm *ssm;

  start_irq_handler (dev);

  self->scanpwr_irq_timeouts = 0;
  ssm = fpi_ssm_new (FP_DEVICE (dev), init_run_state, INIT_NUM_STATES);
  fpi_ssm_start (ssm, activate_initsm_complete);
}

/***** DEINITIALIZATION *****/

static void
deactivate_irqs_stopped (FpImageDevice *dev)
{
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
deactivate_write_reg_cb (FpiUsbTransfer *transfer, FpDevice *dev,
                         gpointer user_data, GError *error)
{
  stop_irq_handler (FP_IMAGE_DEVICE (dev), deactivate_irqs_stopped);
}

static void
dev_deactivate (FpImageDevice *dev)
{
  /* This is started/handled by execute_state_change in order to delay the
   * action until after the image transfer has completed.
   * We just need to override the function so that the complete handler is
   * not called automatically. */
}

static void
execute_state_change (FpImageDevice *dev)
{
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);
  FpiSsm *ssm;

  switch (self->activate_state)
    {
    case FPI_IMAGE_DEVICE_STATE_DEACTIVATING:
      fp_dbg ("deactivating");
      self->irq_cb = NULL;
      self->irq_cb_data = NULL;
      write_reg (dev, REG_MODE, MODE_OFF,
                 deactivate_write_reg_cb, NULL);
      break;

    case FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON:
      fp_dbg ("wait finger on");
      if (!IRQ_HANDLER_IS_RUNNING (self))
        {
          fpi_image_device_session_error (dev,
                                          fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                    "IRQ handler should be running but is not"));
          return;
        }
      self->irq_cb = finger_presence_irq_cb;
      write_reg (dev, REG_MODE, MODE_AWAIT_FINGER_ON,
                 change_state_write_reg_cb, NULL);
      break;

    case FPI_IMAGE_DEVICE_STATE_CAPTURE:
      fp_dbg ("starting capture");
      self->irq_cb = NULL;

      ssm = fpi_ssm_new (FP_DEVICE (dev), imaging_run_state,
                         IMAGING_NUM_STATES);
      self->img_enc_seed = rand ();
      self->img_transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
      self->img_transfer->ssm = ssm;
      self->img_transfer->short_is_error = FALSE;
      fpi_usb_transfer_fill_bulk (self->img_transfer,
                                  EP_DATA,
                                  sizeof (struct uru4k_image));

      fpi_ssm_start (ssm, imaging_complete);

      write_reg (dev, REG_MODE, MODE_CAPTURE,
                 change_state_write_reg_cb, NULL);
      break;

    case FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_OFF:
      fp_dbg ("await finger off");
      if (!IRQ_HANDLER_IS_RUNNING (self))
        {
          fpi_image_device_session_error (dev,
                                          fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                    "IRQ handler should be running but is not"));
          return;
        }
      self->irq_cb = finger_presence_irq_cb;
      write_reg (dev, REG_MODE, MODE_AWAIT_FINGER_OFF,
                 change_state_write_reg_cb, NULL);
      break;

    /* Ignored states */
    case FPI_IMAGE_DEVICE_STATE_IDLE:
    case FPI_IMAGE_DEVICE_STATE_ACTIVATING:
    case FPI_IMAGE_DEVICE_STATE_INACTIVE:
      break;
    }
}

/***** LIBRARY STUFF *****/

static void
dev_init (FpImageDevice *dev)
{
  GError *error = NULL;
  FpiDeviceUru4000 *self;

  g_autoptr(GPtrArray) interfaces = NULL;
  GUsbInterface *iface = NULL;
  guint64 driver_data;
  SECStatus rv;
  SECItem item;
  int i;

  interfaces = g_usb_device_get_interfaces (fpi_device_get_usb_device (FP_DEVICE (dev)), &error);
  if (error)
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  /* Find fingerprint interface; TODO: Move this into probe() */
  for (i = 0; i < interfaces->len; i++)
    {
      GUsbInterface *cur_iface = g_ptr_array_index (interfaces, i);

      if (g_usb_interface_get_class (cur_iface) == 255 &&
          g_usb_interface_get_subclass (cur_iface) == 255 &&
          g_usb_interface_get_protocol (cur_iface) == 255)
        {
          iface = cur_iface;
          break;
        }
    }

  if (iface == NULL)
    {
      fp_err ("could not find interface");
      fpi_image_device_open_complete (dev,
                                      fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                "Could not find interface"));
      return;
    }

  /* TODO: Find/check endpoints; does not seem easily possible with GUsb unfortunately! */
#if 0
  if (iface_desc->bNumEndpoints != 2)
    {
      fp_err ("found %d endpoints!?", iface_desc->bNumEndpoints);
      r = -ENODEV;
      goto out;
    }

  ep = &iface_desc->endpoint[0];
  if (ep->bEndpointAddress != EP_INTR ||
      (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) !=
      LIBUSB_TRANSFER_TYPE_INTERRUPT)
    {
      fp_err ("unrecognised interrupt endpoint");
      r = -ENODEV;
      goto out;
    }

  ep = &iface_desc->endpoint[1];
  if (ep->bEndpointAddress != EP_DATA ||
      (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) !=
      LIBUSB_TRANSFER_TYPE_BULK)
    {
      fp_err ("unrecognised bulk endpoint");
      r = -ENODEV;
      goto out;
    }
#endif

  /* Device looks like a supported reader */

  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                     g_usb_interface_get_number (iface), 0, &error))
    {
      fpi_image_device_open_complete (dev, error);
      return;
    }

  /* Disable loading p11-kit's user configuration */
  g_setenv ("P11_KIT_NO_USER_CONFIG", "1", TRUE);

  /* Initialise NSS early */
  rv = NSS_NoDB_Init (".");
  if (rv != SECSuccess)
    {
      fp_err ("could not initialise NSS");
      fpi_image_device_open_complete (dev,
                                      fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                "Could not initialise NSS"));
      return;
    }

  self = FPI_DEVICE_URU4000 (dev);

  driver_data = fpi_device_get_driver_data (FP_DEVICE (dev));
  self->profile = &uru4k_dev_info[driver_data];
  self->interface = g_usb_interface_get_number (iface);

  /* Set up encryption */
  self->cipher = CKM_AES_ECB;
  self->slot = PK11_GetBestSlot (self->cipher, NULL);
  if (self->slot == NULL)
    {
      fp_err ("could not get encryption slot");
      fpi_image_device_open_complete (dev,
                                      fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                "Could not get encryption slot"));
      return;
    }
  item.type = siBuffer;
  item.data = (unsigned char *) crkey;
  item.len = sizeof (crkey);
  self->symkey = PK11_ImportSymKey (self->slot,
                                    self->cipher,
                                    PK11_OriginUnwrap,
                                    CKA_ENCRYPT,
                                    &item, NULL);
  if (self->symkey == NULL)
    {
      fp_err ("failed to import key into NSS");
      PK11_FreeSlot (self->slot);
      self->slot = NULL;
      fpi_image_device_open_complete (dev,
                                      fpi_device_error_new_msg (FP_DEVICE_ERROR_GENERAL,
                                                                "Failed to import key into NSS"));
      return;
    }
  self->param = PK11_ParamFromIV (self->cipher, NULL);

  fpi_image_device_open_complete (dev, NULL);
}

static void
dev_deinit (FpImageDevice *dev)
{
  GError *error = NULL;
  FpiDeviceUru4000 *self = FPI_DEVICE_URU4000 (dev);

  if (self->symkey)
    PK11_FreeSymKey (self->symkey);
  if (self->param)
    SECITEM_FreeItem (self->param, PR_TRUE);
  if (self->slot)
    PK11_FreeSlot (self->slot);
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  self->interface, 0, &error);
  fpi_image_device_close_complete (dev, error);
}

static const FpIdEntry id_table[] = {
  /* ms kbd with fp rdr */
  { .vid = 0x045e,  .pid = 0x00bb, .driver_data = MS_KBD },

  /* ms intellimouse with fp rdr */
  { .vid = 0x045e,  .pid = 0x00bc, .driver_data = MS_INTELLIMOUSE },

  /* ms fp rdr (standalone) */
  { .vid = 0x045e,  .pid = 0x00bd, .driver_data = MS_STANDALONE },

  /* ms fp rdr (standalone) v2 */
  { .vid = 0x045e,  .pid = 0x00ca, .driver_data = MS_STANDALONE_V2 },

  /* dp uru4000 (standalone) */
  { .vid = 0x05ba,  .pid = 0x0007, .driver_data = DP_URU4000 },

  /* dp uru4000 (keyboard) */
  { .vid = 0x05ba,  .pid = 0x0008, .driver_data = DP_URU4000 },

  /* dp uru4000b (standalone) */
  { .vid = 0x05ba,  .pid = 0x000a, .driver_data = DP_URU4000B },

  /* terminating entry */
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_uru4000_init (FpiDeviceUru4000 *self)
{
}

static void
fpi_device_uru4000_class_init (FpiDeviceUru4000Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "uru4000";
  dev_class->full_name = "Digital Persona U.are.U 4000/4000B/4500";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_PRESS;

  img_class->img_open = dev_init;
  img_class->img_close = dev_deinit;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;
  img_class->change_state = dev_change_state;

  img_class->img_width = IMAGE_WIDTH;
  img_class->img_height = IMAGE_HEIGHT;
}
