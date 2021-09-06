/*
 * FpiSsm Unit tests
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

#include "fp-device.h"
#define FP_COMPONENT "SSM"

#include "drivers_api.h"
#include "test-device-fake.h"
#include "fpi-log.h"

/* Utility functions and shared data */

static FpDevice *fake_device = NULL;

typedef struct
{
  volatile int ref_count;
  int          handler_state;
  GSList      *handlers_chain;
  gboolean     completed;
  GError      *error;
  gboolean     ssm_destroyed;
  gboolean     expected_last_state;
} FpiSsmTestData;

static gboolean
fpi_ssm_test_nullify_pointer (gpointer * nullify_location)
{
  *nullify_location = NULL;

  return G_SOURCE_REMOVE;
}

static FpiSsmTestData *
fpi_ssm_test_data_new (void)
{
  FpiSsmTestData *data = g_new0 (FpiSsmTestData, 1);

  data->ref_count = 1;
  data->handler_state = -1;

  return data;
}

static FpiSsmTestData *
fpi_ssm_test_data_ref (FpiSsmTestData *data)
{
  g_atomic_int_inc (&data->ref_count);
  return data;
}

static void
fpi_ssm_test_data_unref (FpiSsmTestData *data)
{
  if (g_atomic_int_dec_and_test (&data->ref_count))
    {
      g_clear_error (&data->error);
      g_slist_free (data->handlers_chain);
      g_free (data);
    }
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (FpiSsmTestData, fpi_ssm_test_data_unref)

static void
fpi_ssm_test_data_unref_by_ssm (FpiSsmTestData *data)
{
  data->ssm_destroyed = TRUE;

  fpi_ssm_test_data_unref (data);
}

enum {
  FPI_TEST_SSM_STATE_0,
  FPI_TEST_SSM_STATE_1,
  FPI_TEST_SSM_STATE_2,
  FPI_TEST_SSM_STATE_3,
  FPI_TEST_SSM_STATE_NUM
};

static void
test_ssm_handler (FpiSsm   *ssm,
                  FpDevice *dev)
{
  FpiSsmTestData *data;

  g_assert (dev == fake_device);
  g_assert_true (FP_IS_DEVICE (dev));

  data = fpi_ssm_get_data (ssm);
  data->handler_state = fpi_ssm_get_cur_state (ssm);
  data->handlers_chain = g_slist_append (data->handlers_chain,
                                         GINT_TO_POINTER (fpi_ssm_get_cur_state (ssm)));
}

static void
test_ssm_completed_callback (FpiSsm   *ssm,
                             FpDevice *dev,
                             GError   *error)
{
  FpiSsmTestData *data;

  g_assert (dev == fake_device);
  g_assert_true (FP_IS_DEVICE (dev));

  data = fpi_ssm_get_data (ssm);
  data->completed = TRUE;
  data->handlers_chain = g_slist_append (data->handlers_chain,
                                         GINT_TO_POINTER (fpi_ssm_get_cur_state (ssm)));
  g_clear_error (&data->error);
  data->error = error;

  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, data->expected_last_state);
}

static FpiSsm *
ssm_test_new_full (int nr_states, const char *name)
{
  FpiSsm *ssm;
  FpiSsmTestData *data;

  ssm = fpi_ssm_new_full (fake_device, test_ssm_handler, nr_states, name);
  data = fpi_ssm_test_data_new ();
  data->expected_last_state = nr_states;
  fpi_ssm_set_data (ssm, data, (GDestroyNotify) fpi_ssm_test_data_unref_by_ssm);

  return ssm;
}

static FpiSsm *
ssm_test_new (void)
{
  return ssm_test_new_full (FPI_TEST_SSM_STATE_NUM, "FPI_TEST_SSM");
}

static gboolean
test_ssm_cancel_delayed_action_delayed (gpointer data)
{
  FpiSsm *ssm = data;

  fpi_ssm_cancel_delayed_state_change (ssm);

  return G_SOURCE_REMOVE;
}

static gboolean
test_ssm_cancel_cancellable_delayed (gpointer data)
{
  g_cancellable_cancel (G_CANCELLABLE (data));

  return G_SOURCE_REMOVE;
}

/* Tests */

static void
test_ssm_new (void)
{
  FpiSsm *ssm;

  ssm = fpi_ssm_new (fake_device, test_ssm_handler, FPI_TEST_SSM_STATE_NUM);

  g_assert_null (fpi_ssm_get_data (ssm));
  g_assert_no_error (fpi_ssm_get_error (ssm));
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);

  fpi_ssm_free (ssm);
}

