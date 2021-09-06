/*
 * Example fingerprint device prints listing and deletion
 * Enrolls your right index finger and saves the print to disk
 * Copyright (C) 2019 Marco Trevisan <marco.trevisan@canonical.com>
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

#define FP_COMPONENT "example-mange-prints"

#include <stdio.h>

#include <libfprint/fprint.h>
#include "utilities.h"

typedef struct _ListData
{
  GMainLoop *loop;
  int        ret_value;
  GList     *to_delete;
  gboolean   any_failed;
} ListData;

static void
list_data_free (ListData *list_data)
{
  g_list_free_full (list_data->to_delete, g_object_unref);
  g_main_loop_unref (list_data->loop);
  g_free (list_data);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (ListData, list_data_free)

static void
on_device_closed (FpDevice     *dev,
                  GAsyncResult *res,
                  gpointer      user_data)
{
  ListData *list_data = user_data;

  g_autoptr(GError) error = NULL;

  fp_device_close_finish (dev, res, &error);

  if (error)
    g_warning ("Failed closing device %s", error->message);

  g_main_loop_quit (list_data->loop);
}

typedef struct _DeleteData
{
  ListData *list_data;
  FpPrint  *print;
} DeleteData;

static void
delete_data_free (DeleteData *delete_data)
{
  g_object_unref (delete_data->print);
  g_free (delete_data);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (DeleteData, delete_data_free);

static void on_print_deleted (FpDevice     *dev,
                              GAsyncResult *res,
                              gpointer      user_data);

static void
delete_next_print (FpDevice *dev,
                   ListData *list_data)
{
  FpPrint *print;

  g_assert_nonnull (list_data->to_delete);
  print = list_data->to_delete->data;

  g_debug ("Deleting print %s", fp_print_get_description (print));
  fp_device_delete_print (dev, print, NULL,
                          (GAsyncReadyCallback) on_print_deleted, list_data);
}

static void
on_print_deleted (FpDevice     *dev,
                  GAsyncResult *res,
                  gpointer      user_data)
{
  ListData *list_data = user_data;

  g_autoptr(GError) error = NULL;
  g_autoptr(FpPrint) print = NULL;
  GList *deleted_link;

  fp_device_delete_print_finish (dev, res, &error);

  deleted_link = list_data->to_delete;
  print = g_steal_pointer (&deleted_link->data);
  list_data->to_delete = g_list_delete_link (list_data->to_delete, deleted_link);

  if (error)
    {
      g_warning ("Failed to remove print %s: %s",
                 fp_print_get_description (print), error->message);
      list_data->any_failed = TRUE;
    }
  else
    {
      g_debug ("Deleted print %s from device", fp_print_get_description (print));
    }

  if (list_data->to_delete != NULL)
    {
      delete_next_print (dev, list_data);
    }
  else
    {
      if (!list_data->any_failed)
        list_data->ret_value = EXIT_SUCCESS;

      fp_device_close (dev, NULL, (GAsyncReadyCallback) on_device_closed,
                       list_data);
    }
}

static void
on_list_completed (FpDevice     *dev,
                   GAsyncResult *res,
                   gpointer      user_data)
{
  ListData *list_data = user_data;

  g_autoptr(GPtrArray) prints = NULL;
  g_autoptr(GError) error = NULL;

  prints = fp_device_list_prints_finish (dev, res, &error);

  if (!error)
    {
      guint i;
      char buf[128];

      g_print ("Device contains %u prints\n", prints->len);

      for (i = 0; i < prints->len; ++i)
        {
          FpPrint * print = prints->pdata[i];
          const GDate *date = fp_print_get_enroll_date (print);

          g_print ("[%d] Print of %s finger for username %s", i + 1,
                   finger_to_string (fp_print_get_finger (print)),
                   fp_print_get_username (print));

          if (date)
            {
              g_date_strftime (buf, G_N_ELEMENTS (buf), "%Y-%m-%d\0", date);
              g_print (", enrolled on %s", buf);
            }

          g_print (". Description: %s\n", fp_print_get_description (print));
        }

      if (prints->len)
        {
          gint64 idx = 0;

          g_print ("Want to delete saved print? [<number>/A/n]\n> ");
          if (fgets (buf, 3, stdin))
            idx = g_ascii_strtoll (buf, NULL, 10);

          if (idx > 0 && idx <= prints->len)
            {
              FpPrint *print = prints->pdata[idx - 1];
              list_data->to_delete = g_list_prepend (list_data->to_delete,
                                                     g_object_ref (print));
            }
          else if (buf[0] == 'A')
            {
              for (i = 0; i < prints->len; ++i)
                {
                  FpPrint *print = prints->pdata[i];
                  list_data->to_delete = g_list_prepend (list_data->to_delete,
                                                         g_object_ref (print));
                }
            }
          else
            {
              if (buf[0] == 'n' || buf[0] == 'N')
                list_data->ret_value = EXIT_SUCCESS;
              else
                g_warning ("Invalid finger selected");
            }
        }

      if (list_data->to_delete)
        delete_next_print (dev, list_data);
      else
        fp_device_close (dev, NULL, (GAsyncReadyCallback) on_device_closed,
                         list_data);
    }
  else
    {
      g_warning ("Getting prints failed with error %s", error->message);
      g_main_loop_quit (list_data->loop);
    }
}

static void
on_device_opened (FpDevice     *dev,
                  GAsyncResult *res,
                  gpointer      user_data)
{
  ListData *list_data = user_data;

  g_autoptr(GError) error = NULL;

  if (!fp_device_open_finish (dev, res, &error))
    {
      g_warning ("Failed to open device: %s", error->message);
      g_main_loop_quit (list_data->loop);
      return;
    }

  if (!fp_device_has_storage (dev))
    {
      g_warning ("Device %s doesn't support storage", fp_device_get_name (dev));
      g_main_loop_quit (list_data->loop);
      return;
    }

  fp_device_list_prints (dev, NULL,
                         (GAsyncReadyCallback) on_list_completed, list_data);

}

int
main (void)
{
  g_autoptr(FpContext) ctx = NULL;
  g_autoptr(ListData) list_data = NULL;
  GPtrArray *devices;
  FpDevice *dev;

  g_print ("This program will report the prints saved in device\n");

  setenv ("G_MESSAGES_DEBUG", "all", 0);

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

  list_data = g_new0 (ListData, 1);
  list_data->ret_value = EXIT_FAILURE;
  list_data->loop = g_main_loop_new (NULL, FALSE);

  fp_device_open (dev, NULL, (GAsyncReadyCallback) on_device_opened, list_data);

  g_main_loop_run (list_data->loop);

  return list_data->ret_value;
}
