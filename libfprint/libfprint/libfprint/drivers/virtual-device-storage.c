/*
 * Virtual driver for "simple" device debugging with storage
 *
 * Copyright (C) 2020 Bastien Nocera <hadess@hadess.net>
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

/*
 * This is a virtual driver to debug the non-image based drivers. A small
 * python script is provided to connect to it via a socket, allowing
 * prints to registered programmatically.
 * Using this, it is possible to test libfprint and fprintd.
 */

#define FP_COMPONENT "virtual_device_storage"

#include "virtual-device-private.h"
#include "fpi-log.h"

G_DEFINE_TYPE (FpDeviceVirtualDeviceStorage, fpi_device_virtual_device_storage, fpi_device_virtual_device_get_type ())

static GPtrArray * get_stored_prints (FpDeviceVirtualDevice * self);

static void
dev_identify (FpDevice *dev)
{
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);
  g_autofree char *scan_id = NULL;

  scan_id = start_scan_command (self, &error);
  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_PENDING))
    return;

  if (scan_id)
    {
      g_autoptr(GPtrArray) stored = get_stored_prints (self);
      GPtrArray *prints;
      GVariant *data = NULL;
      FpPrint *new_scan;
      FpPrint *match = NULL;
      guint idx;

      new_scan = fp_print_new (dev);
      fpi_print_set_type (new_scan, FPI_PRINT_RAW);
      fpi_print_set_device_stored (new_scan, TRUE);
      data = g_variant_new_string (scan_id);
      g_object_set (new_scan, "fpi-data", data, NULL);

      fpi_device_get_identify_data (dev, &prints);
      g_debug ("Trying to identify print '%s' against a gallery of %u prints", scan_id, prints->len);

      if (!g_ptr_array_find_with_equal_func (stored,
                                             new_scan,
                                             (GEqualFunc) fp_print_equal,
                                             NULL))
        error = fpi_device_error_new (FP_DEVICE_ERROR_DATA_NOT_FOUND);
      else if (g_ptr_array_find_with_equal_func (prints,
                                                 new_scan,
                                                 (GEqualFunc) fp_print_equal,
                                                 &idx))
        match = g_ptr_array_index (prints, idx);

      if (!self->match_reported)
        {
          self->match_reported = TRUE;
          fpi_device_identify_report (dev,
                                      match,
                                      new_scan,
                                      NULL);
        }
    }
  else if (error && error->domain == FP_DEVICE_RETRY)
    {
      fpi_device_identify_report (dev, NULL, NULL, g_steal_pointer (&error));
    }

  fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                           FP_FINGER_STATUS_NONE,
                                           FP_FINGER_STATUS_PRESENT);

  if (should_wait_to_sleep (self, scan_id, error))
    return;

  self->match_reported = FALSE;
  fpi_device_identify_complete (dev, g_steal_pointer (&error));
}

struct ListData
{
  FpDevice  *dev;
  GPtrArray *res;
};

static void
dev_list_insert_print (gpointer key,
                       gpointer value,
                       gpointer user_data)
{
  struct ListData *data = user_data;
  FpPrint *print = fp_print_new (data->dev);
  GVariant *var = NULL;

  fpi_print_fill_from_user_id (print, key);
  fpi_print_set_type (print, FPI_PRINT_RAW);
  var = g_variant_new_string (key);
  g_object_set (print, "fpi-data", var, NULL);
  g_object_ref_sink (print);

  g_ptr_array_add (data->res, print);
}

static GPtrArray *
get_stored_prints (FpDeviceVirtualDevice *self)
{
  GPtrArray * prints_list;
  struct ListData data;

  prints_list = g_ptr_array_new_full (g_hash_table_size (self->prints_storage),
                                      g_object_unref);
  data.dev = FP_DEVICE (self);
  data.res = prints_list;

  g_hash_table_foreach (self->prints_storage, dev_list_insert_print, &data);

  return prints_list;
}

static void
dev_list (FpDevice *dev)
{
  g_autoptr(GPtrArray) prints_list = NULL;
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *vdev = FP_DEVICE_VIRTUAL_DEVICE (dev);

  process_cmds (vdev, FALSE, &error);
  if (should_wait_for_command (vdev, error))
    return;

  if (error && !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      fpi_device_list_complete (dev, NULL, g_steal_pointer (&error));
      return;
    }

  fpi_device_list_complete (dev, get_stored_prints (vdev), NULL);
}

static void
dev_delete (FpDevice *dev)
{
  g_autoptr(GVariant) data = NULL;
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *vdev = FP_DEVICE_VIRTUAL_DEVICE (dev);
  FpPrint *print = NULL;
  const char *id = NULL;

  process_cmds (vdev, FALSE, &error);
  if (should_wait_for_command (vdev, error))
    return;

  if (error && !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      fpi_device_delete_complete (dev, g_steal_pointer (&error));
      return;
    }

  fpi_device_get_delete_data (dev, &print);

  g_object_get (print, "fpi-data", &data, NULL);
  if (data == NULL)
    {
      fpi_device_delete_complete (dev,
                                  fpi_device_error_new (FP_DEVICE_ERROR_DATA_INVALID));
      return;
    }

  id = g_variant_get_string (data, NULL);

  fp_dbg ("Deleting print %s for user %s",
          id,
          fp_print_get_username (print));

  if (g_hash_table_remove (vdev->prints_storage, id))
    fpi_device_delete_complete (dev, NULL);
  else
    fpi_device_delete_complete (dev,
                                fpi_device_error_new (FP_DEVICE_ERROR_DATA_NOT_FOUND));
}

static void
fpi_device_virtual_device_storage_init (FpDeviceVirtualDeviceStorage *self)
{
  FpDeviceVirtualDevice *vdev = FP_DEVICE_VIRTUAL_DEVICE (self);

  vdev->prints_storage = g_hash_table_new_full (g_str_hash,
                                                g_str_equal,
                                                g_free,
                                                NULL);
}

static void
fpi_device_virtual_device_storage_finalize (GObject *object)
{
  FpDeviceVirtualDevice *vdev = FP_DEVICE_VIRTUAL_DEVICE (object);

  G_DEBUG_HERE ();
  g_clear_pointer (&vdev->prints_storage, g_hash_table_destroy);
  G_OBJECT_CLASS (fpi_device_virtual_device_storage_parent_class)->finalize (object);
}

static const FpIdEntry driver_ids[] = {
  { .virtual_envvar = "FP_VIRTUAL_DEVICE_STORAGE" },
  { .virtual_envvar = "FP_VIRTUAL_DEVICE_IDENT" },
  { .virtual_envvar = NULL }
};

static void
fpi_device_virtual_device_storage_class_init (FpDeviceVirtualDeviceStorageClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fpi_device_virtual_device_storage_finalize;

  dev_class->id = FP_COMPONENT;
  dev_class->full_name = "Virtual device with storage and identification for debugging";
  dev_class->id_table = driver_ids;

  dev_class->identify = dev_identify;
  dev_class->list = dev_list;
  dev_class->delete = dev_delete;
}