static void
test_ssm_new_full (void)
{
  FpiSsm *ssm;

  ssm = fpi_ssm_new_full (fake_device, test_ssm_handler,
                          FPI_TEST_SSM_STATE_NUM, "Test SSM Name");

  g_assert_null (fpi_ssm_get_data (ssm));
  g_assert_no_error (fpi_ssm_get_error (ssm));
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);

  fpi_ssm_free (ssm);
}

static void
test_ssm_new_no_handler (void)
{
  G_GNUC_UNUSED g_autoptr(FpiSsm) ssm = NULL;

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
                         "*BUG:*handler*");
  ssm = fpi_ssm_new (fake_device, NULL, FPI_TEST_SSM_STATE_NUM);
  g_test_assert_expected_messages ();
}

static void
test_ssm_new_wrong_states (void)
{
  G_GNUC_UNUSED g_autoptr(FpiSsm) ssm = NULL;

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
                         "*BUG:*nr_states*");
  ssm = fpi_ssm_new (fake_device, test_ssm_handler, -1);
  g_test_assert_expected_messages ();
}

static void
test_ssm_set_data (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  GObject *object = g_object_new (FPI_TYPE_DEVICE_FAKE, NULL);

  g_object_add_weak_pointer (object, (gpointer) & object);

  fpi_ssm_set_data (ssm, object, g_object_unref);
  g_assert (fpi_ssm_get_data (ssm) == object);

  fpi_ssm_set_data (ssm, (gpointer) 0xdeadbeef, NULL);
  g_assert (fpi_ssm_get_data (ssm) == (gpointer) 0xdeadbeef);
  g_assert_null (object);
}

static void
test_ssm_set_data_cleanup (void)
{
  FpiSsm *ssm = ssm_test_new ();
  GObject *object = g_object_new (FPI_TYPE_DEVICE_FAKE, NULL);

  g_object_add_weak_pointer (object, (gpointer) & object);

  fpi_ssm_set_data (ssm, object, g_object_unref);
  g_assert (fpi_ssm_get_data (ssm) == object);

  fpi_ssm_free (ssm);
  g_assert_null (object);
}

static void
test_ssm_start (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_assert_null (data->handlers_chain);

  fpi_ssm_start (ssm, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);
  g_assert_no_error (data->error);
  g_assert_false (data->ssm_destroyed);
}

