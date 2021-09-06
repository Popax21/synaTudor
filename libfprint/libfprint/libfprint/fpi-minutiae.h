/*
 * Internal/private definitions for libfprint
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
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

/* fp_minutia structure definition */
struct fp_minutia
{
  int    x;
  int    y;
  int    ex;
  int    ey;
  int    direction;
  double reliability;
  int    type;
  int    appearing;
  int    feature_id;
  int   *nbrs;
  int   *ridge_counts;
  int    num_nbrs;
};

/* fp_minutiae structure definition */
struct fp_minutiae
{
  int                 alloc;
  int                 num;
  struct fp_minutia **list;
};
