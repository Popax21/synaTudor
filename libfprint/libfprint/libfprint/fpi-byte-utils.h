/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *                    2002 Thomas Vander Stichele <thomas@apestaart.org>
 *
 * gstutils.h: Header for various utility functions
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

/* Define PUT and GET functions for unaligned memory */
#define _FP_GET(__data, __idx, __size, __shift) \
    (((guint##__size) (((const guint8 *) (__data))[__idx])) << (__shift))

#define _FP_PUT(__data, __idx, __size, __shift, __num) \
    (((guint8 *) (__data))[__idx] = (((guint##__size) (__num)) >> (__shift)) & 0xff)

#ifndef __GTK_DOC_IGNORE__

/**
 * FP_READ_UINT64_BE:
 * @data: memory location
 *
 * Read a 64 bit unsigned integer value in big endian format from the memory buffer.
 */

/**
 * FP_READ_UINT64_LE:
 * @data: memory location
 *
 * Read a 64 bit unsigned integer value in little endian format from the memory buffer.
 */
#define _FP_READ_UINT64_BE(data)	(_FP_GET (data, 0, 64, 56) | \
					 _FP_GET (data, 1, 64, 48) | \
					 _FP_GET (data, 2, 64, 40) | \
					 _FP_GET (data, 3, 64, 32) | \
					 _FP_GET (data, 4, 64, 24) | \
					 _FP_GET (data, 5, 64, 16) | \
					 _FP_GET (data, 6, 64,  8) | \
					 _FP_GET (data, 7, 64,  0))

#define _FP_READ_UINT64_LE(data)	(_FP_GET (data, 7, 64, 56) | \
					 _FP_GET (data, 6, 64, 48) | \
					 _FP_GET (data, 5, 64, 40) | \
					 _FP_GET (data, 4, 64, 32) | \
					 _FP_GET (data, 3, 64, 24) | \
					 _FP_GET (data, 2, 64, 16) | \
					 _FP_GET (data, 1, 64,  8) | \
					 _FP_GET (data, 0, 64,  0))

#define FP_READ_UINT64_BE(data) _fpi_slow_read64_be((const guint8 *)(data))
static inline guint64 _fpi_slow_read64_be (const guint8 * data) {
  return _FP_READ_UINT64_BE (data);
}
#define FP_READ_UINT64_LE(data) _fpi_slow_read64_le((const guint8 *)(data))
static inline guint64 _fpi_slow_read64_le (const guint8 * data) {
  return _FP_READ_UINT64_LE (data);
}

/**
 * FP_READ_UINT32_BE:
 * @data: memory location
 *
 * Read a 32 bit unsigned integer value in big endian format from the memory buffer.
 */

/**
 * FP_READ_UINT32_LE:
 * @data: memory location
 *
 * Read a 32 bit unsigned integer value in little endian format from the memory buffer.
 */
#define _FP_READ_UINT32_BE(data)	(_FP_GET (data, 0, 32, 24) | \
					 _FP_GET (data, 1, 32, 16) | \
					 _FP_GET (data, 2, 32,  8) | \
					 _FP_GET (data, 3, 32,  0))

#define _FP_READ_UINT32_LE(data)	(_FP_GET (data, 3, 32, 24) | \
					 _FP_GET (data, 2, 32, 16) | \
					 _FP_GET (data, 1, 32,  8) | \
					 _FP_GET (data, 0, 32,  0))

#define FP_READ_UINT32_BE(data) _fpi_slow_read32_be((const guint8 *)(data))
static inline guint32 _fpi_slow_read32_be (const guint8 * data) {
  return _FP_READ_UINT32_BE (data);
}
#define FP_READ_UINT32_LE(data) _fpi_slow_read32_le((const guint8 *)(data))
static inline guint32 _fpi_slow_read32_le (const guint8 * data) {
  return _FP_READ_UINT32_LE (data);
}

/**
 * FP_READ_UINT24_BE:
 * @data: memory location
 *
 * Read a 24 bit unsigned integer value in big endian format from the memory buffer.
 */
#define _FP_READ_UINT24_BE(data)       (_FP_GET (data, 0, 32, 16) | \
                                         _FP_GET (data, 1, 32,  8) | \
                                         _FP_GET (data, 2, 32,  0))

#define FP_READ_UINT24_BE(data) _fpi_slow_read24_be((const guint8 *)(data))
static inline guint32 _fpi_slow_read24_be (const guint8 * data) {
  return _FP_READ_UINT24_BE (data);
}

/**
 * FP_READ_UINT24_LE:
 * @data: memory location
 *
 * Read a 24 bit unsigned integer value in little endian format from the memory buffer.
 */
#define _FP_READ_UINT24_LE(data)       (_FP_GET (data, 2, 32, 16) | \
                                         _FP_GET (data, 1, 32,  8) | \
                                         _FP_GET (data, 0, 32,  0))

#define FP_READ_UINT24_LE(data) _fpi_slow_read24_le((const guint8 *)(data))
static inline guint32 _fpi_slow_read24_le (const guint8 * data) {
  return _FP_READ_UINT24_LE (data);
}

/**
 * FP_READ_UINT16_BE:
 * @data: memory location
 *
 * Read a 16 bit unsigned integer value in big endian format from the memory buffer.
 */
/**
 * FP_READ_UINT16_LE:
 * @data: memory location
 *
 * Read a 16 bit unsigned integer value in little endian format from the memory buffer.
 */
#define _FP_READ_UINT16_BE(data)	(_FP_GET (data, 0, 16,  8) | \
					 _FP_GET (data, 1, 16,  0))