static void
test_ssm_start_single (void)
{
  g_autoptr(FpiSsmTestData) data = NULL;
  FpiSsm *ssm;

  ssm = ssm_test_new_full (1, "FPI_TEST_SSM_SINGLE_STATE");
  data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, 0);
  g_assert_cmpint (data->handler_state, ==, 0);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, 0);

  g_assert_true (data->completed);
  g_assert_no_error (data->error);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_next (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_next_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_next_state (ssm);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_next_with_delayed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_next_state (ssm);
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);
  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_next_complete (void)
{
  FpiSsm *ssm = ssm_test_new ();

  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 4);

  fpi_ssm_next_state (ssm);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 5);

  g_assert_true (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_jump_to_state (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_jump_to_state_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_2);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_jump_to_state_with_delayed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_2, 10, NULL);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_2);
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);
  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_jump_to_state_last (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_jump_to_state_wrong (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*nr_states*");
  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_NUM + 10);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*state*");
  fpi_ssm_jump_to_state (ssm, FPI_TEST_SSM_STATE_0 - 10);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0 - 10);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0 - 10);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_mark_completed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_completed (g_steal_pointer (&ssm));

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_true (data->completed);
  g_assert_no_error (data->error);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_mark_completed_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_mark_completed (g_steal_pointer (&ssm));
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, -1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 0);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_mark_completed_with_delayed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_completed_delayed (ssm, 10, NULL);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_mark_completed (g_steal_pointer (&ssm));
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);
  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_mark_failed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_failed (g_steal_pointer (&ssm),
                       fpi_device_error_new (FP_DEVICE_ERROR_PROTO));

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_true (data->completed);
  g_assert_error (data->error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_PROTO);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_mark_failed_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_mark_failed (g_steal_pointer (&ssm),
                       fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, -1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 0);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_mark_failed_with_delayed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_mark_completed_delayed (ssm, 10, NULL);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_failed (g_steal_pointer (&ssm),
                       fpi_device_error_new (FP_DEVICE_ERROR_PROTO));
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);
  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_true (data->completed);
  g_assert_error (data->error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_PROTO);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_delayed_next (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  while (data->handler_state == FPI_TEST_SSM_STATE_0)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_next_cancel (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_delayed_action_delayed, ssm, NULL);
  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);

  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_next_cancellable (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(GCancellable) cancellable = g_cancellable_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, cancellable);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_cancellable_delayed, cancellable, NULL);

  while (!g_cancellable_is_cancelled (cancellable))
    g_main_context_iteration (NULL, TRUE);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_cancel_delayed_state_change (ssm);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_next_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, -1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 0);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  while (data->handler_state == -1)
    g_main_context_iteration (NULL, TRUE);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_next_complete (void)
{
  FpiSsm *ssm = ssm_test_new ();

  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  while (data->handler_state == FPI_TEST_SSM_STATE_0)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  while (data->handler_state == FPI_TEST_SSM_STATE_1)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  while (data->handler_state == FPI_TEST_SSM_STATE_2)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 4);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 4);

  while (!data->completed)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 5);

  g_assert_true (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_2, 10, NULL);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  while (data->handler_state == FPI_TEST_SSM_STATE_0)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_1, 10, NULL);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  while (data->handler_state == FPI_TEST_SSM_STATE_2)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state_cancel (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_2, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_delayed_action_delayed, ssm, NULL);
  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);

  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state_cancellable (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(GCancellable) cancellable = g_cancellable_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_2, 10, cancellable);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_cancellable_delayed, cancellable, NULL);

  while (!g_cancellable_is_cancelled (cancellable))
    g_main_context_iteration (NULL, TRUE);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_cancel_delayed_state_change (ssm);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_2, 10, NULL);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, -1);
  g_assert_null (data->handlers_chain);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  while (data->handler_state == -1)
    g_main_context_iteration (NULL, TRUE);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_2);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state_last (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_3, 10, NULL);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  while (data->handler_state == FPI_TEST_SSM_STATE_0)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_3);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_jump_to_state_wrong (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*nr_states*");
  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_NUM + 10, 10, NULL);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*nr_states*");
  while (g_slist_length (data->handlers_chain) == 1)
    g_main_context_iteration (NULL, TRUE);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*state*");
  fpi_ssm_jump_to_state_delayed (ssm, FPI_TEST_SSM_STATE_0 - 10, 10, NULL);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_NUM + 10);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*state*");
  while (g_slist_length (data->handlers_chain) == 2)
    g_main_context_iteration (NULL, TRUE);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0 - 10);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0 - 10);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 3);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_mark_completed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_completed_delayed (g_steal_pointer (&ssm), 10, NULL);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  while (g_slist_length (data->handlers_chain) == 1)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);
  g_assert_true (data->completed);
  g_assert_no_error (data->error);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_delayed_mark_completed_not_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsmTestData) data = fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_mark_completed_delayed (ssm, 10, NULL);
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  while (ssm != NULL)
    g_main_context_iteration (NULL, TRUE);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, -1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 0);
  g_assert_true (data->ssm_destroyed);
}

static void
test_ssm_delayed_mark_completed_cancel (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_mark_completed_delayed (ssm, 10, NULL);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_delayed_action_delayed, ssm, NULL);
  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);

  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
  g_assert_false (data->ssm_destroyed);
}

static void
test_ssm_delayed_mark_completed_cancellable (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(GCancellable) cancellable = g_cancellable_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_mark_completed_delayed (ssm, 10, cancellable);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_idle_add_full (G_PRIORITY_HIGH, test_ssm_cancel_cancellable_delayed, cancellable, NULL);

  while (!g_cancellable_is_cancelled (cancellable))
    g_main_context_iteration (NULL, TRUE);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_cancel_delayed_state_change (ssm);
  g_test_assert_expected_messages ();

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_assert_false (data->completed);
  g_assert_no_error (data->error);
}

static void
test_ssm_delayed_cancel_error (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_cancel_delayed_state_change (ssm);
  g_test_assert_expected_messages ();

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_cancel_delayed_state_change (ssm);
  g_test_assert_expected_messages ();
}

static void
test_ssm_subssm_start (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsm) subssm =
    ssm_test_new_full (FPI_TEST_SSM_STATE_NUM, "FPI_TEST_SUB_SSM");
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_autoptr(FpiSsmTestData) subdata =
    fpi_ssm_test_data_ref (fpi_ssm_get_data (subssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_start_subsm (ssm, subssm);
  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (subssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state (subssm);

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 2);
  g_assert_no_error (subdata->error);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  subdata->expected_last_state = FPI_TEST_SSM_STATE_1;
  fpi_ssm_mark_completed (g_steal_pointer (&subssm));

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 2);
  g_assert_true (subdata->ssm_destroyed);
  g_assert_no_error (subdata->error);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_false (data->ssm_destroyed);
  g_assert_no_error (data->error);
}

