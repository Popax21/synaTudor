/*
 * Validity VFS0050 driver for libfprint
 * Copyright (C) 2015-2016 Konstantin Semenov <zemen17@gmail.com>
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

#define FP_COMPONENT "vfs0050"

#include "drivers_api.h"
#include "vfs0050.h"

G_DEFINE_TYPE (FpDeviceVfs0050, fpi_device_vfs0050, FP_TYPE_IMAGE_DEVICE)

/* USB functions */

/* Callback for async_write */
static void
async_write_callback (FpiUsbTransfer *transfer, FpDevice *device,
                      gpointer user_data, GError *error)
{
  if (error)
    {
      fp_err ("USB write transfer: %s", error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  fpi_ssm_next_state (transfer->ssm);
}

/* Send data to EP1, the only out endpoint */
FP_GNUC_ACCESS (read_only, 3, 4)
static void
async_write (FpiSsm   *ssm,
             FpDevice *dev,
             void     *data,
             int       len)
{
  FpiUsbTransfer *transfer;

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  fpi_usb_transfer_fill_bulk_full (transfer, 0x01, data, len, NULL);
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, VFS_USB_TIMEOUT, NULL,
                           async_write_callback, NULL);
}

/* Callback for async_read */
static void
async_read_callback (FpiUsbTransfer *transfer, FpDevice *device,
                     gpointer user_data, GError *error)
{
  int ep = transfer->endpoint;

  if (error)
    {
      fp_err ("USB read transfer on endpoint %d: %s", ep - 0x80,
              error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  fpi_ssm_next_state (transfer->ssm);
}

/* Receive data from the given ep and either discard or fill the given buffer */
static void
async_read (FpiSsm   *ssm,
            FpDevice *dev,
            int       ep,
            void     *data,
            int       len)
{
  FpiUsbTransfer *transfer;
  GDestroyNotify free_func = NULL;

  ep |= FPI_USB_ENDPOINT_IN;

  transfer = fpi_usb_transfer_new (FP_DEVICE (dev));
  transfer->ssm = ssm;
  transfer->short_is_error = TRUE;

  if (data == NULL)
    {
      data = g_malloc0 (len);
      free_func = g_free;
    }

  /* 0x83 is the only interrupt endpoint */
  if (ep == EP3_IN)
    fpi_usb_transfer_fill_interrupt_full (transfer, ep, data, len, free_func);
  else
    fpi_usb_transfer_fill_bulk_full (transfer, ep, data, len, free_func);

  fpi_usb_transfer_submit (transfer, VFS_USB_TIMEOUT, NULL,
                           async_read_callback, NULL);
}

/* Callback for async_abort */
static void
async_abort_callback (FpiUsbTransfer *transfer, FpDevice *device,
                      gpointer user_data, GError *error)
{
  int ep = transfer->endpoint;

  /* In normal case endpoint is empty */
  if (g_error_matches (error, G_USB_DEVICE_ERROR, G_USB_DEVICE_ERROR_TIMED_OUT) ||
      (g_strcmp0 (g_getenv ("FP_DEVICE_EMULATION"), "1") == 0 && transfer->actual_length == 0))
    {
      g_clear_error (&error);
      fpi_ssm_next_state (transfer->ssm);
      return;
    }

  if (error)
    {
      fp_err ("USB write transfer: %s", error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  /* Don't stop process, only print warning */
  fp_warn ("Endpoint %d had extra %zd bytes readable", ep - 0x80,
           transfer->actual_length);

  fpi_ssm_jump_to_state (transfer->ssm,
                         fpi_ssm_get_cur_state (transfer->ssm));
}

/* Receive data from the given ep; continues to the next state once no
 * more data is available. Otherwise the current state is repeated. */
static void
async_abort (FpDevice *dev, FpiSsm *ssm, int ep)
{
  FpiUsbTransfer *transfer;

  ep |= FPI_USB_ENDPOINT_IN;

  transfer = fpi_usb_transfer_new (dev);

  /* 0x83 is the only interrupt endpoint */
  if (ep == EP3_IN)
    fpi_usb_transfer_fill_interrupt (transfer, ep, VFS_USB_BUFFER_SIZE);
  else
    fpi_usb_transfer_fill_bulk (transfer, ep, VFS_USB_BUFFER_SIZE);

  transfer->ssm = ssm;

  fpi_usb_transfer_submit (transfer, VFS_USB_ABORT_TIMEOUT, NULL,
                           async_abort_callback, NULL);
}

/* Image processing functions */

/* Pixel getter for fpi_assemble_lines */
static unsigned char
vfs0050_get_pixel (struct fpi_line_asmbl_ctx *ctx,
                   GSList * line, unsigned int x)
{
  return ((struct vfs_line *) line->data)->data[x];
}

/* Deviation getter for fpi_assemble_lines */
static int
vfs0050_get_difference (struct fpi_line_asmbl_ctx *ctx,
                        GSList * line_list_1, GSList * line_list_2)
{
  struct vfs_line *line1 = line_list_1->data;
  struct vfs_line *line2 = line_list_2->data;
  const int shift = (VFS_IMAGE_WIDTH - VFS_NEXT_LINE_WIDTH) / 2 - 1;
  int res = 0;

  for (int i = 0; i < VFS_NEXT_LINE_WIDTH; ++i)
    {
      int x =
        (int) line1->next_line_part[i] - (int) line2->data[shift + i];
      res += x * x;
    }
  return res;
}

#define VFS_NOISE_THRESHOLD 40

/* Checks whether line is noise or not using hardware parameters */
static char
is_noise (struct vfs_line *line)
{
  int val1 = line->noise_hash_1;
  int val2 = line->noise_hash_2;

  if (val1 > VFS_NOISE_THRESHOLD &&
      val1 < 256 - VFS_NOISE_THRESHOLD &&
      val2 > VFS_NOISE_THRESHOLD && val2 < 256 - VFS_NOISE_THRESHOLD)
    return 1;
  return 0;
}

/* Parameters for fpi_assemble_lines */
static struct fpi_line_asmbl_ctx assembling_ctx = {
  .line_width = VFS_IMAGE_WIDTH,
  .max_height = VFS_MAX_HEIGHT,
  .resolution = 10,
  .median_filter_size = 25,
  .max_search_offset = 100,
  .get_deviation = vfs0050_get_difference,
  .get_pixel = vfs0050_get_pixel,
};

/* Processes image before submitting */
static FpImage *
prepare_image (FpDeviceVfs0050 *vdev)
{
  int height = vdev->bytes / VFS_LINE_SIZE;

  /* Noise cleaning. IMHO, it works pretty well
     I've not detected cases when it doesn't work or cuts a part of the finger
     Noise arises at the end of scan when some water remains on the scanner */
  while (height > 0)
    {
      if (!is_noise (vdev->lines_buffer + height - 1))
        break;
      --height;
    }
  if (height > VFS_MAX_HEIGHT)
    height = VFS_MAX_HEIGHT;

  /* If image is not good enough */
  if (height < VFS_IMAGE_WIDTH)
    return NULL;

  /* Building GSList */
  GSList *lines = NULL;

  for (int i = height - 1; i >= 0; --i)
    lines = g_slist_prepend (lines, vdev->lines_buffer + i);

  /* Perform line assembling */
  FpImage *img = fpi_assemble_lines (&assembling_ctx, lines, height);

  g_slist_free (lines);
  return img;
}

/* Processes and submits image after fingerprint received */
static void
submit_image (FpDeviceVfs0050 *self)
{
  FpImageDevice *idev = FP_IMAGE_DEVICE (self);

  /* We were not asked to submit image actually */
  if (!self->active)
    return;

  FpImage *img = prepare_image (self);

  if (!img)
    fpi_image_device_retry_scan (idev, FP_DEVICE_RETRY_TOO_SHORT);
  else
    fpi_image_device_image_captured (idev, img);

  /* Finger not on the scanner */
  fpi_image_device_report_finger_status (idev, FALSE);
}

/* Proto functions */

/* SSM loop for clear_ep2 */
static void
clear_ep2_ssm (FpiSsm *ssm, FpDevice *dev)
{
  char command04 = 0x04;

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case SUBSM1_COMMAND_04:
      async_write (ssm, dev, &command04, sizeof (command04));
      break;

    case SUBSM1_RETURN_CODE:
      async_read (ssm, dev, 1, NULL, 2);
      break;

    case SUBSM1_ABORT_2:
      async_abort (dev, ssm, 2);
      break;

    default:
      fp_err ("Unknown SUBSM1 state");
      fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
    }
}

/* Send command to clear EP2 */
static void
clear_ep2 (FpDevice *dev,
           FpiSsm   *ssm)
{
  FpiSsm *subsm = fpi_ssm_new (dev, clear_ep2_ssm, SUBSM1_STATES);

  fpi_ssm_start_subsm (ssm, subsm);
}

static void
send_control_packet_ssm (FpiSsm *ssm, FpDevice *dev)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case SUBSM2_SEND_CONTROL:
      async_write (ssm, dev, self->control_packet,
                   VFS_CONTROL_PACKET_SIZE);
      break;

    case SUBSM2_RETURN_CODE:
      async_read (ssm, dev, 1, NULL, 2);
      break;

    case SUBSM2_SEND_COMMIT:
      /* next_receive_* packets could be sent only in pair */
      if (self->control_packet == next_receive_1)
        {
          self->control_packet = next_receive_2;
          fpi_ssm_jump_to_state (ssm, SUBSM2_SEND_CONTROL);
          break;
        }
      /* commit_out in Windows differs in each commit, but I send the same each time */
      async_write (ssm, dev, commit_out, sizeof (commit_out));
      break;

    case SUBSM2_COMMIT_RESPONSE:
      async_read (ssm, dev, 1, NULL, VFS_COMMIT_RESPONSE_SIZE);
      break;

    case SUBSM2_READ_EMPTY_INTERRUPT:
      /* I don't know how to check result, it could be different
      * NOTE: I guess this comment relates to the above read. */
      async_read (ssm, dev, 3, self->interrupt, VFS_INTERRUPT_SIZE);
      break;

    case SUBSM2_ABORT_3:
      /* Check that interrupt is empty */
      if (memcmp
            (self->interrupt, empty_interrupt, VFS_INTERRUPT_SIZE))
        {
          fp_err ("Unknown SUBSM2 state");
          fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
          break;
        }
      async_abort (dev, ssm, 3);
      break;

    case SUBSM2_CLEAR_EP2:
      /* After turn_on Windows doesn't clear EP2 */
      if (self->control_packet != turn_on)
        clear_ep2 (dev, ssm);
      else
        fpi_ssm_next_state (ssm);
      break;

    default:
      fp_err ("Unknown SUBSM2 state");
      fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
    }
}

/* Send device state control packet */
static void
send_control_packet (FpiSsm   *ssm,
                     FpDevice *dev)
{
  FpiSsm *subsm =
    fpi_ssm_new (dev, send_control_packet_ssm, SUBSM2_STATES);

  fpi_ssm_start_subsm (ssm, subsm);
}

/* Clears all fprint data */
static void
clear_data (FpDeviceVfs0050 *vdev)
{
  g_free (vdev->lines_buffer);
  vdev->lines_buffer = NULL;
  vdev->memory = vdev->bytes = 0;
}

/* After receiving interrupt from EP3 */
static void
interrupt_callback (FpiUsbTransfer *transfer, FpDevice *device,
                    gpointer user_data, GError *error)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (device);
  unsigned char *interrupt = transfer->buffer;

  /* we expect a cancellation error when the device is deactivating
   * go into the SSM_CLEAR_EP2 state in that case. */
  if (!self->active && g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
      g_error_free (error);
      fpi_ssm_jump_to_state (transfer->ssm, SSM_CLEAR_EP2);
      return;
    }

  if (error)
    {
      fp_err ("USB read interrupt transfer: %s",
              error->message);
      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }

  /* Standard interrupts */
  if (memcmp (interrupt, interrupt1, VFS_INTERRUPT_SIZE) == 0 ||
      memcmp (interrupt, interrupt2, VFS_INTERRUPT_SIZE) == 0 ||
      memcmp (interrupt, interrupt3, VFS_INTERRUPT_SIZE) == 0)
    {
      /* Go to the next ssm stage */
      fpi_ssm_next_state (transfer->ssm);
      return;
    }

  /* When finger is on the scanner before turn_on */
  if (interrupt[0] == 0x01)
    {
      fp_warn ("Finger is already on the scanner");

      /* Go to the next ssm stage */
      fpi_ssm_next_state (transfer->ssm);
      return;
    }

  /* Unknown interrupt; abort the session */
  fp_err ("Unknown interrupt '%02x:%02x:%02x:%02x:%02x'!",
          interrupt[0] & 0xff, interrupt[1] & 0xff, interrupt[2] & 0xff,
          interrupt[3] & 0xff, interrupt[4] & 0xff);

  /* Abort ssm */
  fpi_ssm_mark_failed (transfer->ssm,
                       fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
}

static void
receive_callback (FpiUsbTransfer *transfer, FpDevice *device,
                  gpointer user_data, GError *error)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (device);

  if (error && !g_error_matches (error, G_USB_DEVICE_ERROR, G_USB_DEVICE_ERROR_TIMED_OUT))
    {
      fp_err ("USB read transfer: %s", error->message);

      fpi_ssm_mark_failed (transfer->ssm, error);
      return;
    }
  if (error)
    g_error_free (error);

  /* Capture is done when there is no more data to transfer or device timed out */
  if (transfer->actual_length <= 0)
    {
      fpi_ssm_next_state (transfer->ssm);
    }
  else
    {
      self->bytes += transfer->actual_length;

      /* Try reading more data */
      fpi_ssm_jump_to_state (transfer->ssm,
                             fpi_ssm_get_cur_state (transfer->ssm));
    }
}

/* Main SSM loop */
static void
activate_ssm (FpiSsm *ssm, FpDevice *dev)
{
  FpImageDevice *idev = FP_IMAGE_DEVICE (dev);
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case SSM_INITIAL_ABORT_1:
      async_abort (dev, ssm, 1);
      break;

    case SSM_INITIAL_ABORT_2:
      async_abort (dev, ssm, 2);
      break;

    case SSM_INITIAL_ABORT_3:
      async_abort (dev, ssm, 3);
      break;

    case SSM_CLEAR_EP2:
      clear_ep2 (dev, ssm);
      break;

    case SSM_TURN_OFF:
      /* Set control_packet argument */
      self->control_packet = turn_off;

      send_control_packet (ssm, dev);
      break;

    case SSM_TURN_ON:
      if (!self->active)
        {
          /* The only correct exit */
          fpi_ssm_mark_completed (ssm);

          if (self->need_report)
            {
              fpi_image_device_deactivate_complete (idev,
                                                    NULL);
              self->need_report = 0;
            }
          break;
        }
      /* Set control_packet argument */
      self->control_packet = turn_on;

      send_control_packet (ssm, dev);
      break;

    case SSM_ASK_INTERRUPT: {
        FpiUsbTransfer *transfer;
        /* Activated, light must be blinking now */

        /* If we first time here, report that activate completed */
        if (self->need_report)
          {
            fpi_image_device_activate_complete (idev, NULL);
            self->need_report = 0;
          }

        /* Asynchronously enquire an interrupt */
        transfer = fpi_usb_transfer_new (dev);
        transfer->ssm = ssm;
        transfer->short_is_error = TRUE;
        fpi_usb_transfer_fill_interrupt (transfer, 0x83, VFS_INTERRUPT_SIZE);
        fpi_usb_transfer_submit (transfer,
                                 0,
                                 fpi_device_get_cancellable (dev),
                                 interrupt_callback, NULL);

        /* I've put it here to be sure that data is cleared */
        clear_data (self);

        fpi_ssm_next_state (ssm);
        break;
      }

    case SSM_WAIT_INTERRUPT:
      /* TODO: This state is unused at this point. When we
       *       are in this state, then a user cancellation will
       *       cause deactivation. In that case, the USB transfer
       *       is cancelled and the device is set to not be active.
       *       We then go into SSM_CLEAR_EP2 based on the
       *       cancellation. */
      break;

    case SSM_RECEIVE_FINGER: {
        FpiUsbTransfer *transfer;

        if (self->memory == 0)
          {
            /* Initialize fingerprint buffer */
            g_free (self->lines_buffer);
            self->memory = VFS_USB_BUFFER_SIZE;
            self->lines_buffer = g_malloc (self->memory);
            self->bytes = 0;

            /* Finger is on the scanner */
            fpi_image_device_report_finger_status (idev, TRUE);
          }

        /* Increase buffer size while it's insufficient */
        while (self->bytes + VFS_USB_BUFFER_SIZE > self->memory)
          {
            self->memory <<= 1;
            self->lines_buffer =
              (struct vfs_line *) g_realloc (self->lines_buffer,
                                             self->memory);
          }

        /* Receive chunk of data */
        transfer = fpi_usb_transfer_new (dev);
        fpi_usb_transfer_fill_bulk_full (transfer, 0x82,
                                         (guint8 *) self->lines_buffer + self->bytes,
                                         VFS_USB_BUFFER_SIZE, NULL);
        transfer->ssm = ssm;
        fpi_usb_transfer_submit (transfer, VFS_USB_TIMEOUT, NULL,
                                 receive_callback, NULL);
        break;
      }

    case SSM_SUBMIT_IMAGE:
      submit_image (self);
      clear_data (self);

      /* Wait for probable vdev->active changing */
      fpi_ssm_next_state_delayed (ssm, VFS_SSM_TIMEOUT, NULL);
      break;

    case SSM_NEXT_RECEIVE:
      if (!self->active)
        {
          /* It's the last scan */
          fpi_ssm_jump_to_state (ssm, SSM_CLEAR_EP2);
          break;
        }

      /* Set control_packet argument */
      self->control_packet = next_receive_1;

      send_control_packet (ssm, dev);
      break;

    case SSM_WAIT_ANOTHER_SCAN:
      /* Orange light is on now */
      fpi_ssm_jump_to_state_delayed (ssm, SSM_TURN_ON, VFS_SSM_ORANGE_TIMEOUT,
                                     NULL);
      break;

    default:
      fp_err ("Unknown state");
      fpi_ssm_mark_failed (ssm, fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
    }
}

/* Driver functions */

/* Callback for dev_activate ssm */
static void
dev_activate_callback (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (dev);

  self->ssm_active = 0;

  if (error)
    {
      g_warning ("Unhandled device activation error: %s", error->message);
      g_error_free (error);
    }

}

/* Activate device */
static void
dev_activate (FpImageDevice *idev)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (idev);

  /* Initialize flags */
  self->active = 1;
  self->need_report = 1;
  self->ssm_active = 1;

  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (idev), activate_ssm, SSM_STATES);

  fpi_ssm_start (ssm, dev_activate_callback);
}

