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

#define FP_COMPONENT "vfs301"

#include "drivers_api.h"
#include "vfs301.h"

G_DEFINE_TYPE (FpDeviceVfs301, fpi_device_vfs301, FP_TYPE_IMAGE_DEVICE)

/************************** GENERIC STUFF *************************************/

static int
submit_image (FpiSsm        *ssm,
              FpImageDevice *dev)
{
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (dev);
  int height;
  FpImage *img;

#if 0
  /* XXX: This is probably handled by libfprint automagically? */
  if (vdev->scanline_count < 20)
    {
      fpi_ssm_jump_to_state (ssm, M_REQUEST_PRINT);
      return 0;
    }
#endif

  img = fp_image_new (VFS301_FP_OUTPUT_WIDTH, self->scanline_count);
  if (img == NULL)
    return 0;

  vfs301_extract_image (self, img->data, &height);

  /* TODO: how to detect flip? should the resulting image be
   * oriented so that it is equal e.g. to a fingerprint on a paper,
   * or to the finger when I look at it?) */
  img->flags = FPI_IMAGE_COLORS_INVERTED | FPI_IMAGE_V_FLIPPED;

  /* The image buffer is larger at this point, but that does not
   * matter. */
  img->width = VFS301_FP_OUTPUT_WIDTH;
  img->height = height;

  fpi_image_device_image_captured (dev, img);

  return 1;
}

/* Loop ssm states */
enum {
  /* Step 0 - Scan finger */
  M_REQUEST_PRINT,
  M_WAIT_PRINT,
  M_CHECK_PRINT,
  M_READ_PRINT_START,
  M_READ_PRINT_WAIT,
  M_READ_PRINT_POLL,
  M_SUBMIT_PRINT,

  /* Number of states */
  M_LOOP_NUM_STATES,
};

/* Exec loop sequential state machine */
static void
m_loop_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpImageDevice *dev = FP_IMAGE_DEVICE (_dev);
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (_dev);

  switch (fpi_ssm_get_cur_state (ssm))
    {
    case M_REQUEST_PRINT:
      vfs301_proto_request_fingerprint (self);
      fpi_ssm_next_state (ssm);
      break;

    case M_WAIT_PRINT:
      /* Wait fingerprint scanning */
      fpi_ssm_next_state_delayed (ssm, 200, NULL);
      break;

    case M_CHECK_PRINT:
      if (!vfs301_proto_peek_event (self))
        fpi_ssm_jump_to_state (ssm, M_WAIT_PRINT);
      else
        fpi_ssm_next_state (ssm);
      break;

    case M_READ_PRINT_START:
      fpi_image_device_report_finger_status (dev, TRUE);
      vfs301_proto_process_event_start (self);
      fpi_ssm_next_state (ssm);
      break;

    case M_READ_PRINT_WAIT:
      /* Wait fingerprint scanning */
      fpi_ssm_next_state_delayed (ssm, 200, NULL);
      break;

    case M_READ_PRINT_POLL:
      {
        int rv = vfs301_proto_process_event_poll (self);
        g_assert (rv != VFS301_FAILURE);
        if (rv == VFS301_ONGOING)
          fpi_ssm_jump_to_state (ssm, M_READ_PRINT_WAIT);
        else
          fpi_ssm_next_state (ssm);
      }
      break;

    case M_SUBMIT_PRINT:
      if (submit_image (ssm, dev))
        {
          fpi_ssm_mark_completed (ssm);
          /* NOTE: finger off is expected only after submitting image... */
          fpi_image_device_report_finger_status (dev, FALSE);
        }
      else
        {
          fpi_ssm_jump_to_state (ssm, M_REQUEST_PRINT);
        }
      break;

    default:
      g_assert_not_reached ();
    }
}

/* Exec init sequential state machine */
static void
m_init_state (FpiSsm *ssm, FpDevice *_dev)
{
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (_dev);

  g_assert (fpi_ssm_get_cur_state (ssm) == 0);

  vfs301_proto_init (self);

  fpi_ssm_mark_completed (ssm);
}

/* Complete init sequential state machine */
static void
m_init_complete (FpiSsm *ssm, FpDevice *dev, GError *error)
{
  fpi_image_device_activate_complete (FP_IMAGE_DEVICE (dev), error);
}

/* Activate device */
static void
dev_activate (FpImageDevice *dev)
{
  FpiSsm *ssm;

  /* Start init ssm */
  ssm = fpi_ssm_new (FP_DEVICE (dev), m_init_state, 1);
  fpi_ssm_start (ssm, m_init_complete);
}

/* Deactivate device */
static void
dev_deactivate (FpImageDevice *dev)
{
  FpDeviceVfs301 *self;

  self = FPI_DEVICE_VFS301 (dev);
  vfs301_proto_deinit (self);
  fpi_image_device_deactivate_complete (dev, NULL);
}

static void
dev_change_state (FpImageDevice *dev, FpiImageDeviceState state)
{
  FpiSsm *ssm_loop;

  if (state != FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON)
    return;

  /* Start a capture operation. */
  ssm_loop = fpi_ssm_new (FP_DEVICE (dev), m_loop_state, M_LOOP_NUM_STATES);
  fpi_ssm_start (ssm_loop, NULL);
}

static void
dev_open (FpImageDevice *dev)
{
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (dev);
  GError *error = NULL;

  /* Claim usb interface */
  g_usb_device_claim_interface (fpi_device_get_usb_device (FP_DEVICE (dev)), 0, 0, &error);

  /* Initialize private structure */
  self->scanline_count = 0;

  /* Notify open complete */
  fpi_image_device_open_complete (dev, error);
}

static void
dev_close (FpImageDevice *dev)
{
  FpDeviceVfs301 *self = FPI_DEVICE_VFS301 (dev);
  GError *error = NULL;

  /* Release private structure */
  g_clear_pointer (&self->scanline_buf, g_free);

  /* Release usb interface */
  g_usb_device_release_interface (fpi_device_get_usb_device (FP_DEVICE (dev)),
                                  0, 0, &error);

  /* Notify close complete */
  fpi_image_device_close_complete (dev, error);
}

/* Usb id table of device */
static const FpIdEntry id_table[] = {
  { /* vfs301 */ .vid = 0x138a,  .pid = 0x0005, },
  { /* vfs300 */ .vid = 0x138a,  .pid = 0x0008, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_vfs301_init (FpDeviceVfs301 *self)
{
}
static void
fpi_device_vfs301_class_init (FpDeviceVfs301Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);

  dev_class->id = "vfs301";
  dev_class->full_name = "Validity VFS301";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->img_open = dev_open;
  img_class->img_close = dev_close;
  img_class->activate = dev_activate;
  img_class->deactivate = dev_deactivate;
  img_class->change_state = dev_change_state;

  img_class->bz3_threshold = 24;

  img_class->img_width = VFS301_FP_WIDTH;
  img_class->img_height = -1;
}