static void
test_ssm_subssm_mark_failed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsm) subssm =
    ssm_test_new_full (FPI_TEST_SSM_STATE_NUM, "FPI_TEST_SUB_SSM");
  g_autoptr(FpiSsmTestData) data =
    fpi_ssm_test_data_ref (fpi_ssm_get_data (ssm));
  g_autoptr(FpiSsmTestData) subdata =
    fpi_ssm_test_data_ref (fpi_ssm_get_data (subssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_start_subsm (g_steal_pointer (&ssm), subssm);
  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (subssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);

  data->expected_last_state = FPI_TEST_SSM_STATE_0;
  subdata->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_failed (g_steal_pointer (&subssm),
                       fpi_device_error_new (FP_DEVICE_ERROR_BUSY));

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);
  g_assert_true (subdata->ssm_destroyed);
  g_assert_no_error (subdata->error);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_true (data->completed);
  g_assert_true (data->ssm_destroyed);
  g_assert_error (data->error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_BUSY);
}

static void
test_ssm_subssm_start_with_started (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsm) subssm =
    ssm_test_new_full (FPI_TEST_SSM_STATE_NUM, "FPI_TEST_SUB_SSM");
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_autoptr(FpiSsmTestData) subdata =
    fpi_ssm_test_data_ref (fpi_ssm_get_data (subssm));

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_start (subssm, test_ssm_completed_callback);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*completed*");
  fpi_ssm_start_subsm (ssm, subssm);
  g_test_assert_expected_messages ();

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (subssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 2);

  subdata->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_completed (g_steal_pointer (&subssm));

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 2);
  g_assert_true (subdata->ssm_destroyed);
  g_assert_no_error (subdata->error);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_false (data->ssm_destroyed);
  g_assert_no_error (data->error);
}

static void
test_ssm_subssm_start_with_delayed (void)
{
  g_autoptr(FpiSsm) ssm = ssm_test_new ();
  g_autoptr(FpiSsm) subssm =
    ssm_test_new_full (FPI_TEST_SSM_STATE_NUM, "FPI_TEST_SUB_SSM");
  FpiSsmTestData *data = fpi_ssm_get_data (ssm);

  g_autoptr(FpiSsmTestData) subdata =
    fpi_ssm_test_data_ref (fpi_ssm_get_data (subssm));
  gpointer timeout_tracker = GUINT_TO_POINTER (TRUE);

  fpi_ssm_start (ssm, test_ssm_completed_callback);
  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 1);

  fpi_ssm_next_state_delayed (ssm, 10, NULL);

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "*BUG:*timeout*");
  fpi_ssm_start_subsm (ssm, subssm);
  g_test_assert_expected_messages ();

  g_timeout_add (100, G_SOURCE_FUNC (fpi_ssm_test_nullify_pointer), &timeout_tracker);
  while (timeout_tracker)
    g_main_context_iteration (NULL, TRUE);

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpint (fpi_ssm_get_cur_state (subssm), ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);

  subdata->expected_last_state = FPI_TEST_SSM_STATE_0;
  fpi_ssm_mark_completed (g_steal_pointer (&subssm));

  g_assert_cmpint (subdata->handler_state, ==, FPI_TEST_SSM_STATE_0);
  g_assert_cmpuint (g_slist_length (subdata->handlers_chain), ==, 1);
  g_assert_true (subdata->ssm_destroyed);
  g_assert_no_error (subdata->error);

  g_assert_cmpint (data->handler_state, ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpint (fpi_ssm_get_cur_state (ssm), ==, FPI_TEST_SSM_STATE_1);
  g_assert_cmpuint (g_slist_length (data->handlers_chain), ==, 2);

  g_assert_false (data->completed);
  g_assert_false (data->ssm_destroyed);
  g_assert_no_error (data->error);
}

