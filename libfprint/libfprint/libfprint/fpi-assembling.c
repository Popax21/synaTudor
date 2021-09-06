/*
 * Image assembling routines
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (C) 2013 Arseniy Lartsev <arseniy@chalmers.se>
 * Copyright (C) 2015 Vasily Khoruzhick <anarsoul@gmail.com>
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

#define FP_COMPONENT "assembling"

#include "fpi-log.h"
#include "fpi-image.h"

#include <string.h>

#include "fpi-assembling.h"

/**
 * SECTION:fpi-assembling
 * @title: Image frame assembly
 * @short_description: Image frame assembly helpers
 *
 * Those are the helpers to manipulate capture data from fingerprint readers
 * into a uniform image that can be further processed. This is usually used
 * by drivers for devices which have a small sensor and thus need to capture
 * data in small stripes.
 */

static unsigned int
calc_error (struct fpi_frame_asmbl_ctx *ctx,
            struct fpi_frame           *first_frame,
            struct fpi_frame           *second_frame,
            int                         dx,
            int                         dy)
{
  unsigned int width, height;
  unsigned int x1, y1, x2, y2, err, i, j;

  width = ctx->frame_width - (dx > 0 ? dx : -dx);
  height = ctx->frame_height - dy;

  if (height == 0 || width == 0)
    return INT_MAX;

  y1 = 0;
  y2 = dy;
  i = 0;
  err = 0;
  do
    {
      x1 = dx < 0 ? 0 : dx;
      x2 = dx < 0 ? -dx : 0;
      j = 0;

      do
        {
          unsigned char v1, v2;


          v1 = ctx->get_pixel (ctx, first_frame, x1, y1);
          v2 = ctx->get_pixel (ctx, second_frame, x2, y2);
          err += v1 > v2 ? v1 - v2 : v2 - v1;
          j++;
          x1++;
          x2++;

        }
      while (j < width);
      i++;
      y1++;
      y2++;
    }
  while (i < height);

  /* Normalize error */
  err *= (ctx->frame_height * ctx->frame_width);
  err /= (height * width);

  return err;
}

/* This function is rather CPU-intensive. It's better to use hardware
 * to detect movement direction when possible.
 */
static void
find_overlap (struct fpi_frame_asmbl_ctx *ctx,
              struct fpi_frame           *first_frame,
              struct fpi_frame           *second_frame,
              int                        *dx_out,
              int                        *dy_out,
              unsigned int               *min_error)
{
  int dx, dy;
  unsigned int err;

  *min_error = 255 * ctx->frame_height * ctx->frame_width;

  /* Seeking in horizontal and vertical dimensions,
   * for horizontal dimension we'll check only 8 pixels
   * in both directions. For vertical direction diff is
   * rarely less than 2, so start with it.
   */
  for (dy = 2; dy < ctx->frame_height; dy++)
    {
      for (dx = -8; dx < 8; dx++)
        {
          err = calc_error (ctx, first_frame, second_frame,
                            dx, dy);
          if (err < *min_error)
            {
              *min_error = err;
              *dx_out = -dx;
              *dy_out = dy;
            }
        }
    }
}

static unsigned int
do_movement_estimation (struct fpi_frame_asmbl_ctx *ctx,
                        GSList *stripes, gboolean reverse)
{
  GSList *l;
  GTimer *timer;
  guint num_frames = 1;
  struct fpi_frame *prev_stripe;
  unsigned int min_error;
  /* Max error is width * height * 255, for AES2501 which has the largest
   * sensor its 192*16*255 = 783360. So for 32bit value it's ~5482 frame before
   * we might get int overflow. Use 64bit value here to prevent integer overflow
   */
  unsigned long long total_error = 0;

  timer = g_timer_new ();

  /* Skip the first frame */
  prev_stripe = stripes->data;

  for (l = stripes->next; l != NULL; l = l->next, num_frames++)
    {
      struct fpi_frame *cur_stripe = l->data;

      if (reverse)
        {
          find_overlap (ctx, prev_stripe, cur_stripe,
                        &cur_stripe->delta_x, &cur_stripe->delta_y,
                        &min_error);
          cur_stripe->delta_y = -cur_stripe->delta_y;
          cur_stripe->delta_x = -cur_stripe->delta_x;
        }
      else
        {
          find_overlap (ctx, cur_stripe, prev_stripe,
                        &cur_stripe->delta_x, &cur_stripe->delta_y,
                        &min_error);
        }
      total_error += min_error;

      prev_stripe = cur_stripe;
    }

  g_timer_stop (timer);
  fp_dbg ("calc delta completed in %f secs", g_timer_elapsed (timer, NULL));
  g_timer_destroy (timer);

  return total_error / num_frames;
}

