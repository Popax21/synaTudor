/*
 * Virtual driver for "simple" device debugging
 *
 * Copyright (C) 2019 Benjamin Berg <bberg@redhat.com>
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

#define FP_COMPONENT "virtual_device"

#include "virtual-device-private.h"
#include "fpi-log.h"

G_DEFINE_TYPE (FpDeviceVirtualDevice, fpi_device_virtual_device, FP_TYPE_DEVICE)

#define INSERT_CMD_PREFIX "INSERT "
#define REMOVE_CMD_PREFIX "REMOVE "
#define SCAN_CMD_PREFIX "SCAN "
#define ERROR_CMD_PREFIX "ERROR "
#define RETRY_CMD_PREFIX "RETRY "
#define FINGER_CMD_PREFIX "FINGER "
#define SLEEP_CMD_PREFIX "SLEEP "
#define SET_ENROLL_STAGES_PREFIX "SET_ENROLL_STAGES "
#define SET_SCAN_TYPE_PREFIX "SET_SCAN_TYPE "
#define SET_CANCELLATION_PREFIX "SET_CANCELLATION_ENABLED "
#define SET_KEEP_ALIVE_PREFIX "SET_KEEP_ALIVE "

#define LIST_CMD "LIST"
#define UNPLUG_CMD "UNPLUG"

static void
maybe_continue_current_action (FpDeviceVirtualDevice *self)
{
  FpDevice *dev = FP_DEVICE (self);

  if (self->sleep_timeout_id)
    return;

  switch (fpi_device_get_current_action (dev))
    {
    case FPI_DEVICE_ACTION_ENROLL:
      FP_DEVICE_GET_CLASS (self)->enroll (dev);
      break;

    case FPI_DEVICE_ACTION_VERIFY:
      FP_DEVICE_GET_CLASS (self)->verify (dev);
      break;

    case FPI_DEVICE_ACTION_IDENTIFY:
      FP_DEVICE_GET_CLASS (self)->identify (dev);
      break;

    case FPI_DEVICE_ACTION_LIST:
      FP_DEVICE_GET_CLASS (self)->list (dev);
      break;

    case FPI_DEVICE_ACTION_DELETE:
      FP_DEVICE_GET_CLASS (self)->delete (dev);
      break;

    case FPI_DEVICE_ACTION_OPEN:
      FP_DEVICE_GET_CLASS (self)->open (dev);
      break;

    case FPI_DEVICE_ACTION_CLOSE:
      FP_DEVICE_GET_CLASS (self)->close (dev);
      break;

    default:
      break;
    }
}

static gboolean
sleep_timeout_cb (gpointer data)
{
  FpDeviceVirtualDevice *self = data;

  self->sleep_timeout_id = 0;

  if (g_cancellable_is_cancelled (self->cancellable))
    return FALSE;

  g_debug ("Sleeping completed");
  maybe_continue_current_action (self);

  return FALSE;
}

char *
process_cmds (FpDeviceVirtualDevice * self,
              gboolean                scan,
              GError                **error)
{
  if (g_cancellable_is_cancelled (self->cancellable) ||
      (fpi_device_get_current_action (FP_DEVICE (self)) != FPI_DEVICE_ACTION_NONE &&
       g_cancellable_is_cancelled (fpi_device_get_cancellable (FP_DEVICE (self)))))
    {
      g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_CANCELLED,
                           "Operation was cancelled");
      return NULL;
    }

  while (self->pending_commands->len > 0)
    {
      gchar *cmd = g_ptr_array_index (self->pending_commands, 0);

      g_debug ("Processing command %s", cmd);

      /* These are always processed. */
      if (g_str_has_prefix (cmd, INSERT_CMD_PREFIX))
        {
          g_assert (self->prints_storage);
          g_hash_table_add (self->prints_storage,
                            g_strdup (cmd + strlen (INSERT_CMD_PREFIX)));

          g_ptr_array_remove_index (self->pending_commands, 0);
          continue;
        }
      else if (g_str_has_prefix (cmd, REMOVE_CMD_PREFIX))
        {
          g_assert (self->prints_storage);
          if (!g_hash_table_remove (self->prints_storage,
                                    cmd + strlen (REMOVE_CMD_PREFIX)))
            g_warning ("ID %s was not found in storage", cmd + strlen (REMOVE_CMD_PREFIX));

          g_ptr_array_remove_index (self->pending_commands, 0);
          continue;
        }
      else if (g_str_has_prefix (cmd, SLEEP_CMD_PREFIX))
        {
          guint64 sleep_ms = g_ascii_strtoull (cmd + strlen (SLEEP_CMD_PREFIX), NULL, 10);

          g_debug ("Sleeping %lums", sleep_ms);
          self->sleep_timeout_id = g_timeout_add (sleep_ms, sleep_timeout_cb, self);
          g_ptr_array_remove_index (self->pending_commands, 0);

          return NULL;
        }
      else if (g_str_has_prefix (cmd, ERROR_CMD_PREFIX))
        {
          g_propagate_error (error,
                             fpi_device_error_new (g_ascii_strtoull (cmd + strlen (ERROR_CMD_PREFIX), NULL, 10)));

          g_ptr_array_remove_index (self->pending_commands, 0);
          return NULL;
        }

      /* If we are not scanning, then we have to stop here. */
      if (!scan)
        {
          g_warning ("Could not process command: %s", cmd);
          g_ptr_array_remove_index (self->pending_commands, 0);
          break;
        }

      if (g_str_has_prefix (cmd, SCAN_CMD_PREFIX))
        {
          char *res = g_strdup (cmd + strlen (SCAN_CMD_PREFIX));

          g_ptr_array_remove_index (self->pending_commands, 0);
          return res;
        }
      else if (g_str_has_prefix (cmd, RETRY_CMD_PREFIX))
        {
          g_propagate_error (error,
                             fpi_device_retry_new (g_ascii_strtoull (cmd + strlen (RETRY_CMD_PREFIX), NULL, 10)));

          g_ptr_array_remove_index (self->pending_commands, 0);
          return NULL;
        }
      else if (g_str_has_prefix (cmd, FINGER_CMD_PREFIX))
        {
          gboolean finger_present;

          finger_present = g_ascii_strtoull (cmd + strlen (FINGER_CMD_PREFIX), NULL, 10) != 0;
          fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                                   finger_present ? FP_FINGER_STATUS_PRESENT : FP_FINGER_STATUS_NONE,
                                                   finger_present ? FP_FINGER_STATUS_NONE : FP_FINGER_STATUS_PRESENT);

          g_ptr_array_remove_index (self->pending_commands, 0);
          continue;
        }
      else
        {
          g_warning ("Could not process command: %s", cmd);
          g_ptr_array_remove_index (self->pending_commands, 0);
        }
    }

  /* No commands left, throw a timeout error. */
  g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "No commands left that can be run!");
  return NULL;
}

