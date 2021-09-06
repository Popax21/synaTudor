/*
 * FpDevice Unit tests
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

#include "test-utils.h"

static void
on_device_opened (FpDevice *dev, GAsyncResult *res, FptContext *tctx)
{
  g_autoptr(GError) error = NULL;

  g_assert_true (fp_device_open_finish (dev, res, &error));
  g_assert_no_error (error);
  g_assert_true (fp_device_is_open (tctx->device));

  tctx->user_data = GUINT_TO_POINTER (TRUE);
}

static void
test_device_open_async (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open (tctx->device, NULL, (GAsyncReadyCallback) on_device_opened, tctx);

  while (!GPOINTER_TO_UINT (tctx->user_data))
    g_main_context_iteration (NULL, TRUE);
}

static void
on_device_closed (FpDevice *dev, GAsyncResult *res, FptContext *tctx)
{
  g_autoptr(GError) error = NULL;

  g_assert_true (fp_device_close_finish (dev, res, &error));
  g_assert_no_error (error);
  g_assert_false (fp_device_is_open (tctx->device));

  tctx->user_data = GUINT_TO_POINTER (TRUE);
}

static void
test_device_close_async (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open (tctx->device, NULL, (GAsyncReadyCallback) on_device_opened, tctx);
  while (!tctx->user_data)
    g_main_context_iteration (NULL, TRUE);

  tctx->user_data = GUINT_TO_POINTER (FALSE);
  fp_device_close (tctx->device, NULL, (GAsyncReadyCallback) on_device_closed, tctx);

  while (!GPOINTER_TO_UINT (tctx->user_data))
    g_main_context_iteration (NULL, TRUE);
}

static void
test_device_open_sync (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);
  g_assert_true (fp_device_is_open (tctx->device));

  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_error (error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_ALREADY_OPEN);
}

static void
on_open_notify (FpDevice *rdev, GParamSpec *spec, FptContext *tctx)
{
  g_assert_cmpstr (spec->name, ==, "open");
  tctx->user_data = GUINT_TO_POINTER (TRUE);
}

static void
test_device_open_sync_notify (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  g_signal_connect (tctx->device, "notify::open", G_CALLBACK (on_open_notify), tctx);
  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);
  g_assert_true (GPOINTER_TO_INT (tctx->user_data));
}

static void
test_device_close_sync (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  fp_device_close_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);
  g_assert_false (fp_device_is_open (tctx->device));

  fp_device_close_sync (tctx->device, NULL, &error);
  g_assert_error (error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_NOT_OPEN);
}

static void
on_close_notify (FpDevice *rdev, GParamSpec *spec, FptContext *tctx)
{
  g_assert_cmpstr (spec->name, ==, "open");
  tctx->user_data = GUINT_TO_POINTER (TRUE);
}

static void
test_device_close_sync_notify (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);

  g_signal_connect (tctx->device, "notify::open", G_CALLBACK (on_close_notify), tctx);
  fp_device_close_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);
  g_assert_true (GPOINTER_TO_INT (tctx->user_data));
}

static void
test_device_get_driver (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpstr (fp_device_get_driver (tctx->device), ==, "virtual_image");
}

static void
test_device_get_device_id (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpstr (fp_device_get_device_id (tctx->device), ==, "0");
}

static void
test_device_get_name (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpstr (fp_device_get_name (tctx->device), ==,
                   "Virtual image device for debugging");
}

static void
test_device_get_scan_type (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpint (fp_device_get_scan_type (tctx->device), ==, FP_SCAN_TYPE_SWIPE);
}

static void
test_device_get_finger_status (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpint (fp_device_get_finger_status (tctx->device), ==, FP_FINGER_STATUS_NONE);
}

static void
test_device_get_nr_enroll_stages (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_cmpuint (fp_device_get_nr_enroll_stages (tctx->device), ==, 5);
}

static void
test_device_supports_identify (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_true (fp_device_supports_identify (tctx->device));
}

static void
test_device_supports_capture (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_true (fp_device_supports_capture (tctx->device));
}

static void
test_device_has_storage (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_device_open_sync (tctx->device, NULL, NULL);
  g_assert_false (fp_device_has_storage (tctx->device));
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/device/async/open", test_device_open_async);
  g_test_add_func ("/device/async/close", test_device_close_async);
  g_test_add_func ("/device/sync/open", test_device_open_sync);
  g_test_add_func ("/device/sync/open/notify", test_device_open_sync_notify);
  g_test_add_func ("/device/sync/close", test_device_close_sync);
  g_test_add_func ("/device/sync/close/notify", test_device_close_sync_notify);
  g_test_add_func ("/device/sync/get_driver", test_device_get_driver);
  g_test_add_func ("/device/sync/get_device_id", test_device_get_device_id);
  g_test_add_func ("/device/sync/get_name", test_device_get_name);
  g_test_add_func ("/device/sync/get_scan_type", test_device_get_scan_type);
  g_test_add_func ("/device/sync/get_finger_status", test_device_get_finger_status);
  g_test_add_func ("/device/sync/get_nr_enroll_stages", test_device_get_nr_enroll_stages);
  g_test_add_func ("/device/sync/supports_identify", test_device_supports_identify);
  g_test_add_func ("/device/sync/supports_capture", test_device_supports_capture);
  g_test_add_func ("/device/sync/has_storage", test_device_has_storage);

  return g_test_run ();
}
