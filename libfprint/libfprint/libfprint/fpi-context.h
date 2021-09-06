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

#include <gusb.h>
#include "fp-context.h"
#include "fpi-compat.h"

/**
 * fpi_get_driver_types:
 *
 * This function is purely for private used. It is solely part of the public
 * API as it is useful during build time.
 *
 * Stability: private
 * Returns: (element-type GType) (transfer container): a #GArray filled with
 *   all driver types
 */
GArray *fpi_get_driver_types (void);