static void
write_key_to_listener (void *key, void *val, void *user_data)
{
  FpiDeviceVirtualListener *listener = FPI_DEVICE_VIRTUAL_LISTENER (user_data);

  if (!fpi_device_virtual_listener_write_sync (listener, key, strlen (key), NULL) ||
      !fpi_device_virtual_listener_write_sync (listener, "\n", 1, NULL))
    g_warning ("Error writing reply to LIST command");
}

static void
recv_instruction_cb (GObject      *source_object,
                     GAsyncResult *res,
                     gpointer      user_data)
{
  g_autoptr(GError) error = NULL;
  FpiDeviceVirtualListener *listener = FPI_DEVICE_VIRTUAL_LISTENER (source_object);
  gsize bytes;

  bytes = fpi_device_virtual_listener_read_finish (listener, res, &error);
  fp_dbg ("Got instructions of length %ld", bytes);

  if (error)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Error receiving instruction data: %s", error->message);
      return;
    }

  if (bytes > 0)
    {
      FpDeviceVirtualDevice *self;
      g_autofree char *cmd = NULL;

      self = FP_DEVICE_VIRTUAL_DEVICE (user_data);

      cmd = g_strndup (self->recv_buf, bytes);
      fp_dbg ("Received command %s", cmd);

      if (g_str_has_prefix (cmd, LIST_CMD))
        {
          if (self->prints_storage)
            g_hash_table_foreach (self->prints_storage, write_key_to_listener, listener);
        }
      else if (g_str_has_prefix (cmd, UNPLUG_CMD))
        {
          fpi_device_remove (FP_DEVICE (self));
        }
      else if (g_str_has_prefix (cmd, SET_ENROLL_STAGES_PREFIX))
        {
          guint stages;

          stages = g_ascii_strtoull (cmd + strlen (SET_ENROLL_STAGES_PREFIX), NULL, 10);
          fpi_device_set_nr_enroll_stages (FP_DEVICE (self), stages);
        }
      else if (g_str_has_prefix (cmd, SET_SCAN_TYPE_PREFIX))
        {
          const char *scan_type = cmd + strlen (SET_SCAN_TYPE_PREFIX);
          g_autoptr(GEnumClass) scan_types = g_type_class_ref (fp_scan_type_get_type ());
          GEnumValue *value = g_enum_get_value_by_nick (scan_types, scan_type);

          if (value)
            fpi_device_set_scan_type (FP_DEVICE (self), value->value);
          else
            g_warning ("Scan type '%s' not found", scan_type);
        }
      else if (g_str_has_prefix (cmd, SET_CANCELLATION_PREFIX))
        {
          self->supports_cancellation = g_ascii_strtoull (
            cmd + strlen (SET_CANCELLATION_PREFIX), NULL, 10) != 0;

          g_debug ("Cancellation support toggled: %d",
                   self->supports_cancellation);
        }
      else if (g_str_has_prefix (cmd, SET_KEEP_ALIVE_PREFIX))
        {
          self->keep_alive = g_ascii_strtoull (
            cmd + strlen (SET_KEEP_ALIVE_PREFIX), NULL, 10) != 0;

          g_debug ("Keep alive toggled: %d", self->keep_alive);
        }
      else
        {
          g_ptr_array_add (self->pending_commands, g_steal_pointer (&cmd));
          g_clear_handle_id (&self->wait_command_id, g_source_remove);

          maybe_continue_current_action (self);
        }
    }

  fpi_device_virtual_listener_connection_close (listener);
}

