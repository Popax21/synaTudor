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

#define FP_COMPONENT "image"

#include "fpi-image.h"
#include "fpi-log.h"

#include <nbis.h>

/**
 * SECTION: fp-image
 * @title: FpImage
 * @short_description: Internal Image handling routines
 *
 * Some devices will provide the image data corresponding to a print
 * this object allows accessing this data.
 */

G_DEFINE_TYPE (FpImage, fp_image, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_WIDTH,
  PROP_HEIGHT,
  N_PROPS
};

static GParamSpec *properties[N_PROPS];

FpImage *
fp_image_new (gint width, gint height)
{
  return g_object_new (FP_TYPE_IMAGE,
                       "width", width,
                       "height", height,
                       NULL);
}

static void
fp_image_finalize (GObject *object)
{
  FpImage *self = (FpImage *) object;

  g_clear_pointer (&self->data, g_free);
  g_clear_pointer (&self->binarized, g_free);
  g_clear_pointer (&self->minutiae, g_ptr_array_unref);

  G_OBJECT_CLASS (fp_image_parent_class)->finalize (object);
}

static void
fp_image_constructed (GObject *object)
{
  FpImage *self = (FpImage *) object;

  self->data = g_malloc0 (self->width * self->height);
}

static void
fp_image_get_property (GObject    *object,
                       guint       prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
  FpImage *self = FP_IMAGE (object);

  switch (prop_id)
    {
    case PROP_WIDTH:
      g_value_set_uint (value, self->width);
      break;

    case PROP_HEIGHT:
      g_value_set_uint (value, self->height);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fp_image_set_property (GObject      *object,
                       guint         prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  FpImage *self = FP_IMAGE (object);

  switch (prop_id)
    {
    case PROP_WIDTH:
      self->width = g_value_get_uint (value);
      break;

    case PROP_HEIGHT:
      self->height = g_value_get_uint (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fp_image_class_init (FpImageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fp_image_finalize;
  object_class->constructed = fp_image_constructed;
  object_class->set_property = fp_image_set_property;
  object_class->get_property = fp_image_get_property;

  properties[PROP_WIDTH] =
    g_param_spec_uint ("width",
                       "Width",
                       "The width of the image",
                       0,
                       G_MAXUINT16,
                       0,
                       G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_HEIGHT] =
    g_param_spec_uint ("height",
                       "Height",
                       "The height of the image",
                       0,
                       G_MAXUINT16,
                       0,
                       G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
fp_image_init (FpImage *self)
{
}

typedef struct
{
  GAsyncReadyCallback user_cb;
  struct fp_minutiae *minutiae;
  gint                width, height;
  gdouble             ppmm;
  FpiImageFlags       flags;
  guchar             *image;
  guchar             *binarized;
} DetectMinutiaeData;

static void
fp_image_detect_minutiae_free (DetectMinutiaeData *data)
{
  g_clear_pointer (&data->image, g_free);
  g_clear_pointer (&data->minutiae, free_minutiae);
  g_clear_pointer (&data->binarized, g_free);
  g_free (data);
}

static void
fp_image_detect_minutiae_cb (GObject      *source_object,
                             GAsyncResult *res,
                             gpointer      user_data)
{
  GTask *task = G_TASK (res);
  FpImage *image;
  DetectMinutiaeData *data = g_task_get_task_data (task);

  if (!g_task_had_error (task))
    {
      gint i;
      image = FP_IMAGE (source_object);

      image->flags = data->flags;

      g_clear_pointer (&image->data, g_free);
      image->data = g_steal_pointer (&data->image);

      g_clear_pointer (&image->binarized, g_free);
      image->binarized = g_steal_pointer (&data->binarized);

      g_clear_pointer (&image->minutiae, g_ptr_array_unref);
      image->minutiae = g_ptr_array_new_full (data->minutiae->num,
                                              (GDestroyNotify) free_minutia);

      for (i = 0; i < data->minutiae->num; i++)
        g_ptr_array_add (image->minutiae,
                         g_steal_pointer (&data->minutiae->list[i]));

      /* Don't let it delete anything. */
      data->minutiae->num = 0;
    }

  if (data->user_cb)
    data->user_cb (source_object, res, user_data);
}

static void
vflip (guint8 *data, gint width, gint height)
{
  int data_len = width * height;
  unsigned char rowbuf[width];
  int i;

  for (i = 0; i < height / 2; i++)
    {
      int offset = i * width;
      int swap_offset = data_len - (width * (i + 1));

      /* copy top row into buffer */
      memcpy (rowbuf, data + offset, width);

      /* copy lower row over upper row */
      memcpy (data + offset, data + swap_offset, width);

      /* copy buffer over lower row */
      memcpy (data + swap_offset, rowbuf, width);
    }
}

static void
hflip (guint8 *data, gint width, gint height)
{
  unsigned char rowbuf[width];
  int i, j;

  for (i = 0; i < height; i++)
    {
      int offset = i * width;

      memcpy (rowbuf, data + offset, width);
      for (j = 0; j < width; j++)
        data[offset + j] = rowbuf[width - j - 1];
    }
}

static void
invert_colors (guint8 *data, gint width, gint height)
{
  int data_len = width * height;
  int i;

  for (i = 0; i < data_len; i++)
    data[i] = 0xff - data[i];
}

static void
fp_image_detect_minutiae_thread_func (GTask        *task,
                                      gpointer      source_object,
                                      gpointer      task_data,
                                      GCancellable *cancellable)
{
  g_autoptr(GTimer) timer = NULL;
  DetectMinutiaeData *data = task_data;
  struct fp_minutiae *minutiae = NULL;
  g_autofree gint *direction_map = NULL;
  g_autofree gint *low_contrast_map = NULL;
  g_autofree gint *low_flow_map = NULL;
  g_autofree gint *high_curve_map = NULL;
  g_autofree gint *quality_map = NULL;
  g_autofree guchar *bdata = NULL;
  gint map_w, map_h;
  gint bw, bh, bd;
  gint r;
  g_autofree LFSPARMS *lfsparms = NULL;

  /* Normalize the image first */
  if (data->flags & FPI_IMAGE_H_FLIPPED)
    hflip (data->image, data->width, data->height);

  if (data->flags & FPI_IMAGE_V_FLIPPED)
    vflip (data->image, data->width, data->height);

  if (data->flags & FPI_IMAGE_COLORS_INVERTED)
    invert_colors (data->image, data->width, data->height);

  data->flags &= ~(FPI_IMAGE_H_FLIPPED | FPI_IMAGE_V_FLIPPED | FPI_IMAGE_COLORS_INVERTED);

  lfsparms = g_memdup (&g_lfsparms_V2, sizeof (LFSPARMS));
  lfsparms->remove_perimeter_pts = data->flags & FPI_IMAGE_PARTIAL ? TRUE : FALSE;

  timer = g_timer_new ();
  r = get_minutiae (&minutiae, &quality_map, &direction_map,
                    &low_contrast_map, &low_flow_map, &high_curve_map,
                    &map_w, &map_h, &bdata, &bw, &bh, &bd,
                    data->image, data->width, data->height, 8,
                    data->ppmm, lfsparms);
  g_timer_stop (timer);
  fp_dbg ("Minutiae scan completed in %f secs", g_timer_elapsed (timer, NULL));

  data->binarized = g_steal_pointer (&bdata);
  data->minutiae = minutiae;

  if (r)
    {
      fp_err ("get minutiae failed, code %d", r);
      g_task_return_new_error (task, G_IO_ERROR, G_IO_ERROR_FAILED, "Minutiae scan failed with code %d", r);
      g_object_unref (task);
      return;
    }

  if (!data->minutiae || data->minutiae->num == 0)
    {
      g_task_return_new_error (task, G_IO_ERROR, G_IO_ERROR_FAILED,
                               "No minutiae found");
      g_object_unref (task);
      return;
    }

  g_task_return_boolean (task, TRUE);
  g_object_unref (task);
}

/**
 * fp_image_get_height:
 * @self: A #FpImage
 *
 * Gets the pixel height of an image.
 *
 * Returns: the height of the image
 */
guint
fp_image_get_height (FpImage *self)
{
  return self->height;
}

/**
 * fp_image_get_width:
 * @self: A #FpImage
 *
 * Gets the pixel width of an image.
 *
 * Returns: the width of the image
 */
guint
fp_image_get_width (FpImage *self)
{
  return self->width;
}

/**
 * fp_image_get_ppmm:
 * @self: A #FpImage
 *
 * Gets the resolution of the image. Note that this is assumed to
 * be fixed to 500 points per inch (~19.685 p/mm) for most drivers.
 *
 * Returns: the resolution of the image in points per millimeter
 */
gdouble
fp_image_get_ppmm (FpImage *self)
{
  return self->ppmm;
}

/**
 * fp_image_get_data:
 * @self: A #FpImage
 * @len: (out) (optional): Return location for length or %NULL
 *
 * Gets the greyscale data for an image. This data must not be modified or
 * freed.
 *
 * Returns: (transfer none) (array length=len): The image data
 */
const guchar *
fp_image_get_data (FpImage *self, gsize *len)
{
  if (len)
    *len = self->width * self->height;

  return self->data;
}

/**
 * fp_image_get_binarized:
 * @self: A #FpImage
 * @len: (out) (optional): Return location for length or %NULL
 *
 * Gets the binarized data for an image. This data must not be modified or
 * freed. You need to first detect the minutiae using
 * fp_image_detect_minutiae().
 *
 * Returns: (transfer none) (array length=len): The binarized image data
 */
const guchar *
fp_image_get_binarized (FpImage *self, gsize *len)
{
  if (len && self->binarized)
    *len = self->width * self->height;

  return self->binarized;
}

/**
 * fp_image_get_minutiae:
 * @self: A #FpImage
 *
 * Gets the minutiae for an image. This data must not be modified or
 * freed. You need to first detect the minutiae using
 * fp_image_detect_minutiae().
 *
 * Returns: (transfer none) (element-type FpMinutia): The detected minutiae
 */
GPtrArray *
fp_image_get_minutiae (FpImage *self)
{
  return self->minutiae;
}

/**
 * fp_image_detect_minutiae:
 * @self: A #FpImage
 * @cancellable: a #GCancellable, or %NULL
 * @callback: the function to call on completion
 * @user_data: the data to pass to @callback
 *
 * Detects the minutiae found in an image.
 */
void
fp_image_detect_minutiae (FpImage            *self,
                          GCancellable       *cancellable,
                          GAsyncReadyCallback callback,
                          gpointer            user_data)
{
  GTask *task;
  DetectMinutiaeData *data = g_new0 (DetectMinutiaeData, 1);

  task = g_task_new (self, cancellable, fp_image_detect_minutiae_cb, user_data);

  data->image = g_malloc (self->width * self->height);
  memcpy (data->image, self->data, self->width * self->height);
  data->flags = self->flags;
  data->width = self->width;
  data->height = self->height;
  data->ppmm = self->ppmm;
  data->user_cb = callback;

  g_task_set_task_data (task, data, (GDestroyNotify) fp_image_detect_minutiae_free);
  g_task_run_in_thread (task, fp_image_detect_minutiae_thread_func);
}

/**
 * fp_image_detect_minutiae_finish:
 * @self: A #FpImage
 * @result: A #GAsyncResult
 * @error: Return location for errors, or %NULL to ignore
 *
 * Finish minutiae detection in an image
 *
 * Returns: %TRUE on success
 */
gboolean
fp_image_detect_minutiae_finish (FpImage      *self,
                                 GAsyncResult *result,
                                 GError      **error)
{
  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * fp_minutia_get_coords:
 * @min: A #FpMinutia
 * @x: (out): x position in image
 * @y: (out): y position in image
 *
 * Returns the coordinates of the found minutia. This is only useful for
 * debugging purposes and the API is not considered stable for production.
 */
void
fp_minutia_get_coords (FpMinutia *min, gint *x, gint *y)
{
  if (x)
    *x = min->x;
  if (y)
    *y = min->y;
}
