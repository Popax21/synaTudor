/*
 * AuthenTec AES3500/AES4000 common routines
 *
 * The AES3500 and AES4000 sensors are press-typed, and could capture
 * fingerprint images in 128x128 and 96x96 pixels respectively. They
 * share a same communication interface: a number of frames are
 * transferred and captured, from which a final image could be
 * assembled. Each frame has fixed height of 16 pixels.
 *
 * As the imaging area is a bit small, only a part of finger could be
 * captured, the detected minutiae are not so many that the NBIS
 * matching works not so good. The verification rate is very low at the
 * moment.
 *
 * This work is derived from Daniel Drake's AES4000 driver.
 *
 * Copyright (C) 2013 Juvenn Woo <machese@gmail.com>
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#pragma once
#include "fpi-image-device.h"
#include "aeslib.h"

#define AES3K_FRAME_HEIGHT 16

G_DECLARE_DERIVABLE_TYPE (FpiDeviceAes3k, fpi_device_aes3k, FPI,
                          DEVICE_AES3K, FpImageDevice)

#define FPI_TYPE_DEVICE_AES3K (fpi_device_aes3k_get_type ())

struct _FpiDeviceAes3kClass
{
  FpImageDeviceClass   parent;

  gsize                frame_width; /* image size = frame_width x frame_width */
  gsize                frame_size; /* 4 bits/pixel: frame_width x AES3K_FRAME_HEIGHT / 2 */
  gsize                frame_number; /* number of frames */
  gsize                enlarge_factor;

  gsize                data_buflen;    /* buffer length of usb bulk transfer */
  struct aes_regwrite *init_reqs;       /* initial values sent to device */
  gsize                init_reqs_len;
  struct aes_regwrite *capture_reqs;       /* capture values sent to device */
  gsize                capture_reqs_len;
};
