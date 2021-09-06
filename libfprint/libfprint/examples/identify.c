/*
 * Example fingerprint verification program, which verifies the
 * finger which has been previously enrolled to disk.
 * Copyright (C) 2020 Marco Trevisan <marco.trevisan@canonical.com>
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

#define FP_COMPONENT "example-identify"

#include <stdio.h>
#include <libfprint/fprint.h>
#include <glib-unix.h>

#include "storage.h"
#include "utilities.h"

typedef struct _IdentifyData
{
  GMainLoop    *loop;
  GCancellable *cancellable;
  unsigned int  sigint_handler;
  int           ret_value;
} IdentifyData;

static void
identify_data_free (IdentifyData *identify_data)
{
  g_clear_handle_id (&identify_data->sigint_handler, g_source_remove);
  g_clear_object (&identify_data->cancellable);
  g_main_loop_unref (identify_data->loop);
  g_free (identify_data);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (IdentifyData, identify_data_free)

static void
on_device_closed (FpDevice *dev, GAsyncResult *res, void *user_data)
{
  IdentifyData *identify_data = user_data;

  g_autoptr(GError) error = NULL;

  fp_device_close_finish (dev, res, &error);

  if (error)
    g_warning ("Failed closing device %s", error->message);

  g_main_loop_quit (identify_data->loop);
}

static void
identify_quit (FpDevice     *dev,
               IdentifyData *identify_data)
{
  if (!fp_device_is_open (dev))
    {
      g_main_loop_quit (identify_data->loop);
      return;
    }

  fp_device_close (dev, NULL, (GAsyncReadyCallback) on_device_closed, identify_data);
}

static void start_identification (FpDevice     *dev,
                                  IdentifyData *identify_data);

static void
on_identify_completed (FpDevice *dev, GAsyncResult *res, void *user_data)
{
  IdentifyData *identify_data = user_data;

  g_autoptr(FpPrint) print = NULL;
  g_autoptr(FpPrint) match = NULL;
  g_autoptr(GError) error = NULL;
  char buffer[20];

  if (!fp_device_identify_finish (dev, res, &match, &print, &error))
    {
      g_warning ("Failed to identify print: %s", error->message);
      identify_data->ret_value = EXIT_FAILURE;

      if (error->domain != FP_DEVICE_RETRY)
        {
          identify_quit (dev, identify_data);
          return;
        }
    }

  g_print ("Identify again? [Y/n]? ");
  if (fgets (buffer, sizeof (buffer), stdin) &&
      (buffer[0] == 'Y' || buffer[0] == 'y' || buffer[0] == '\n'))
    {
      start_identification (dev, identify_data);
      return;
    }

  identify_quit (dev, identify_data);
}

static FpPrint *
get_stored_print (FpDevice *dev, FpPrint *print)
{
  g_autoptr(GPtrArray) gallery = gallery_data_load (dev);
  guint index;

  if (g_ptr_array_find_with_equal_func (gallery, print,
                                        (GEqualFunc) fp_print_equal,
                                        &index))
    return g_object_ref (g_ptr_array_index (gallery, index));

  return NULL;
}

static void
on_identify_cb (FpDevice *dev, FpPrint *match, FpPrint *print,
                gpointer user_data, GError *error)
{
  IdentifyData *identify_data = user_data;

  if (error)
    {
      g_warning ("Identify report: No finger matched, retry error reported: %s",
                 error->message);
      return;
    }

  if (print && fp_print_get_image (print) &&
      print_image_save (print, "identify.pgm"))
    g_print ("Print image saved as identify.pgm\n");

  if (match)
    {
      g_autoptr(FpPrint) matched_print = g_object_ref (match);
      char date_str[128] = {};

      identify_data->ret_value = EXIT_SUCCESS;

      if (fp_print_get_device_stored (match))
        {
          FpPrint *stored_print = get_stored_print (dev, match);

          if (stored_print)
            matched_print = g_steal_pointer (&stored_print);
        }

      if (fp_print_get_enroll_date (matched_print))
        g_date_strftime (date_str, G_N_ELEMENTS (date_str), "%Y-%m-%d\0",
                         fp_print_get_enroll_date (matched_print));
      else
        strcpy (date_str, "<unknown>");

      g_debug ("Identify report: device %s matched finger %s successfully "
               "with print '%s', enrolled on date %s by user %s",
               fp_device_get_name (dev),
               finger_to_string (fp_print_get_finger (matched_print)),
               fp_print_get_description (matched_print), date_str,
               fp_print_get_username (matched_print));

      g_print ("IDENTIFIED!\n");
    }
  else
    {
      g_debug ("Identification report: No finger matched");
      g_print ("NOT IDENTIFIED!\n");
    }
}

static void
on_list_completed (FpDevice *dev, GAsyncResult *res, gpointer user_data)
{
  IdentifyData *identify_data = user_data;

  g_autoptr(GPtrArray) gallery = NULL;
  g_autoptr(GError) error = NULL;

  gallery = fp_device_list_prints_finish (dev, res, &error);

  if (!error)
    {
      if (!gallery->len)
        {
          g_warning ("No prints saved on device");
          identify_quit (dev, identify_data);
          return;
        }

      g_debug ("Identifying with %u prints in gallery", gallery->len);
      fp_device_identify (dev, gallery, identify_data->cancellable,
                          on_identify_cb, identify_data, NULL,
                          (GAsyncReadyCallback) on_identify_completed,
                          identify_data);
    }
  else
    {
      g_warning ("Loading prints failed with error %s", error->message);
      identify_quit (dev, identify_data);
    }
}

static void
start_identification (FpDevice *dev, IdentifyData *identify_data)
{
  if (fp_device_has_storage (dev))
    {
      g_print ("Creating finger template, using device storage...\n");
      fp_device_list_prints (dev, NULL,
                             (GAsyncReadyCallback) on_list_completed,
                             identify_data);
    }
  else
    {
      g_autoptr(GPtrArray) gallery = gallery_data_load (dev);

      if (!gallery)
        {
          identify_quit (dev, identify_data);
          return;
        }

      g_print ("Gallery loaded. Time to identify!\n");
      fp_device_identify (dev, gallery, identify_data->cancellable,
                          on_identify_cb, identify_data, NULL,
                          (GAsyncReadyCallback) on_identify_completed,
                          identify_data);
    }
}

static void
on_device_opened (FpDevice *dev, GAsyncResult *res, void *user_data)
{
  IdentifyData *identify_data = user_data;

  g_autoptr(GError) error = NULL;

  if (!fp_device_open_finish (dev, res, &error))
    {
      g_warning ("Failed to open device: %s", error->message);
      identify_quit (dev, identify_data);
      return;
    }

  g_print ("Opened device. ");

  start_identification (dev, identify_data);
}

static gboolean
sigint_cb (void *user_data)
{
  IdentifyData *identify_data = user_data;

  g_cancellable_cancel (identify_data->cancellable);

  return G_SOURCE_CONTINUE;
}

int
main (void)
{
  g_autoptr(FpContext) ctx = NULL;
  g_autoptr(IdentifyData) identify_data = NULL;
  GPtrArray *devices;
  FpDevice *dev;

  setenv ("G_MESSAGES_DEBUG", "all", 0);
  setenv ("LIBUSB_DEBUG", "3", 0);

  ctx = fp_context_new ();

  devices = fp_context_get_devices (ctx);
  if (!devices)
    {
      g_warning ("Impossible to get devices");
      return EXIT_FAILURE;
    }

  dev = discover_device (devices);
  if (!dev)
    {
      g_warning ("No devices detected.");
      return EXIT_FAILURE;
    }

  if (!fp_device_supports_identify (dev))
    {
      g_warning ("Device %s does not support identification.",
                 fp_device_get_name (dev));
      return EXIT_FAILURE;
    }

  identify_data = g_new0 (IdentifyData, 1);
  identify_data->ret_value = EXIT_FAILURE;
  identify_data->loop = g_main_loop_new (NULL, FALSE);
  identify_data->cancellable = g_cancellable_new ();
  identify_data->sigint_handler = g_unix_signal_add_full (G_PRIORITY_HIGH,
                                                          SIGINT,
                                                          sigint_cb,
                                                          identify_data,
                                                          NULL);
  fp_device_open (dev, identify_data->cancellable,
                  (GAsyncReadyCallback) on_device_opened,
                  identify_data);

  g_main_loop_run (identify_data->loop);

  return identify_data->ret_value;
}
