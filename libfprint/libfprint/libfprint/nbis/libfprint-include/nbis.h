/*
 * Example fingerprint device prints listing and deletion
 * Enrolls your right index finger and saves the print to disk
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"

#include <bozorth.h>
#include <bz_array.h>
#include <defs.h>
#include <lfs.h>
#include <log.h>
#include <morph.h>
#include <mytime.h>
#include <sunrast.h>

#pragma GCC diagnostic pop