static void
recv_instruction (FpDeviceVirtualDevice *self)
{
  fpi_device_virtual_listener_read (self->listener,
                                    FALSE,
                                    self->recv_buf,
                                    sizeof (self->recv_buf),
                                    recv_instruction_cb,
                                    self);
}

static void
on_listener_connected (FpiDeviceVirtualListener *listener,
                       gpointer                  user_data)
{
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (user_data);

  recv_instruction (self);
}

static void
dev_init (FpDevice *dev)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GCancellable) cancellable = NULL;
  g_autoptr(FpiDeviceVirtualListener) listener = NULL;
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);

  G_DEBUG_HERE ();

  process_cmds (self, FALSE, &error);
  if (error && !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      fpi_device_open_complete (dev, g_steal_pointer (&error));
      return;
    }
  else if (self->sleep_timeout_id)
    {
      return;
    }
  else if (self->listener)
    {
      fpi_device_open_complete (dev, NULL);
      return;
    }

  listener = fpi_device_virtual_listener_new ();
  cancellable = g_cancellable_new ();

  if (!fpi_device_virtual_listener_start (listener,
                                          fpi_device_get_virtual_env (FP_DEVICE (self)),
                                          cancellable,
                                          on_listener_connected,
                                          self,
                                          &error))
    {
      fpi_device_open_complete (dev, g_steal_pointer (&error));
      return;
    }

  self->listener = g_steal_pointer (&listener);
  self->cancellable = g_steal_pointer (&cancellable);

  fpi_device_open_complete (dev, NULL);
}

