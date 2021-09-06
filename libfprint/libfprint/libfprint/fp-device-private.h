/*
 * FpDevice - A fingerprint reader device
 * Copyright (C) 2019 Benjamin Berg <bberg@redhat.com>
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

#pragma once

#include "fpi-device.h"

typedef struct
{
  FpDeviceType type;

  GUsbDevice  *usb_device;
  const gchar *virtual_env;

  gboolean     is_removed;
  gboolean     is_open;

  gchar       *device_id;
  gchar       *device_name;
  FpScanType   scan_type;

  guint64      driver_data;

  gint         nr_enroll_stages;
  GSList      *sources;

  /* We always make sure that only one task is run at a time. */
  FpiDeviceAction     current_action;
  GTask              *current_task;
  GAsyncReadyCallback current_user_cb;
  gulong              current_cancellable_id;
  GSource            *current_idle_cancel_source;
  GSource            *current_task_idle_return_source;

  /* State for tasks */
  gboolean            wait_for_finger;
  FpFingerStatusFlags finger_status;
} FpDevicePrivate;


typedef struct
{
  FpPrint         *print;

  FpEnrollProgress enroll_progress_cb;
  gpointer         enroll_progress_data;
  GDestroyNotify   enroll_progress_destroy;
} FpEnrollData;

void enroll_data_free (FpEnrollData *enroll_data);

typedef struct
{
  FpPrint       *enrolled_print;   /* verify */
  GPtrArray     *gallery;   /* identify */

  gboolean       result_reported;
  FpPrint       *match;
  FpPrint       *print;
  GError        *error;

  FpMatchCb      match_cb;
  gpointer       match_data;
  GDestroyNotify match_destroy;
} FpMatchData;

void match_data_free (FpMatchData *match_data);