/* Deactivate device */
static void
dev_deactivate (FpImageDevice *idev)
{
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (idev);

  if (!self->ssm_active)
    {
      fpi_image_device_deactivate_complete (idev, NULL);
      return;
    }

  /* Initialize flags */
  self->active = 0;
  self->need_report = 1;
}

/* Callback for dev_open ssm */
static void
dev_open_callback (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  /* Notify open complete */
  fpi_image_device_open_complete (FP_IMAGE_DEVICE (dev), error);
}

/* Open device */
static void
dev_open (FpImageDevice *idev)
{
  GError *error = NULL;

  /* Claim usb interface */
  if (!g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (idev)), 0, 0, &error))
    {
      fpi_image_device_open_complete (idev, error);
      return;
    }

  /* Clearing previous device state */
  FpiSsm *ssm = fpi_ssm_new (FP_DEVICE (idev), activate_ssm, SSM_STATES);

  fpi_ssm_start (ssm, dev_open_callback);
}

/* Close device */
static void
dev_close (FpImageDevice *idev)
{
  GError *error = NULL;
  FpDeviceVfs0050 *self = FPI_DEVICE_VFS0050 (idev);

  clear_data (self);

  /* Release usb interface */
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (idev)),
                                  0, 0, &error);

  /* Notify close complete */
  fpi_image_device_close_complete (idev, error);
}

/* Usb id table of device */
static const FpIdEntry id_table[] = {
  {.vid = 0x138a,  .pid = 0x0050, },
  {.vid = 0,  .pid = 0,  .driver_data = 0},
};

static void
fpi_device_vfs0050_init (FpDeviceVfs0050 *self)
{
}
static void
fpi_device_vfs0050_class_init (FpDeviceVfs0050Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "vfs0050";
  dev_class->full_name = "Validity VFS0050";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_open;
  img_class->img_close = dev_close;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;

  img_class->bz3_threshold = 24;

  img_class->img_width = VFS_IMAGE_WIDTH;
  img_class->img_height = -1;
}