static gboolean
wait_for_command_timeout (gpointer data)
{
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (data);
  GError *error = NULL;

  self->wait_command_id = 0;

  switch (fpi_device_get_current_action (FP_DEVICE (self)))
    {
    case FPI_DEVICE_ACTION_LIST:
    case FPI_DEVICE_ACTION_DELETE:
      self->ignore_wait = TRUE;
      maybe_continue_current_action (self);
      self->ignore_wait = FALSE;
      return FALSE;

    default:
      break;
    }

  error = g_error_new (G_IO_ERROR, G_IO_ERROR_TIMED_OUT, "No commands arrived in time to run!");
  fpi_device_action_error (FP_DEVICE (self), error);

  return FALSE;
}

gboolean
should_wait_for_command (FpDeviceVirtualDevice *self,
                         GError                *error)
{
  if (!error && self->sleep_timeout_id)
    return TRUE;

  if (self->ignore_wait)
    return FALSE;

  if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    return FALSE;

  if (self->wait_command_id)
    return FALSE;

  self->wait_command_id = g_timeout_add (500, wait_for_command_timeout, self);
  return TRUE;
}

char *
start_scan_command (FpDeviceVirtualDevice *self,
                    GError               **error)
{
  g_autoptr(GError) local_error = NULL;
  g_autofree char *scan_id = NULL;

  if (fp_device_get_finger_status (FP_DEVICE (self)) == FP_FINGER_STATUS_NONE)
    self->injected_synthetic_cmd = FALSE;

  scan_id = process_cmds (self, TRUE, &local_error);

  if (!self->sleep_timeout_id)
    {
      fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                               FP_FINGER_STATUS_NEEDED,
                                               FP_FINGER_STATUS_NONE);
    }

  if (should_wait_for_command (self, local_error))
    {
      g_assert (!scan_id);

      g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_PENDING,
                           "Still waiting for command");
      return NULL;
    }

  if (local_error)
    g_propagate_error (error, g_steal_pointer (&local_error));
  else
    fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                             FP_FINGER_STATUS_PRESENT,
                                             FP_FINGER_STATUS_NONE);

  return g_steal_pointer (&scan_id);
}

gboolean
should_wait_to_sleep (FpDeviceVirtualDevice *self,
                      const char            *scan_id,
                      GError                *error)
{
  const gchar *cmd;

  if (self->sleep_timeout_id)
    return TRUE;

  if (!self->pending_commands->len)
    return FALSE;

  cmd = g_ptr_array_index (self->pending_commands, 0);

  if (g_str_has_prefix (cmd, SLEEP_CMD_PREFIX))
    {
      g_autoptr(GError) local_error = NULL;
      g_free (process_cmds (self, FALSE, &local_error));

      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return FALSE;

      g_assert (!self->injected_synthetic_cmd);
      g_assert (self->sleep_timeout_id != 0);

      if (!self->pending_commands->len)
        {
          g_autofree char *injected_cmd = NULL;

          if (scan_id)
            injected_cmd = g_strconcat (SCAN_CMD_PREFIX, scan_id, NULL);
          else if (error && error->domain == FP_DEVICE_ERROR)
            injected_cmd = g_strdup_printf (ERROR_CMD_PREFIX " %d", error->code);
          else if (error && error->domain == FP_DEVICE_RETRY)
            injected_cmd = g_strdup_printf (RETRY_CMD_PREFIX " %d", error->code);
          else
            return TRUE;

          g_debug ("Sleeping now, command queued for later: %s", injected_cmd);

          g_ptr_array_insert (self->pending_commands, 0, g_steal_pointer (&injected_cmd));
          self->injected_synthetic_cmd = TRUE;
        }
    }

  return self->sleep_timeout_id != 0;
}

