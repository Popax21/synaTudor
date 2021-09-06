/*
 * FpContext - A FPrint context
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

#pragma once

#include "fp-device.h"

G_BEGIN_DECLS

#define FP_TYPE_CONTEXT (fp_context_get_type ())
G_DECLARE_DERIVABLE_TYPE (FpContext, fp_context, FP, CONTEXT, GObject)

/**
 * FpContextClass:
 * @device_added: Called when a new device is added
 * @device_removed: Called when a device is removed
 *
 * Class structure for #FpContext instances.
 */
struct _FpContextClass
{
  GObjectClass parent_class;

  void         (*device_added)            (FpContext *context,
                                           FpDevice  *device);
  void         (*device_removed)          (FpContext *context,
                                           FpDevice  *device);
};

FpContext *fp_context_new (void);

void fp_context_enumerate (FpContext *context);

GPtrArray *fp_context_get_devices (FpContext *context);

G_END_DECLS
