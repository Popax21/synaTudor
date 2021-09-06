/*
 * FpContext Unit tests
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
#include "fpi-device.h"

static void
test_context_new (void)
{
  g_autoptr(FpContext) context = fp_context_new ();
  g_assert_true (FP_CONTEXT (context));
}

static void
test_context_has_no_devices (void)
{
  g_autoptr(FpContext) context = NULL;
  GPtrArray *devices;

  context = fp_context_new ();
  devices = fp_context_get_devices (context);

  g_assert_nonnull (devices);
  g_assert_cmpuint (devices->len, ==, 0);
}

static void
test_context_has_virtual_device (void)
{
  g_autoptr(FpContext) context = NULL;
  FpDevice *virtual_device = NULL;
  GPtrArray *devices;
  unsigned int i;

  fpt_setup_virtual_device_environment (FPT_VIRTUAL_DEVICE_IMAGE);

  context = fp_context_new ();
  devices = fp_context_get_devices (context);

  g_assert_nonnull (devices);
  g_assert_cmpuint (devices->len, ==, 1);

  for (i = 0; i < devices->len; ++i)
    {
      FpDevice *device = devices->pdata[i];

      if (g_strcmp0 (fp_device_get_driver (device), "virtual_image") == 0)
        {
          virtual_device = device;
          break;
        }
    }

  g_assert_true (FP_IS_DEVICE (virtual_device));

  fpt_teardown_virtual_device_environment ();
}

static void
test_context_enumerates_new_devices (void)
{
  g_autoptr(FpContext) context = NULL;
  GPtrArray *devices;

  context = fp_context_new ();

  fpt_setup_virtual_device_environment (FPT_VIRTUAL_DEVICE_IMAGE);

  fp_context_enumerate (context);
  devices = fp_context_get_devices (context);

  g_assert_nonnull (devices);
  g_assert_cmpuint (devices->len, ==, 1);

  fpt_teardown_virtual_device_environment ();
}

#define DEV_REMOVED_CB 1
#define CTX_DEVICE_REMOVED_CB 2

static void
device_removed_cb (FpDevice *device, FptContext *tctx)
{
  g_assert_nonnull (device);
  g_assert_true (device == tctx->device);

  g_assert_null (tctx->user_data);
  tctx->user_data = GINT_TO_POINTER (DEV_REMOVED_CB);
}

static void
context_device_removed_cb (FpContext *ctx, FpDevice *device, FptContext *tctx)
{
  g_assert_nonnull (device);
  g_assert_true (device == tctx->device);

  /* "device-removed" on context is always after "removed" on device */
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);
  tctx->user_data = GINT_TO_POINTER (CTX_DEVICE_REMOVED_CB);
}

static void
test_context_remove_device_closed (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);
  gboolean removed;

  tctx->user_data = NULL;
  g_signal_connect (tctx->device, "removed", (GCallback) device_removed_cb, tctx);
  g_signal_connect (tctx->fp_context, "device-removed", (GCallback) context_device_removed_cb, tctx);

  /* Triggering remove on closed device. */
  fpi_device_remove (tctx->device);

  g_assert_nonnull (tctx->device);
  g_object_get (tctx->device, "removed", &removed, NULL);
  g_assert_true (removed);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* device-removed is dispatched from idle. */
  while (g_main_context_iteration (NULL, FALSE))
    {
    }

  /* The device is now destroyed and device-removed was called. */
  g_assert_null (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, CTX_DEVICE_REMOVED_CB);

  fpt_teardown_virtual_device_environment ();
}

static void
close_done_cb (GObject *device, GAsyncResult *res, gpointer user_data)
{
  g_autoptr(FpPrint) print = NULL;
  GError **error = user_data;

  g_assert_nonnull (error);
  g_assert_false (fp_device_close_finish (FP_DEVICE (device), res, error));
  g_assert_null (print);
  g_assert_nonnull (*error);
}

static void
test_context_remove_device_closing (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);
  g_autoptr(GError) close_error = NULL;
  g_autoptr(GError) error = NULL;
  gboolean removed;

  tctx->user_data = NULL;
  g_signal_connect (tctx->device, "removed", (GCallback) device_removed_cb, tctx);
  g_signal_connect (tctx->fp_context, "device-removed", (GCallback) context_device_removed_cb, tctx);

  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);

  /* Triggering remove on device that is being closed. */
  fp_device_close (tctx->device, NULL, close_done_cb, &close_error);
  fpi_device_remove (tctx->device);

  /* Removed but not yet notified*/
  g_assert_nonnull (tctx->device);
  g_object_get (tctx->device, "removed", &removed, NULL);
  g_assert_true (removed);
  g_assert_null (tctx->user_data);

  /* Running the mainloop now will cause the close to fail eventually. */
  while (!close_error)
    g_main_context_iteration (NULL, TRUE);
  g_assert_error (close_error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_REMOVED);

  /* Now the removed callback has been called already. */
  g_assert_nonnull (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* While device-removed needs another idle iteration. */
  while (g_main_context_iteration (NULL, FALSE))
    {
    }

  g_assert_null (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, CTX_DEVICE_REMOVED_CB);

  fpt_teardown_virtual_device_environment ();
}