static void
dev_verify (FpDevice *dev)
{
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);
  g_autofree char *scan_id = NULL;

  scan_id = start_scan_command (self, &error);
  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_PENDING))
    return;

  if (scan_id)
    {
      GVariant *data = NULL;
      FpPrint *new_scan;
      FpPrint *print;
      gboolean success;

      g_debug ("Virtual device scanned print %s", scan_id);
      fpi_device_get_verify_data (dev, &print);

      new_scan = fp_print_new (dev);
      fpi_print_set_type (new_scan, FPI_PRINT_RAW);
      if (self->prints_storage)
        fpi_print_set_device_stored (new_scan, TRUE);
      data = g_variant_new_string (scan_id);
      g_object_set (new_scan, "fpi-data", data, NULL);

      if (self->prints_storage && !g_hash_table_contains (self->prints_storage, scan_id))
        {
          error = fpi_device_error_new (FP_DEVICE_ERROR_DATA_NOT_FOUND);
          success = FALSE;
        }
      else
        {
          success = fp_print_equal (print, new_scan);
        }

      if (!self->match_reported)
        {
          self->match_reported = TRUE;
          fpi_device_verify_report (dev,
                                    success ? FPI_MATCH_SUCCESS : FPI_MATCH_FAIL,
                                    new_scan,
                                    NULL);
        }
    }
  else if (error)
    {
      g_debug ("Virtual device scan failed with error: %s", error->message);
    }

  fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                           FP_FINGER_STATUS_NONE,
                                           FP_FINGER_STATUS_PRESENT);

  if (error && error->domain == FP_DEVICE_RETRY)
    fpi_device_verify_report (dev, FPI_MATCH_ERROR, NULL, g_steal_pointer (&error));

  if (should_wait_to_sleep (self, scan_id, error))
    return;

  self->match_reported = FALSE;
  fpi_device_verify_complete (dev, g_steal_pointer (&error));
}

static void
dev_enroll (FpDevice *dev)
{
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);
  FpPrint *print = NULL;
  g_autofree char *id = NULL;

  id = start_scan_command (self, &error);
  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_PENDING))
    return;

  fpi_device_get_enroll_data (dev, &print);

  if (id)
    {
      GVariant *data;
      gboolean completed;

      if (self->prints_storage && g_hash_table_contains (self->prints_storage, id))
        {
          if (should_wait_to_sleep (self, id, error))
            return;

          fpi_device_enroll_complete (dev, NULL,
                                      fpi_device_error_new (FP_DEVICE_ERROR_DATA_DUPLICATE));
          return;
        }

      if (self->enroll_stages_passed == 0)
        {
          fpi_print_set_type (print, FPI_PRINT_RAW);
          data = g_variant_new_string (id);
          g_object_set (print, "fpi-data", data, NULL);
        }
      else
        {
          gboolean changed;

          g_object_get (print, "fpi-data", &data, NULL);
          changed = !g_str_equal (id, g_variant_get_string (data, NULL));
          g_variant_unref (data);

          if (changed)
            {
              g_set_error (&error, FP_DEVICE_RETRY, FP_DEVICE_RETRY_GENERAL, "ID Mismatch");
              fpi_device_enroll_progress (dev, self->enroll_stages_passed, NULL,
                                          g_steal_pointer (&error));

              if (!should_wait_to_sleep (self, id, error))
                self->sleep_timeout_id = g_idle_add (sleep_timeout_cb, self);
              return;
            }
        }

      self->enroll_stages_passed++;
      completed = self->enroll_stages_passed == fp_device_get_nr_enroll_stages (FP_DEVICE (self));
      fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                               completed ?
                                               FP_FINGER_STATUS_NEEDED :
                                               FP_FINGER_STATUS_NONE,
                                               FP_FINGER_STATUS_PRESENT);

      fpi_device_enroll_progress (dev, self->enroll_stages_passed, print, NULL);

      if (completed)
        {
          if (self->prints_storage)
            {
              fpi_print_set_device_stored (print, TRUE);
              g_hash_table_add (self->prints_storage, g_strdup (id));
            }

          fpi_device_enroll_complete (dev, g_object_ref (print), NULL);
          self->enroll_stages_passed = 0;
        }
      else if (!should_wait_to_sleep (self, id, error))
        {
          self->sleep_timeout_id = g_idle_add (sleep_timeout_cb, self);
        }
    }
  else
    {
      fpi_device_report_finger_status_changes (FP_DEVICE (self),
                                               FP_FINGER_STATUS_NONE,
                                               FP_FINGER_STATUS_PRESENT);

      if (error && error->domain == FP_DEVICE_RETRY)
        {
          fpi_device_enroll_progress (dev, self->enroll_stages_passed, NULL, g_steal_pointer (&error));

          if (!should_wait_to_sleep (self, id, error))
            self->sleep_timeout_id = g_idle_add (sleep_timeout_cb, self);
        }
      else
        {
          if (should_wait_to_sleep (self, id, error))
            return;

          self->enroll_stages_passed = 0;
          fpi_device_enroll_complete (dev, NULL, g_steal_pointer (&error));
        }
    }
}

