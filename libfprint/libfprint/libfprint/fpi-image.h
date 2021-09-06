/*
 * FPrint Image
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

#pragma once

#include <config.h>
#include "fp-image.h"

/**
 * FpiImageFlags:
 * @FPI_IMAGE_V_FLIPPED: the image is vertically flipped
 * @FPI_IMAGE_H_FLIPPED: the image is horizontally flipped
 * @FPI_IMAGE_COLORS_INVERTED: the colours are inverted
 *
 * Flags used in an #FpImage structure to describe the contained image.
 * This is useful for image drivers as they can simply set these flags and
 * rely on the image to be normalized by libfprint before further processing.
 */
typedef enum {
  FPI_IMAGE_V_FLIPPED       = 1 << 0,
  FPI_IMAGE_H_FLIPPED       = 1 << 1,
  FPI_IMAGE_COLORS_INVERTED = 1 << 2,
  FPI_IMAGE_PARTIAL         = 1 << 3,
} FpiImageFlags;

/**
 * FpImage:
 * @width: Width of the image
 * @height: Height of the image
 * @ppmm: Pixels per millimeter
 * @flags: #FpiImageFlags for required normalization
 *
 * Structure holding an image. The public fields are only public for internal
 * use by the drivers.
 */
struct _FpImage
{
  /*< private >*/
  GObject parent;

  /*< public >*/
  guint         width;
  guint         height;

  gdouble       ppmm;

  FpiImageFlags flags;

  /*< private >*/
  guint8    *data;
  guint8    *binarized;

  GPtrArray *minutiae;
  guint      ref_count;
};

gint fpi_std_sq_dev (const guint8 *buf,
                     gint          size);
gint fpi_mean_sq_diff_norm (const guint8 *buf1,
                            const guint8 *buf2,
                            gint          size);

#if HAVE_PIXMAN
FpImage *fpi_image_resize (FpImage *orig,
                           guint    w_factor,
                           guint    h_factor);
#endif
