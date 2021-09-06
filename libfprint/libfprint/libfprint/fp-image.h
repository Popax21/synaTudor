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

#include <gio/gio.h>

G_BEGIN_DECLS

#define FP_TYPE_IMAGE (fp_image_get_type ())

typedef struct fp_minutia FpMinutia;

G_DECLARE_FINAL_TYPE (FpImage, fp_image, FP, IMAGE, GObject)

FpImage     *fp_image_new (gint width,
                           gint height);

guint         fp_image_get_width (FpImage *self);
guint         fp_image_get_height (FpImage *self);
gdouble       fp_image_get_ppmm (FpImage *self);

GPtrArray *   fp_image_get_minutiae (FpImage *self);

void          fp_image_detect_minutiae (FpImage            *self,
                                        GCancellable       *cancellable,
                                        GAsyncReadyCallback callback,
                                        gpointer            user_data);
gboolean      fp_image_detect_minutiae_finish (FpImage      *self,
                                               GAsyncResult *result,
                                               GError      **error);

const guchar * fp_image_get_data (FpImage *self,
                                  gsize   *len);
const guchar * fp_image_get_binarized (FpImage *self,
                                       gsize   *len);

void           fp_minutia_get_coords (FpMinutia *min,
                                      gint      *x,
                                      gint      *y);

G_END_DECLS