static void
dev_cancel (FpDevice *dev)
{
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);

  if (self->injected_synthetic_cmd)
    {
      self->injected_synthetic_cmd = FALSE;
      g_ptr_array_remove_index (self->pending_commands, 0);
    }

  if (!self->supports_cancellation)
    return;

  g_debug ("Got cancellation!");
  g_clear_handle_id (&self->sleep_timeout_id, g_source_remove);

  maybe_continue_current_action (self);
}

static void
stop_listener (FpDeviceVirtualDevice *self)
{
  g_cancellable_cancel (self->cancellable);
  g_clear_object (&self->cancellable);
  g_clear_object (&self->listener);
}

static void
dev_deinit (FpDevice *dev)
{
  g_autoptr(GError) error = NULL;
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (dev);

  process_cmds (self, FALSE, &error);
  if (error && !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      fpi_device_close_complete (dev, g_steal_pointer (&error));
      return;
    }
  else if (self->sleep_timeout_id)
    {
      return;
    }

  g_clear_handle_id (&self->wait_command_id, g_source_remove);
  g_clear_handle_id (&self->sleep_timeout_id, g_source_remove);

  if (!self->keep_alive)
    stop_listener (self);

  fpi_device_close_complete (dev, NULL);
}

static void
fpi_device_virtual_device_finalize (GObject *object)
{
  FpDeviceVirtualDevice *self = FP_DEVICE_VIRTUAL_DEVICE (object);

  G_DEBUG_HERE ();
  stop_listener (self);
  g_clear_pointer (&self->pending_commands, g_ptr_array_unref);
  G_OBJECT_CLASS (fpi_device_virtual_device_parent_class)->finalize (object);
}

static void
fpi_device_virtual_device_init (FpDeviceVirtualDevice *self)
{
  self->supports_cancellation = TRUE;
  self->pending_commands = g_ptr_array_new_with_free_func (g_free);
}

static const FpIdEntry driver_ids[] = {
  { .virtual_envvar = "FP_VIRTUAL_DEVICE", },
  { .virtual_envvar = NULL }
};

static void
fpi_device_virtual_device_class_init (FpDeviceVirtualDeviceClass *klass)
{
  FpDeviceClass *dev_class = FP_DEVICE_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fpi_device_virtual_device_finalize;

  dev_class->id = FP_COMPONENT;
  dev_class->full_name = "Virtual device for debugging";
  dev_class->type = FP_DEVICE_TYPE_VIRTUAL;
  dev_class->id_table = driver_ids;
  dev_class->nr_enroll_stages = 5;

  dev_class->open = dev_init;
  dev_class->close = dev_deinit;
  dev_class->verify = dev_verify;
  dev_class->enroll = dev_enroll;
  dev_class->cancel = dev_cancel;
}
