/*
 * FPrint Print handling
 * Copyright (C) 2007 Daniel Drake <dsd@gentoo.org>
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

#include "fpi-print.h"
#include "fpi-image.h"

#include <nbis.h>

struct _FpPrint
{
  GInitiallyUnowned parent_instance;

  FpiPrintType      type;

  gchar            *driver;
  gchar            *device_id;
  gboolean          device_stored;

  FpImage          *image;

  /* Metadata */
  FpFinger   finger;
  gchar     *username;
  gchar     *description;
  GDate     *enroll_date;

  GVariant  *data;
  GPtrArray *prints;
};