static void
test_context_remove_device_open (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);
  g_autoptr(GError) error = NULL;
  gboolean removed = FALSE;

  tctx->user_data = NULL;
  g_signal_connect (tctx->fp_context, "device-removed", (GCallback) context_device_removed_cb, tctx);
  g_signal_connect (tctx->device, "removed", (GCallback) device_removed_cb, tctx);

  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);

  /* Triggering remove on open device. */
  fpi_device_remove (tctx->device);

  g_assert_nonnull (tctx->device);
  g_object_get (tctx->device, "removed", &removed, NULL);
  g_assert_true (removed);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* At this point, the "removed" cb on the device should have been called!
   * Iterating the mainloop will not change anything.
   */
  while (g_main_context_iteration (NULL, FALSE))
    {
    }
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* On close, the device will be removed from the context,
   * but only a main loop iteration later. */
  fp_device_close_sync (tctx->device, NULL, &error);
  g_assert_error (error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_REMOVED);
  g_assert_nonnull (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  while (g_main_context_iteration (NULL, FALSE))
    {
    }
  g_assert_null (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, CTX_DEVICE_REMOVED_CB);

  fpt_teardown_virtual_device_environment ();
}

static void
open_done_cb (GObject *device, GAsyncResult *res, gpointer user_data)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(FpPrint) print = NULL;
  gboolean *data = user_data;

  g_assert_true (fp_device_open_finish (FP_DEVICE (device), res, &error));
  g_assert_null (print);
  g_assert_null (error);

  *data = TRUE;
}

static void
test_context_remove_device_opening (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);
  g_autoptr(GError) close_error = NULL;
  gboolean open_done = FALSE;
  gboolean removed;

  tctx->user_data = NULL;
  g_signal_connect (tctx->device, "removed", (GCallback) device_removed_cb, tctx);
  g_signal_connect (tctx->fp_context, "device-removed", (GCallback) context_device_removed_cb, tctx);

  fp_device_open (tctx->device, NULL, open_done_cb, &open_done);
  g_assert_false (open_done);

  fpi_device_remove (tctx->device);

  /* Removed but not yet notified*/
  g_assert_nonnull (tctx->device);
  g_object_get (tctx->device, "removed", &removed, NULL);
  g_assert_true (removed);
  g_assert_null (tctx->user_data);

  /* Running the mainloop now will cause the open to *succeed* despite removal! */
  while (!open_done)
    g_main_context_iteration (NULL, TRUE);

  /* Now the removed callback has been called already. */
  g_assert_nonnull (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  fp_device_close_sync (tctx->device, NULL, &close_error);
  g_assert_error (close_error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_REMOVED);

  g_assert_nonnull (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* The device-removed signal needs an idle iteration. */
  while (g_main_context_iteration (NULL, FALSE))
    {
    }

  g_assert_null (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, CTX_DEVICE_REMOVED_CB);

  fpt_teardown_virtual_device_environment ();
}

static void
enroll_done_cb (GObject *device, GAsyncResult *res, gpointer user_data)
{
  g_autoptr(FpPrint) print = NULL;
  GError **error = user_data;

  g_assert_nonnull (error);
  print = fp_device_enroll_finish (FP_DEVICE (device), res, error);
  g_assert_null (print);
  g_assert_nonnull (*error);
}

static void
test_context_remove_device_active (void)
{
  g_autoptr(FptContext) tctx = fpt_context_new_with_virtual_device (FPT_VIRTUAL_DEVICE_IMAGE);
  g_autoptr(GError) error = NULL;
  g_autoptr(GCancellable) cancellable = NULL;
  g_autoptr(GError) enroll_error = NULL;
  FpPrint *template;
  gboolean removed = FALSE;

  tctx->user_data = NULL;
  g_signal_connect (tctx->fp_context, "device-removed", (GCallback) context_device_removed_cb, tctx);
  g_signal_connect (tctx->device, "removed", (GCallback) device_removed_cb, tctx);

  fp_device_open_sync (tctx->device, NULL, &error);
  g_assert_no_error (error);

  /* Start an enroll that we can cancel/fail later.
   * NOTE: We need to cancel explicitly as remove() does not trigger a failure!
   */
  template = fp_print_new (tctx->device);
  cancellable = g_cancellable_new ();
  fp_device_enroll (tctx->device, template, cancellable, NULL, NULL, NULL, enroll_done_cb, &enroll_error);

  /* Triggering remove on active device. */
  fpi_device_remove (tctx->device);

  /* The removed property has changed, but the cb has *not* been called yet. */
  g_assert_nonnull (tctx->device);
  g_object_get (tctx->device, "removed", &removed, NULL);
  g_assert_true (removed);
  g_assert_null (tctx->user_data);

  /* Running the mainloop now will cause the operation to fail eventually. */
  while (!enroll_error)
    g_main_context_iteration (NULL, TRUE);

  /* The virtual image device throws an PROTO error internally,
   * but we should still receive a REMOVED error here. */
  g_assert_error (enroll_error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_REMOVED);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* Now we close the device, state remains unchanged mostly. */
  fp_device_close_sync (tctx->device, NULL, &error);
  g_assert_error (error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_REMOVED);
  g_assert_nonnull (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, DEV_REMOVED_CB);

  /* And "device-removed" is called */
  while (g_main_context_iteration (NULL, FALSE))
    {
    }

  g_assert_null (tctx->device);
  g_assert_cmpint (GPOINTER_TO_INT (tctx->user_data), ==, CTX_DEVICE_REMOVED_CB);

  fpt_teardown_virtual_device_environment ();
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/context/new", test_context_new);
  g_test_add_func ("/context/no-devices", test_context_has_no_devices);
  g_test_add_func ("/context/has-virtual-device", test_context_has_virtual_device);
  g_test_add_func ("/context/enumerates-new-devices", test_context_enumerates_new_devices);
  g_test_add_func ("/context/remove-device-closed", test_context_remove_device_closed);
  g_test_add_func ("/context/remove-device-closing", test_context_remove_device_closing);
  g_test_add_func ("/context/remove-device-open", test_context_remove_device_open);
  g_test_add_func ("/context/remove-device-opening", test_context_remove_device_opening);
  g_test_add_func ("/context/remove-device-active", test_context_remove_device_active);

  return g_test_run ();
}
