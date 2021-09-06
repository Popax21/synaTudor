/*
 * FPrint Image - Private APIs
 * Copyright (C) 2007 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2019 Benjamin Berg <bberg@redhat.com>
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

#define FP_COMPONENT "image"

#include "fpi-image.h"
#include "fpi-log.h"

#include <nbis.h>

#if HAVE_PIXMAN
#include <pixman.h>
#endif

/**
 * SECTION: fpi-image
 * @title: Internal FpImage
 * @short_description: Internal image handling routines
 *
 * Internal image handling routines. See #FpImage for public routines.
 */

/**
 * fpi_std_sq_dev:
 * @buf: buffer (usually bitmap, one byte per pixel)
 * @size: size of @buffer
 *
 * Calculates the squared standard deviation of the individual
 * pixels in the buffer, as per the following formula:
 * |[<!-- -->
 *    mean = sum (buf[0..size]) / size
 *    sq_dev = sum ((buf[0.size] - mean) ^ 2)
 * ]|
 * This function is usually used to determine whether image
 * is empty.
 *
 * Returns: the squared standard deviation for @buffer
 */
gint
fpi_std_sq_dev (const guint8 *buf,
                gint          size)
{
  guint64 res = 0, mean = 0;
  gint i;

  for (i = 0; i < size; i++)
    mean += buf[i];

  mean /= size;

  for (i = 0; i < size; i++)
    {
      int dev = (int) buf[i] - mean;
      res += dev * dev;
    }

  return res / size;
}

/**
 * fpi_mean_sq_diff_norm:
 * @buf1: buffer (usually bitmap, one byte per pixel)
 * @buf2: buffer (usually bitmap, one byte per pixel)
 * @size: buffer size of smallest buffer
 *
 * This function calculates the normalized mean square difference of
 * two buffers, usually two lines, as per the following formula:
 * |[<!-- -->
 *    sq_diff = sum ((buf1[0..size] - buf2[0..size]) ^ 2) / size
 * ]|
 *
 * This functions is usually used to get numerical difference
 * between two images.
 *
 * Returns: the normalized mean squared difference between @buf1 and @buf2
 */
gint
fpi_mean_sq_diff_norm (const guint8 *buf1,
                       const guint8 *buf2,
                       gint          size)
{
  int res = 0, i;

  for (i = 0; i < size; i++)
    {
      int dev = (int) buf1[i] - (int) buf2[i];
      res += dev * dev;
    }

  return res / size;
}

#if HAVE_PIXMAN
FpImage *
fpi_image_resize (FpImage *orig_img,
                  guint    w_factor,
                  guint    h_factor)
{
  int new_width = orig_img->width * w_factor;
  int new_height = orig_img->height * h_factor;
  pixman_image_t *orig, *resized;
  pixman_transform_t transform;
  FpImage *newimg;

  orig = pixman_image_create_bits (PIXMAN_a8, orig_img->width, orig_img->height, (uint32_t *) orig_img->data, orig_img->width);
  resized = pixman_image_create_bits (PIXMAN_a8, new_width, new_height, NULL, new_width);

  pixman_transform_init_identity (&transform);
  pixman_transform_scale (NULL, &transform, pixman_int_to_fixed (w_factor), pixman_int_to_fixed (h_factor));
  pixman_image_set_transform (orig, &transform);
  pixman_image_set_filter (orig, PIXMAN_FILTER_BILINEAR, NULL, 0);
  pixman_image_composite32 (PIXMAN_OP_SRC,
                            orig, /* src */
                            NULL, /* mask */
                            resized, /* dst */
                            0, 0, /* src x y */
                            0, 0, /* mask x y */
                            0, 0, /* dst x y */
                            new_width, new_height /* width height */
                           );

  newimg = fp_image_new (new_width, new_height);
  newimg->flags = orig_img->flags;

  memcpy (newimg->data, pixman_image_get_data (resized), new_width * new_height);

  pixman_image_unref (orig);
  pixman_image_unref (resized);

  return newimg;
}
#endif
