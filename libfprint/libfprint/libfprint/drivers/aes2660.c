/*
 * AuthenTec AES2660 driver for libfprint
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

#define FP_COMPONENT "aes2660"

#include "drivers_api.h"
#include "aeslib.h"
#include "aesx660.h"
#include "aes2660.h"

#define FRAME_WIDTH 192
#define IMAGE_WIDTH (FRAME_WIDTH + (FRAME_WIDTH / 2))

struct _FpiDeviceAes2660
{
  FpiDeviceAesX660 parent;
};
G_DECLARE_FINAL_TYPE (FpiDeviceAes2660, fpi_device_aes2660, FPI,
                      DEVICE_AES2660, FpiDeviceAesX660);
G_DEFINE_TYPE (FpiDeviceAes2660, fpi_device_aes2660, FPI_TYPE_DEVICE_AES_X660);

static struct fpi_frame_asmbl_ctx assembling_ctx = {
  .frame_width = FRAME_WIDTH,
  .frame_height = AESX660_FRAME_HEIGHT,
  .image_width = IMAGE_WIDTH,
  .get_pixel = aes_get_pixel,
};

static const FpIdEntry id_table[] = {
  { .vid = 0x08ff,  .pid = 0x2660, },
  { .vid = 0x08ff,  .pid = 0x2680, },
  { .vid = 0x08ff,  .pid = 0x2681, },
  { .vid = 0x08ff,  .pid = 0x2682, },
  { .vid = 0x08ff,  .pid = 0x2683, },
  { .vid = 0x08ff,  .pid = 0x2684, },
  { .vid = 0x08ff,  .pid = 0x2685, },
  { .vid = 0x08ff,  .pid = 0x2686, },
  { .vid = 0x08ff,  .pid = 0x2687, },
  { .vid = 0x08ff,  .pid = 0x2688, },
  { .vid = 0x08ff,  .pid = 0x2689, },
  { .vid = 0x08ff,  .pid = 0x268a, },
  { .vid = 0x08ff,  .pid = 0x268b, },
  { .vid = 0x08ff,  .pid = 0x268c, },
  { .vid = 0x08ff,  .pid = 0x268d, },
  { .vid = 0x08ff,  .pid = 0x268e, },
  { .vid = 0x08ff,  .pid = 0x268f, },
  { .vid = 0x08ff,  .pid = 0x2691, },
  { .vid = 0,  .pid = 0,  .driver_data = 0 },
};

static void
fpi_device_aes2660_init (FpiDeviceAes2660 *self)
{
}

static void
fpi_device_aes2660_class_init (FpiDeviceAes2660Class *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  FpImageDeviceClass *img_class = FP_IMAGE_DEVICE_CLASS (klass);
  FpiDeviceAesX660Class *aes_class = FPI_DEVICE_AES_X660_CLASS (klass);

  dev_class->id = "aes2660";
  dev_class->full_name = "AuthenTec AES2660";
  dev_class->type = FP_DEVICE_TYPE_USB;
  dev_class->id_table = id_table;
  dev_class->scan_type = FP_SCAN_TYPE_SWIPE;

  img_class->bz3_threshold = 20;

  img_class->img_width = FRAME_WIDTH + FRAME_WIDTH / 2;
  img_class->img_height = -1;

  aes_class->init_seqs[0] = aes2660_init_1;
  aes_class->init_seqs_len[0] = G_N_ELEMENTS (aes2660_init_1);
  aes_class->init_seqs[1] = aes2660_init_2;
  aes_class->init_seqs_len[1] = G_N_ELEMENTS (aes2660_init_2);
  aes_class->start_imaging_cmd = (unsigned char *) aes2660_start_imaging_cmd;
  aes_class->start_imaging_cmd_len = sizeof (aes2660_start_imaging_cmd);
  aes_class->assembling_ctx = &assembling_ctx;
}
