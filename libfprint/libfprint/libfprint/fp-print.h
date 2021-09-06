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

#pragma once

#include "fp-image.h"
#include "fp-enums.h"

G_BEGIN_DECLS

#define FP_TYPE_PRINT (fp_print_get_type ())
G_DECLARE_FINAL_TYPE (FpPrint, fp_print, FP, PRINT, GInitiallyUnowned)

#define FP_FINGER_IS_VALID(finger) \
  ((finger) >= FP_FINGER_FIRST && (finger) <= FP_FINGER_LAST)

#include "fp-device.h"

/**
 * FpFinger:
 * @FP_FINGER_UNKNOWN: The finger is unknown
 * @FP_FINGER_LEFT_THUMB: Left thumb
 * @FP_FINGER_LEFT_INDEX: Left index finger
 * @FP_FINGER_LEFT_MIDDLE: Left middle finger
 * @FP_FINGER_LEFT_RING: Left ring finger
 * @FP_FINGER_LEFT_LITTLE: Left little finger
 * @FP_FINGER_RIGHT_THUMB: Right thumb
 * @FP_FINGER_RIGHT_INDEX: Right index finger
 * @FP_FINGER_RIGHT_MIDDLE: Right middle finger
 * @FP_FINGER_RIGHT_RING: Right ring finger
 * @FP_FINGER_RIGHT_LITTLE: Right little finger
 * @FP_FINGER_FIRST: The first finger in the fp-print order
 * @FP_FINGER_LAST: The last finger in the fp-print order
 */
typedef enum {
  FP_FINGER_UNKNOWN = 0,
  FP_FINGER_LEFT_THUMB,
  FP_FINGER_LEFT_INDEX,
  FP_FINGER_LEFT_MIDDLE,
  FP_FINGER_LEFT_RING,
  FP_FINGER_LEFT_LITTLE,
  FP_FINGER_RIGHT_THUMB,
  FP_FINGER_RIGHT_INDEX,
  FP_FINGER_RIGHT_MIDDLE,
  FP_FINGER_RIGHT_RING,
  FP_FINGER_RIGHT_LITTLE,

  FP_FINGER_FIRST = FP_FINGER_LEFT_THUMB,
  FP_FINGER_LAST = FP_FINGER_RIGHT_LITTLE,
} FpFinger;

/**
 * FpFingerStatusFlags:
 * @FP_FINGER_STATUS_NONE: Sensor has not the finger on it, nor requires it
 * @FP_FINGER_STATUS_NEEDED: Sensor waits for the finger
 * @FP_FINGER_STATUS_PRESENT: Sensor has the finger on it
 */
typedef enum {
  FP_FINGER_STATUS_NONE    = 0,
  FP_FINGER_STATUS_NEEDED  = 1 << 0,
  FP_FINGER_STATUS_PRESENT = 1 << 1,
} FpFingerStatusFlags;

FpPrint *fp_print_new (FpDevice *device);

const gchar *fp_print_get_driver (FpPrint *print);
const gchar *fp_print_get_device_id (FpPrint *print);
FpImage     *fp_print_get_image (FpPrint *print);

FpFinger     fp_print_get_finger (FpPrint *print);
const gchar *fp_print_get_username (FpPrint *print);
const gchar *fp_print_get_description (FpPrint *print);
const GDate *fp_print_get_enroll_date (FpPrint *print);
gboolean     fp_print_get_device_stored (FpPrint *print);

void         fp_print_set_finger (FpPrint *print,
                                  FpFinger finger);
void         fp_print_set_username (FpPrint     *print,
                                    const gchar *username);
void         fp_print_set_description (FpPrint     *print,
                                       const gchar *description);
void         fp_print_set_enroll_date (FpPrint     *print,
                                       const GDate *enroll_date);

gboolean fp_print_compatible (FpPrint  *self,
                              FpDevice *device);
gboolean fp_print_equal (FpPrint *self,
                         FpPrint *other);

gboolean fp_print_serialize (FpPrint *print,
                             guchar **data,
                             gsize   *length,
                             GError **error);

FpPrint *fp_print_deserialize (const guchar *data,
                               gsize         length,
                               GError      **error);

G_END_DECLS
