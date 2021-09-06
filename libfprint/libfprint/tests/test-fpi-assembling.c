/*
 * Example fingerprint device prints listing and deletion
 * Enrolls your right index finger and saves the print to disk
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

#include <glib.h>
#include <cairo.h>
#include "fpi-assembling.h"
#include "fpi-image.h"
#include "test-config.h"

typedef struct
{
  struct fpi_frame frame;
  cairo_surface_t *surf;
  guchar          *data;
  guint            stride;
  guint            width;
  guint            height;
  guint            x;
  guint            y;
} cairo_frame;

static unsigned char
cairo_get_pixel (struct fpi_frame_asmbl_ctx *ctx,
                 struct fpi_frame           *frame,
                 unsigned int                x,
                 unsigned int                y)
{
  cairo_frame *c_frame = (void *) frame; /* Indirect cast to avoid alignment warning. */

  x = x + c_frame->x;
  y = y + c_frame->y;

  g_assert (x < c_frame->width);
  g_assert (y < c_frame->height);

  return c_frame->data[x * 4 + y * c_frame->stride + 1];
}

static void
test_frame_assembling (void)
{
  g_autofree char *path = NULL;
  cairo_surface_t *img = NULL;
  int width, height, stride, offset;
  int test_height;
  guchar *data;
  struct fpi_frame_asmbl_ctx ctx = { 0, };

  g_autoptr(FpImage) fp_img = NULL;
  GSList *frames = NULL;

  g_assert_false (SOURCE_ROOT == NULL);
  path = g_build_path (G_DIR_SEPARATOR_S, SOURCE_ROOT, "tests", "vfs5011", "capture.png", NULL);

  img = cairo_image_surface_create_from_png (path);
  data = cairo_image_surface_get_data (img);
  width = cairo_image_surface_get_width (img);
  height = cairo_image_surface_get_height (img);
  stride = cairo_image_surface_get_stride (img);
  g_assert_cmpint (cairo_image_surface_get_format (img), ==, CAIRO_FORMAT_RGB24);

  ctx.get_pixel = cairo_get_pixel;
  ctx.frame_width = width;
  ctx.frame_height = 20;
  ctx.image_width = width;

  g_assert (height > ctx.frame_height);

  offset = 10;
  test_height = height - (height - ctx.frame_height) % offset;

  /* for now, fixed offset */
  for (int y = 0; y + ctx.frame_height < height; y += offset)
    {
      cairo_frame *frame = g_new0 (cairo_frame, 1);

      frame->surf = img;
      frame->width = width;
      frame->height = height;
      frame->stride = stride;
      frame->data = data;
      frame->x = 0;
      frame->y = y;
      //frame->y = test_height - ctx.frame_height - y;

      frames = g_slist_append (frames, frame);
    }
  //offset = -offset;

  fpi_do_movement_estimation (&ctx, frames);
  for (GSList *l = frames->next; l != NULL; l = l->next)
    {
      cairo_frame * frame = l->data;

      g_assert_cmpint (frame->frame.delta_x, ==, 0);
      g_assert_cmpint (frame->frame.delta_y, ==, offset);
    }

  fp_img = fpi_assemble_frames (&ctx, frames);
  g_assert_cmpint (fp_img->height, ==, test_height);

  /* The FpImage and cairo surface need to be identical in the test area */
  for (int y = 0; y < test_height; y++)
    for (int x = 0; x < width; x++)
      g_assert_cmpint (data[x * 4 + y * stride + 1], ==, fp_img->data[x + y * width]);

  g_slist_free_full (frames, g_free);
  cairo_surface_destroy (img);
  g_assert (1);
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/assembling/frames", test_frame_assembling);

  return g_test_run ();
}
