/*
 * AuthenTec AES1660 driver for libfprint
 * Copyright (C) 2012 Vasily Khoruzhick
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

#define FP_COMPONENT "aes1660"

#include "drivers_api.h"
#include "aeslib.h"
#include "aesx660.h"
#include "aes1660.h"

#define FRAME_WIDTH 128
#define IMAGE_WIDTH (FRAME_WIDTH + (FRAME_WIDTH / 2))

struct _FpiDeviceAes1660
{
  FpiDeviceAesX660 parent;
};
G_DECLARE_FINAL_TYPE (FpiDeviceAes1660, fpi_device_aes1660, FPI,
                      DEVICE_AES1660, FpiDeviceAesX660);
G_DEFINE_TYPE (FpiDeviceAes1660, fpi_device_aes1660, FPI_TYPE_DEVICE_AES_X660);

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = FRAME_WIDTH,
  .frame_height = AESX660_FRAME_HEIGHT,
  .image_width = IMAGE_WIDTH,
  .get_pixel = aes_get_pixel,
};

static const FpIdEntry id_table[] = {
  { .vid = 0x08ff,  .pid = 0x1660, },
  { .vid = 0x08ff,  .pid = 0x1680, },
  { .vid = 0x08ff,  .pid = 0x1681, },
  { .vid = 0x08ff,  .pid = 0x1682, },
  { .vid = 0x08ff,  .pid = 0x1683, },
  { .vid = 0x08ff,  .pid = 0x1684, },
  { .vid = 0x08ff,  .pid = 0x1685, },
  { .vid = 0x08ff,  .pid = 0x1686, },
  { .vid = 0x08ff,  .pid = 0x1687, },
  { .vid = 0x08ff,  .pid = 0x1688, },
  { .vid = 0x08ff,  .pid = 0x1689, },
  { .vid = 0x08ff,  .pid = 0x168a, },
  { .vid = 0x08ff,  .pid = 0x168b, },
  { .vid = 0x08ff,  .pid = 0x168c, },
  { .vid = 0x08ff,  .pid = 0x168d, },
  { .vid = 0x08ff,  .pid = 0x168e, },
  { .vid = 0x08ff,  .pid = 0x168f, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_aes1660_init (FpiDeviceAes1660 *self)
{
}
static void
fpi_device_aes1660_class_init (FpiDeviceAes1660Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);
  FpiDeviceAesX660Class *aes_class = FPI_DEVICE_AES_X660_CLASS (klass);

  dev_class->id = "aes1660";
  dev_class->full_name = "AuthenTec AES1660";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->bz3_threshold = 20;

  img_class->img_width = FRAME_WIDTH + FRAME_WIDTH / 2;
  img_class->img_height = -1;

  aes_class->init_seqs[0] = aes1660_init_1;
  aes_class->init_seqs_len[0] = G_N_ELEMENTS (aes1660_init_1);
  aes_class->init_seqs[1] = aes1660_init_2;
  aes_class->init_seqs_len[1] = G_N_ELEMENTS (aes1660_init_2);
  aes_class->start_imaging_cmd = (unsigned char *) aes1660_start_imaging_cmd;
  aes_class->start_imaging_cmd_len = sizeof (aes1660_start_imaging_cmd);
  aes_class->assembling_ctx = &assembling_ctx;
}