/**
 * fpi_do_movement_estimation:
 * @ctx: #fpi_frame_asmbl_ctx - frame assembling context
 * @stripes: a singly-linked list of #fpi_frame
 *
 * fpi_do_movement_estimation() estimates the movement between adjacent
 * frames, populating @delta_x and @delta_y values for each #fpi_frame.
 *
 * This function is used for devices that don't do movement estimation
 * in hardware. If hardware movement estimation is supported, the driver
 * should populate @delta_x and @delta_y instead.
 */
void
fpi_do_movement_estimation (struct fpi_frame_asmbl_ctx *ctx,
                            GSList                     *stripes)
{
  int err, rev_err;

  err = do_movement_estimation (ctx, stripes, FALSE);
  rev_err = do_movement_estimation (ctx, stripes, TRUE);
  fp_dbg ("errors: %d rev: %d", err, rev_err);
  if (err < rev_err)
    do_movement_estimation (ctx, stripes, FALSE);
}

static inline void
aes_blit_stripe (struct fpi_frame_asmbl_ctx *ctx,
                 FpImage *img,
                 struct fpi_frame *stripe,
                 int x, int y)
{
  unsigned int ix, iy;
  unsigned int fx, fy;
  unsigned int width, height;

  /* Find intersection */
  if (x < 0)
    {
      width = ctx->frame_width + x;
      ix = 0;
      fx = -x;
    }
  else
    {
      ix = x;
      fx = 0;
      width = ctx->frame_width;
    }
  if ((ix + width) > img->width)
    width = img->width - ix;

  if (y < 0)
    {
      iy = 0;
      fy = -y;
      height = ctx->frame_height + y;
    }
  else
    {
      iy = y;
      fy = 0;
      height = ctx->frame_height;
    }

  if (fx > ctx->frame_width)
    return;

  if (fy > ctx->frame_height)
    return;

  if (ix > img->width)
    return;

  if (iy > img->height)
    return;

  if ((iy + height) > img->height)
    height = img->height - iy;

  for (; fy < height; fy++, iy++)
    {
      if (x < 0)
        {
          ix = 0;
          fx = -x;
        }
      else
        {
          ix = x;
          fx = 0;
        }
      for (; fx < width; fx++, ix++)
        img->data[ix + (iy * img->width)] = ctx->get_pixel (ctx, stripe, fx, fy);
    }
}

/**
 * fpi_assemble_frames:
 * @ctx: #fpi_frame_asmbl_ctx - frame assembling context
 * @stripes: linked list of #fpi_frame
 *
 * fpi_assemble_frames() assembles individual frames into a single image.
 * It expects @delta_x and @delta_y of #fpi_frame to be populated.
 *
 * Returns: a newly allocated #fp_img.
 */
FpImage *
fpi_assemble_frames (struct fpi_frame_asmbl_ctx *ctx,
                     GSList                     *stripes)
{
  GSList *l;
  FpImage *img;
  int height = 0;
  int y, x;
  gboolean reverse = FALSE;
  struct fpi_frame *fpi_frame;

  //FIXME g_return_if_fail
  g_return_val_if_fail (stripes != NULL, NULL);
  BUG_ON (ctx->image_width < ctx->frame_width);

  /* No offset for 1st image */
  fpi_frame = stripes->data;
  fpi_frame->delta_x = 0;
  fpi_frame->delta_y = 0;
  for (l = stripes; l != NULL; l = l->next)
    {
      fpi_frame = l->data;

      height += fpi_frame->delta_y;
    }

  fp_dbg ("height is %d", height);

  if (height < 0)
    {
      reverse = TRUE;
      height = -height;
    }

  /* For last frame */
  height += ctx->frame_height;

  /* Create buffer big enough for max image */
  img = fp_image_new (ctx->image_width, height);
  img->flags = FPI_IMAGE_COLORS_INVERTED;
  img->flags |= reverse ? 0 :  FPI_IMAGE_H_FLIPPED | FPI_IMAGE_V_FLIPPED;
  img->width = ctx->image_width;
  img->height = height;

  /* Assemble stripes */
  y = reverse ? (height - ctx->frame_height) : 0;
  x = (ctx->image_width - ctx->frame_width) / 2;

  for (l = stripes; l != NULL; l = l->next)
    {
      fpi_frame = l->data;

      y += fpi_frame->delta_y;
      x += fpi_frame->delta_x;

      aes_blit_stripe (ctx, img, fpi_frame, x, y);
    }

  return img;
}

static int
cmpint (const void *p1, const void *p2, gpointer data)
{
  int a = *((int *) p1);
  int b = *((int *) p2);

  if (a < b)
    return -1;
  else if (a == b)
    return 0;
  else
    return 1;
}

