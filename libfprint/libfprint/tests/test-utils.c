/*
 * Unit tests for libfprint
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

#include <libfprint/fprint.h>
#include <glib/gstdio.h>

#include "test-utils.h"

struct
{
  const char *envvar;
  const char *driver_id;
  const char *device_id;
} devtype_vars[FPT_NUM_VIRTUAL_DEVICE_TYPES] = {
  { "FP_VIRTUAL_IMAGE", "virtual_image", "virtual_image" },               /* FPT_VIRTUAL_DEVICE_IMAGE */
  { "FP_VIRTUAL_DEVICE", "virtual_device", "virtual_device" },            /* FPT_VIRTUAL_DEVICE_NONIMAGE */
  { "FP_VIRTUAL_DEVICE_STORAGE", "virtual_device_storage", "virtual_device_storage" } /* FPT_VIRTUAL_DEVICE_NONIMAGE_STORAGE */
};

static FptVirtualDeviceType global_devtype;

void
fpt_teardown_virtual_device_environment (void)
{
  const char *path;

  path = g_getenv (devtype_vars[global_devtype].envvar);

  if (path)
    {
      g_autofree char *temp_dir = g_path_get_dirname (path);

      g_unsetenv (devtype_vars[global_devtype].envvar);
      g_unlink (path);
      g_rmdir (temp_dir);
    }
}

static void
on_signal_event (int sig)
{
  fpt_teardown_virtual_device_environment ();
}

void
fpt_setup_virtual_device_environment (FptVirtualDeviceType devtype)
{
  g_autoptr(GError) error = NULL;
  g_autofree char *temp_dir = NULL;
  g_autofree char *temp_path = NULL;
  g_autofree char *filename = NULL;

  g_assert_null (g_getenv (devtype_vars[devtype].envvar));

  temp_dir = g_dir_make_tmp ("libfprint-XXXXXX", &error);
  g_assert_no_error (error);

  filename = g_strdup_printf ("%s.socket", devtype_vars[devtype].device_id);
  temp_path = g_build_filename (temp_dir, filename, NULL);
  g_setenv (devtype_vars[devtype].envvar, temp_path, TRUE);

  global_devtype = devtype;

  signal (SIGKILL, on_signal_event);
  signal (SIGABRT, on_signal_event);
  signal (SIGSEGV, on_signal_event);
  signal (SIGTERM, on_signal_event);
  signal (SIGQUIT, on_signal_event);
  signal (SIGPIPE, on_signal_event);
}

FptContext *
fpt_context_new (void)
{
  FptContext *tctx;

  tctx = g_new0 (FptContext, 1);
  tctx->fp_context = fp_context_new ();

  return tctx;
}

FptContext *
fpt_context_new_with_virtual_device (FptVirtualDeviceType devtype)
{
  FptContext *tctx;
  GPtrArray *devices;
  unsigned int i;

  g_assert_true (devtype >= FPT_VIRTUAL_DEVICE_IMAGE &&
                 devtype < FPT_NUM_VIRTUAL_DEVICE_TYPES);

  fpt_setup_virtual_device_environment (devtype);

  tctx = fpt_context_new ();
  devices = fp_context_get_devices (tctx->fp_context);

  g_assert_nonnull (devices);
  g_assert_cmpuint (devices->len, ==, 1);

  for (i = 0; i < devices->len; ++i)
    {
      FpDevice *device = devices->pdata[i];

      if (g_strcmp0 (fp_device_get_driver (device), devtype_vars[devtype].driver_id) == 0)
        {
          tctx->device = device;
          break;
        }
    }

  g_assert_true (FP_IS_DEVICE (tctx->device));
  g_object_add_weak_pointer (G_OBJECT (tctx->device), (gpointer) & tctx->device);

  return tctx;
}

void
fpt_context_free (FptContext *tctx)
{
  if (tctx->device && fp_device_is_open (tctx->device))
    {
      g_autoptr(GError) error = NULL;

      fp_device_close_sync (tctx->device, NULL, &error);
      g_assert_no_error (error);
    }

  g_clear_object (&tctx->fp_context);
  g_free (tctx);

  fpt_teardown_virtual_device_environment ();
}