int
main (int argc, char *argv[])
{
  g_autoptr(FpDevice) device = NULL;

  g_test_init (&argc, &argv, NULL);

  device = g_object_new (FPI_TYPE_DEVICE_FAKE, NULL);
  fake_device = device;
  g_object_add_weak_pointer (G_OBJECT (device), (gpointer) & fake_device);

  g_test_add_func ("/ssm/new", test_ssm_new);
  g_test_add_func ("/ssm/new/full", test_ssm_new_full);
  g_test_add_func ("/ssm/new/no_handler", test_ssm_new_no_handler);
  g_test_add_func ("/ssm/new/wrong_states", test_ssm_new_wrong_states);
  g_test_add_func ("/ssm/set_data", test_ssm_set_data);
  g_test_add_func ("/ssm/set_data/cleanup", test_ssm_set_data_cleanup);
  g_test_add_func ("/ssm/start", test_ssm_start);
  g_test_add_func ("/ssm/start/single", test_ssm_start_single);
  g_test_add_func ("/ssm/next", test_ssm_next);
  g_test_add_func ("/ssm/next/complete", test_ssm_next_complete);
  g_test_add_func ("/ssm/next/not_started", test_ssm_next_not_started);
  g_test_add_func ("/ssm/next/with_delayed", test_ssm_next_with_delayed);
  g_test_add_func ("/ssm/jump_to_state", test_ssm_jump_to_state);
  g_test_add_func ("/ssm/jump_to_state/not_started", test_ssm_jump_to_state_not_started);
  g_test_add_func ("/ssm/jump_to_state/with_delayed", test_ssm_jump_to_state_with_delayed);
  g_test_add_func ("/ssm/jump_to_state/last", test_ssm_jump_to_state_last);
  g_test_add_func ("/ssm/jump_to_state/wrong", test_ssm_jump_to_state_wrong);
  g_test_add_func ("/ssm/mark_completed", test_ssm_mark_completed);
  g_test_add_func ("/ssm/mark_completed/not_started", test_ssm_mark_completed_not_started);
  g_test_add_func ("/ssm/mark_completed/with_delayed", test_ssm_mark_completed_with_delayed);
  g_test_add_func ("/ssm/mark_failed", test_ssm_mark_failed);
  g_test_add_func ("/ssm/mark_failed/not_started", test_ssm_mark_failed_not_started);
  g_test_add_func ("/ssm/mark_failed/with_delayed", test_ssm_mark_failed_with_delayed);
  g_test_add_func ("/ssm/delayed/next", test_ssm_delayed_next);
  g_test_add_func ("/ssm/delayed/next/cancel", test_ssm_delayed_next_cancel);
  g_test_add_func ("/ssm/delayed/next/cancellable", test_ssm_delayed_next_cancellable);
  g_test_add_func ("/ssm/delayed/next/not_started", test_ssm_delayed_next_not_started);
  g_test_add_func ("/ssm/delayed/next/complete", test_ssm_delayed_next_complete);
  g_test_add_func ("/ssm/delayed/jump_to_state", test_ssm_delayed_jump_to_state);
  g_test_add_func ("/ssm/delayed/jump_to_state/cancel", test_ssm_delayed_jump_to_state_cancel);
  g_test_add_func ("/ssm/delayed/jump_to_state/cancellable", test_ssm_delayed_jump_to_state_cancellable);
  g_test_add_func ("/ssm/delayed/jump_to_state/not_started", test_ssm_delayed_jump_to_state_not_started);
  g_test_add_func ("/ssm/delayed/jump_to_state/last", test_ssm_delayed_jump_to_state_last);
  g_test_add_func ("/ssm/delayed/jump_to_state/wrong", test_ssm_delayed_jump_to_state_wrong);
  g_test_add_func ("/ssm/delayed/mark_completed", test_ssm_delayed_mark_completed);
  g_test_add_func ("/ssm/delayed/mark_completed/cancel", test_ssm_delayed_mark_completed_cancel);
  g_test_add_func ("/ssm/delayed/mark_completed/cancellable", test_ssm_delayed_mark_completed_cancellable);
  g_test_add_func ("/ssm/delayed/mark_completed/not_started", test_ssm_delayed_mark_completed_not_started);
  g_test_add_func ("/ssm/delayed/cancel/error", test_ssm_delayed_cancel_error);
  g_test_add_func ("/ssm/subssm/start", test_ssm_subssm_start);
  g_test_add_func ("/ssm/subssm/start/with_started", test_ssm_subssm_start_with_started);
  g_test_add_func ("/ssm/subssm/start/with_delayed", test_ssm_subssm_start_with_delayed);
  g_test_add_func ("/ssm/subssm/mark_failed", test_ssm_subssm_mark_failed);

  return g_test_run ();
}