#define _FP_READ_UINT16_LE(data)	(_FP_GET (data, 1, 16,  8) | \
					 _FP_GET (data, 0, 16,  0))

#define FP_READ_UINT16_BE(data) _fpi_slow_read16_be((const guint8 *)(data))
static inline guint16 _fpi_slow_read16_be (const guint8 * data) {
  return _FP_READ_UINT16_BE (data);
}
#define FP_READ_UINT16_LE(data) _fpi_slow_read16_le((const guint8 *)(data))
static inline guint16 _fpi_slow_read16_le (const guint8 * data) {
  return _FP_READ_UINT16_LE (data);
}

/**
 * FP_READ_UINT8:
 * @data: memory location
 *
 * Read an 8 bit unsigned integer value from the memory buffer.
 */
#define FP_READ_UINT8(data)            (_FP_GET (data, 0,  8,  0))

/**
 * FP_WRITE_UINT64_BE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 64 bit unsigned integer value in big endian format into the memory buffer.
 */
/**
 * FP_WRITE_UINT64_LE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 64 bit unsigned integer value in little endian format into the memory buffer.
 */
#define FP_WRITE_UINT64_BE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint64 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 64, 56, __put_val); \
                                          _FP_PUT (__put_data, 1, 64, 48, __put_val); \
                                          _FP_PUT (__put_data, 2, 64, 40, __put_val); \
                                          _FP_PUT (__put_data, 3, 64, 32, __put_val); \
                                          _FP_PUT (__put_data, 4, 64, 24, __put_val); \
                                          _FP_PUT (__put_data, 5, 64, 16, __put_val); \
                                          _FP_PUT (__put_data, 6, 64,  8, __put_val); \
                                          _FP_PUT (__put_data, 7, 64,  0, __put_val); \
                                        } while (0)

#define FP_WRITE_UINT64_LE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint64 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 64,  0, __put_val); \
                                          _FP_PUT (__put_data, 1, 64,  8, __put_val); \
                                          _FP_PUT (__put_data, 2, 64, 16, __put_val); \
                                          _FP_PUT (__put_data, 3, 64, 24, __put_val); \
                                          _FP_PUT (__put_data, 4, 64, 32, __put_val); \
                                          _FP_PUT (__put_data, 5, 64, 40, __put_val); \
                                          _FP_PUT (__put_data, 6, 64, 48, __put_val); \
                                          _FP_PUT (__put_data, 7, 64, 56, __put_val); \
                                        } while (0)

/**
 * FP_WRITE_UINT32_BE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 32 bit unsigned integer value in big endian format into the memory buffer.
 */
/**
 * FP_WRITE_UINT32_LE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 32 bit unsigned integer value in little endian format into the memory buffer.
 */
#define FP_WRITE_UINT32_BE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint32 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 32, 24, __put_val); \
                                          _FP_PUT (__put_data, 1, 32, 16, __put_val); \
                                          _FP_PUT (__put_data, 2, 32,  8, __put_val); \
                                          _FP_PUT (__put_data, 3, 32,  0, __put_val); \
                                        } while (0)

#define FP_WRITE_UINT32_LE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint32 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 32,  0, __put_val); \
                                          _FP_PUT (__put_data, 1, 32,  8, __put_val); \
                                          _FP_PUT (__put_data, 2, 32, 16, __put_val); \
                                          _FP_PUT (__put_data, 3, 32, 24, __put_val); \
                                        } while (0)

/**
 * FP_WRITE_UINT24_BE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 24 bit unsigned integer value in big endian format into the memory buffer.
 */
#define FP_WRITE_UINT24_BE(data, num)  do { \
                                          gpointer __put_data = data; \
                                          guint32 __put_val = num; \
                                          _FP_PUT (__put_data, 0, 32,  16, __put_val); \
                                          _FP_PUT (__put_data, 1, 32,  8, __put_val); \
                                          _FP_PUT (__put_data, 2, 32,  0, __put_val); \
                                        } while (0)

/**
 * FP_WRITE_UINT24_LE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 24 bit unsigned integer value in little endian format into the memory buffer.
 */