static void
median_filter (int *data, int size, int filtersize)
{
  int i;
  int *result = (int *) g_malloc0 (size * sizeof (int));
  int *sortbuf = (int *) g_malloc0 (filtersize * sizeof (int));

  for (i = 0; i < size; i++)
    {
      int i1 = i - (filtersize - 1) / 2;
      int i2 = i + (filtersize - 1) / 2;
      if (i1 < 0)
        i1 = 0;
      if (i2 >= size)
        i2 = size - 1;
      memmove (sortbuf, data + i1, (i2 - i1 + 1) * sizeof (int));
      g_qsort_with_data (sortbuf, i2 - i1 + 1, sizeof (int), cmpint, NULL);
      result[i] = sortbuf[(i2 - i1 + 1) / 2];
    }
  memmove (data, result, size * sizeof (int));
  g_free (result);
  g_free (sortbuf);
}

static void
interpolate_lines (struct fpi_line_asmbl_ctx *ctx,
                   GSList *line1, gint32 y1_f,
                   GSList *line2, gint32 y2_f,
                   unsigned char *output, gint32 yi_f,
                   int size)
{
  int i;
  unsigned char p1, p2;

  if (!line1 || !line2)
    return;

  for (i = 0; i < size; i++)
    {
      gint unscaled;
      p1 = ctx->get_pixel (ctx, line1, i);
      p2 = ctx->get_pixel (ctx, line2, i);

      unscaled = (yi_f - y1_f) * p2 + (y2_f - yi_f) * p1;
      output[i] = (unscaled) / (y2_f - y1_f);
    }
}

/**
 * fpi_assemble_lines:
 * @ctx: #fpi_frame_asmbl_ctx - frame assembling context
 * @lines: linked list of lines
 * @num_lines: number of items in @lines to process
 *
 * #fpi_assemble_lines assembles individual lines into a single image.
 * It also rescales image to account variable swiping speed.
 *
 * Note that @num_lines might be shorter than the length of the list,
 * if some lines should be skipped.
 *
 * Returns: a newly allocated #fp_img.
 */
FpImage *
fpi_assemble_lines (struct fpi_line_asmbl_ctx *ctx,
                    GSList *lines, size_t num_lines)
{
  /* Number of output lines per distance between two scanners */
  int i;
  GSList *row1, *row2;
  /* The y coordinate is tracked as a 16.16 fixed point number. All
   * variables postfixed with _f follow this format here and in
   * interpolate_lines.
   * We could also use floating point here, but using fixed point means
   * we get consistent results across architectures.
   */
  gint32 y_f = 0;
  int line_ind = 0;
  int *offsets = g_new0 (int, num_lines / 2);
  unsigned char *output = g_malloc0 (ctx->line_width * ctx->max_height);
  FpImage *img;

  g_return_val_if_fail (lines != NULL, NULL);
  g_return_val_if_fail (num_lines >= 2, NULL);

  fp_dbg ("%"G_GINT64_FORMAT, g_get_real_time ());

  row1 = lines;
  for (i = 0; (i < num_lines - 1) && row1; i += 2)
    {
      int bestmatch = i;
      int bestdiff = 0;
      int j, firstrow, lastrow;

      firstrow = i + 1;
      lastrow = MIN (i + ctx->max_search_offset, num_lines - 1);

      row2 = g_slist_next (row1);
      for (j = firstrow; j <= lastrow; j++)
        {
          int diff = ctx->get_deviation (ctx,
                                         row1,
                                         row2);
          if ((j == firstrow) || (diff < bestdiff))
            {
              bestdiff = diff;
              bestmatch = j;
            }
          row2 = g_slist_next (row2);
        }
      offsets[i / 2] = bestmatch - i;
      fp_dbg ("%d", offsets[i / 2]);
      row1 = g_slist_next (row1);
      if (row1)
        row1 = g_slist_next (row1);
    }

  median_filter (offsets, (num_lines / 2) - 1, ctx->median_filter_size);

  fp_dbg ("offsets_filtered: %"G_GINT64_FORMAT, g_get_real_time ());
  for (i = 0; i <= (num_lines / 2) - 1; i++)
    fp_dbg ("%d", offsets[i]);
  row1 = lines;
  for (i = 0; i < num_lines - 1; i++, row1 = g_slist_next (row1))
    {
      int offset = offsets[i / 2];
      if (offset > 0)
        {
          gint32 ynext_f = y_f + (ctx->resolution << 16) / offset;
          while ((line_ind << 16) < ynext_f)
            {
              if (line_ind > ctx->max_height - 1)
                goto out;
              interpolate_lines (ctx,
                                 row1, y_f,
                                 g_slist_next (row1),
                                 ynext_f,
                                 output + line_ind * ctx->line_width,
                                 line_ind << 16,
                                 ctx->line_width);
              line_ind++;
            }
          y_f = ynext_f;
        }
    }
out:
  img = fp_image_new (ctx->line_width, line_ind);
  img->height = line_ind;
  img->width = ctx->line_width;
  img->flags = FPI_IMAGE_V_FLIPPED;
  memmove (img->data, output, ctx->line_width * line_ind);
  g_free (offsets);
  g_free (output);
  return img;
}
