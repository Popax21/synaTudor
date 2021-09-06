/*
 * Shared functions between libfprint Authentec drivers
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
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

#define FP_COMPONENT "aeslib"

#include "drivers_api.h"

#include <errno.h>
#include <string.h>

#include "aeslib.h"

#define MAX_REGWRITES_PER_REQUEST 16

#define BULK_TIMEOUT 4000
#define EP_IN (1 | FPI_USB_ENDPOINT_IN)
#define EP_OUT (2 | FPI_USB_ENDPOINT_OUT)

struct write_regv_data
{
  unsigned int               num_regs;
  const struct aes_regwrite *regs;
  unsigned int               offset;
  aes_write_regv_cb          callback;
  void                      *user_data;
};

static void continue_write_regv (FpImageDevice          *dev,
                                 struct write_regv_data *wdata);

/* libusb bulk callback for regv write completion transfer. continues the
 * transaction */
static void
write_regv_trf_complete (FpiUsbTransfer *transfer, FpDevice *device,
                         gpointer user_data, GError *error)
{
  struct write_regv_data *wdata = user_data;

  if (error)
    {
      wdata->callback (FP_IMAGE_DEVICE (device), error, wdata->user_data);
      g_free (wdata);
    }
  else
    {
      continue_write_regv (FP_IMAGE_DEVICE (device), wdata);
    }
}

/* write from wdata->offset to upper_bound (inclusive) of wdata->regs */
static void
do_write_regv (FpImageDevice *dev, struct write_regv_data *wdata, int upper_bound)
{
  unsigned int offset = wdata->offset;
  unsigned int num = upper_bound - offset + 1;
  size_t alloc_size = num * 2;
  unsigned int i;
  size_t data_offset = 0;
  FpiUsbTransfer *transfer = fpi_usb_transfer_new (FP_DEVICE (dev));

  fpi_usb_transfer_fill_bulk (transfer, EP_OUT, alloc_size);

  for (i = offset; i < offset + num; i++)
    {
      const struct aes_regwrite *regwrite = &wdata->regs[i];
      transfer->buffer[data_offset++] = regwrite->reg;
      transfer->buffer[data_offset++] = regwrite->value;
    }

  transfer->short_is_error = TRUE;
  fpi_usb_transfer_submit (transfer, BULK_TIMEOUT, NULL,
                           write_regv_trf_complete, wdata);
}

/* write the next batch of registers to be written, or if there are no more,
 * indicate completion to the caller */
static void
continue_write_regv (FpImageDevice *dev, struct write_regv_data *wdata)
{
  unsigned int offset = wdata->offset;
  unsigned int regs_remaining;
  unsigned int limit;
  unsigned int upper_bound;
  int i;

  /* skip all zeros and ensure there is still work to do */
  while (TRUE)
    {
      if (offset >= wdata->num_regs)
        {
          fp_dbg ("all registers written");
          wdata->callback (dev, 0, wdata->user_data);
          g_free (wdata);
          return;
        }
      if (wdata->regs[offset].reg)
        break;
      offset++;
    }

  wdata->offset = offset;
  regs_remaining = wdata->num_regs - offset;
  limit = MIN (regs_remaining, MAX_REGWRITES_PER_REQUEST);
  upper_bound = offset + limit - 1;

  /* determine if we can write the entire of the regs at once, or if there
   * is a zero dividing things up */
  for (i = offset; i <= upper_bound; i++)
    if (!wdata->regs[i].reg)
      {
        upper_bound = i - 1;
        break;
      }

  do_write_regv (dev, wdata, upper_bound);

  wdata->offset = upper_bound + 1;
}

/* write a load of registers to the device, combining multiple writes in a
 * single URB up to a limit. insert writes to non-existent register 0 to force
 * specific groups of writes to be separated by different URBs. */
void
aes_write_regv (FpImageDevice *dev, const struct aes_regwrite *regs,
                unsigned int num_regs, aes_write_regv_cb callback,
                void *user_data)
{
  struct write_regv_data *wdata;

  fp_dbg ("write %d regs", num_regs);
  wdata = g_malloc (sizeof (*wdata));
  wdata->num_regs = num_regs;
  wdata->regs = regs;
  wdata->offset = 0;
  wdata->callback = callback;
  wdata->user_data = user_data;
  continue_write_regv (dev, wdata);
}

unsigned char
aes_get_pixel (struct fpi_frame_asmbl_ctx *ctx,
               struct fpi_frame           *frame,
               unsigned int                x,
               unsigned int                y)
{
  unsigned char ret;

  ret = frame->data[x * (ctx->frame_height >> 1) + (y >> 1)];
  ret = y % 2 ? ret >> 4 : ret & 0xf;
  ret *= 17;

  return ret;
}