#define FP_WRITE_UINT24_LE(data, num)  do { \
                                          gpointer __put_data = data; \
                                          guint32 __put_val = num; \
                                          _FP_PUT (__put_data, 0, 32,  0, __put_val); \
                                          _FP_PUT (__put_data, 1, 32,  8, __put_val); \
                                          _FP_PUT (__put_data, 2, 32,  16, __put_val); \
                                        } while (0)

/**
 * FP_WRITE_UINT16_BE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 16 bit unsigned integer value in big endian format into the memory buffer.
 */
/**
 * FP_WRITE_UINT16_LE:
 * @data: memory location
 * @val: value to store
 *
 * Store a 16 bit unsigned integer value in little endian format into the memory buffer.
 */
#define FP_WRITE_UINT16_BE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint16 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 16,  8, __put_val); \
                                          _FP_PUT (__put_data, 1, 16,  0, __put_val); \
                                        } while (0)

#define FP_WRITE_UINT16_LE(data,val)   do { \
                                          gpointer __put_data = data; \
                                          guint16 __put_val = val; \
                                          _FP_PUT (__put_data, 0, 16,  0, __put_val); \
                                          _FP_PUT (__put_data, 1, 16,  8, __put_val); \
                                        } while (0)

/**
 * FP_WRITE_UINT8:
 * @data: memory location
 * @num: value to store
 *
 * Store an 8 bit unsigned integer value into the memory buffer.
 */
#define FP_WRITE_UINT8(data, num)      do { \
                                          _FP_PUT (data, 0,  8,  0, num); \
                                        } while (0)

/* Float endianness conversion macros */

/**
 * FP_READ_FLOAT_LE:
 * @data: memory location
 *
 * Read a 32 bit float value in little endian format from the memory buffer.
 *
 * Returns: The floating point value read from @data
 */
static inline gfloat
FP_READ_FLOAT_LE(const guint8 *data)
{
  union
  {
    guint32 i;
    gfloat f;
  } u;

  u.i = FP_READ_UINT32_LE (data);
  return u.f;
}

/**
 * FP_READ_FLOAT_BE:
 * @data: memory location
 *
 * Read a 32 bit float value in big endian format from the memory buffer.
 *
 * Returns: The floating point value read from @data
 */
static inline gfloat
FP_READ_FLOAT_BE(const guint8 *data)
{
  union
  {
    guint32 i;
    gfloat f;
  } u;

  u.i = FP_READ_UINT32_BE (data);
  return u.f;
}

/**
 * FP_READ_DOUBLE_LE:
 * @data: memory location
 *
 * Read a 64 bit double value in little endian format from the memory buffer.
 *
 * Returns: The double-precision floating point value read from @data
 */
static inline gdouble
FP_READ_DOUBLE_LE(const guint8 *data)
{
  union
  {
    guint64 i;
    gdouble d;
  } u;

  u.i = FP_READ_UINT64_LE (data);
  return u.d;
}

/**
 * FP_READ_DOUBLE_BE:
 * @data: memory location
 *
 * Read a 64 bit double value in big endian format from the memory buffer.
 *
 * Returns: The double-precision floating point value read from @data
 */
static inline gdouble
FP_READ_DOUBLE_BE(const guint8 *data)
{
  union
  {
    guint64 i;
    gdouble d;
  } u;

  u.i = FP_READ_UINT64_BE (data);
  return u.d;
}

/**
 * FP_WRITE_FLOAT_LE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 32 bit float value in little endian format into the memory buffer.
 */
static inline void
FP_WRITE_FLOAT_LE(guint8 *data, gfloat num)
{
  union
  {
    guint32 i;
    gfloat f;
  } u;

  u.f = num;
  FP_WRITE_UINT32_LE (data, u.i);
}

/**
 * FP_WRITE_FLOAT_BE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 32 bit float value in big endian format into the memory buffer.
 */
static inline void
FP_WRITE_FLOAT_BE(guint8 *data, gfloat num)
{
  union
  {
    guint32 i;
    gfloat f;
  } u;

  u.f = num;
  FP_WRITE_UINT32_BE (data, u.i);
}

/**
 * FP_WRITE_DOUBLE_LE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 64 bit double value in little endian format into the memory buffer.
 */
static inline void
FP_WRITE_DOUBLE_LE(guint8 *data, gdouble num)
{
  union
  {
    guint64 i;
    gdouble d;
  } u;

  u.d = num;
  FP_WRITE_UINT64_LE (data, u.i);
}

/**
 * FP_WRITE_DOUBLE_BE:
 * @data: memory location
 * @num: value to store
 *
 * Store a 64 bit double value in big endian format into the memory buffer.
 */
static inline void
FP_WRITE_DOUBLE_BE(guint8 *data, gdouble num)
{
  union
  {
    guint64 i;
    gdouble d;
  } u;

  u.d = num;
  FP_WRITE_UINT64_BE (data, u.i);
}

G_END_DECLS

#endif /* __GTK_DOC_IGNORE__ */
