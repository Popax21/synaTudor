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

#include <glib.h>

typedef enum {
  FPT_VIRTUAL_DEVICE_IMAGE = 0,
  FPT_VIRTUAL_DEVICE_NONIMAGE,
  FPT_VIRTUAL_DEVICE_NONIMAGE_STORAGE,
  FPT_NUM_VIRTUAL_DEVICE_TYPES
} FptVirtualDeviceType;

void fpt_setup_virtual_device_environment (FptVirtualDeviceType devtype);
void fpt_teardown_virtual_device_environment (void);

typedef struct _FptContext
{
  FpContext *fp_context;
  FpDevice  *device;
  gpointer   user_data;
} FptContext;

FptContext * fpt_context_new (void);
FptContext * fpt_context_new_with_virtual_device (FptVirtualDeviceType devtype);

void fpt_context_free (FptContext *test_context);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (FptContext, fpt_context_free)
