/* GStreamer byte writer
 *
 * Copyright (C) 2009 Sebastian Dröge <sebastian.droege@collabora.co.uk>.
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

#include "fpi-byte-reader.h"
#include <string.h>

G_BEGIN_DECLS

#define FPI_BYTE_WRITER(writer) ((FpiByteWriter *) (writer))

/**
 * FpiByteWriter:
 * @parent: #FpiByteReader parent
 * @alloc_size: Allocation size of the data
 * @fixed: If %TRUE no reallocations are allowed
 * @owned: If %FALSE no reallocations are allowed and copies of data are returned
 *
 * A byte writer instance.
 */
typedef struct {
  FpiByteReader parent;

  guint alloc_size;

  gboolean fixed;
  gboolean owned;

  /* < private > */
} FpiByteWriter;


FpiByteWriter * fpi_byte_writer_new             (void) G_GNUC_MALLOC;


FpiByteWriter * fpi_byte_writer_new_with_size   (guint size, gboolean fixed) G_GNUC_MALLOC;


FpiByteWriter * fpi_byte_writer_new_with_data   (guint8 *data, guint size, gboolean initialized) G_GNUC_MALLOC;


void            fpi_byte_writer_init            (FpiByteWriter *writer);


void            fpi_byte_writer_init_with_size  (FpiByteWriter *writer, guint size, gboolean fixed);


void            fpi_byte_writer_init_with_data  (FpiByteWriter *writer, guint8 *data,
                                                 guint size, gboolean initialized);

void            fpi_byte_writer_free                    (FpiByteWriter *writer);


guint8 *        fpi_byte_writer_free_and_get_data       (FpiByteWriter *writer);


void            fpi_byte_writer_reset                   (FpiByteWriter *writer);


guint8 *        fpi_byte_writer_reset_and_get_data      (FpiByteWriter *writer);

/**
 * fpi_byte_writer_get_pos:
 * @writer: #FpiByteWriter instance
 *
 * Returns: The current position of the read/write cursor
 */
/**
 * fpi_byte_writer_set_pos:
 * @writer: #FpiByteWriter instance
 * @pos: new position
 *
 * Sets the current read/write cursor of @writer. The new position
 * can only be between 0 and the current size.
 *
 * Returns: %TRUE if the new position could be set
 */
/**
 * fpi_byte_writer_get_size:
 * @writer: #FpiByteWriter instance
 *
 * Returns: The current, initialized size of the data
 */
static inline guint
fpi_byte_writer_get_pos (const FpiByteWriter *writer)
{
  return fpi_byte_reader_get_pos ((const FpiByteReader *) writer);
}

static inline gboolean
fpi_byte_writer_set_pos (FpiByteWriter *writer, guint pos)
{
  return fpi_byte_reader_set_pos (FPI_BYTE_READER (writer), pos);
}

static inline guint
fpi_byte_writer_get_size (const FpiByteWriter *writer)
{
  return fpi_byte_reader_get_size ((const FpiByteReader *) writer);
}


guint           fpi_byte_writer_get_remaining     (const FpiByteWriter *writer);


gboolean        fpi_byte_writer_ensure_free_space (FpiByteWriter *writer, guint size);


gboolean        fpi_byte_writer_put_uint8         (FpiByteWriter *writer, guint8 val);


gboolean        fpi_byte_writer_put_int8          (FpiByteWriter *writer, gint8 val);


gboolean        fpi_byte_writer_put_uint16_be     (FpiByteWriter *writer, guint16 val);


gboolean        fpi_byte_writer_put_uint16_le     (FpiByteWriter *writer, guint16 val);


gboolean        fpi_byte_writer_put_int16_be      (FpiByteWriter *writer, gint16 val);


gboolean        fpi_byte_writer_put_int16_le      (FpiByteWriter *writer, gint16 val);


gboolean        fpi_byte_writer_put_uint24_be     (FpiByteWriter *writer, guint32 val);


gboolean        fpi_byte_writer_put_uint24_le     (FpiByteWriter *writer, guint32 val);


gboolean        fpi_byte_writer_put_int24_be      (FpiByteWriter *writer, gint32 val);


gboolean        fpi_byte_writer_put_int24_le      (FpiByteWriter *writer, gint32 val);


gboolean        fpi_byte_writer_put_uint32_be     (FpiByteWriter *writer, guint32 val);


gboolean        fpi_byte_writer_put_uint32_le     (FpiByteWriter *writer, guint32 val);


gboolean        fpi_byte_writer_put_int32_be      (FpiByteWriter *writer, gint32 val);


gboolean        fpi_byte_writer_put_int32_le      (FpiByteWriter *writer, gint32 val);


gboolean        fpi_byte_writer_put_uint64_be     (FpiByteWriter *writer, guint64 val);


gboolean        fpi_byte_writer_put_uint64_le     (FpiByteWriter *writer, guint64 val);


gboolean        fpi_byte_writer_put_int64_be      (FpiByteWriter *writer, gint64 val);


gboolean        fpi_byte_writer_put_int64_le      (FpiByteWriter *writer, gint64 val);


gboolean        fpi_byte_writer_put_float32_be    (FpiByteWriter *writer, gfloat val);


gboolean        fpi_byte_writer_put_float32_le    (FpiByteWriter *writer, gfloat val);


gboolean        fpi_byte_writer_put_float64_be    (FpiByteWriter *writer, gdouble val);


gboolean        fpi_byte_writer_put_float64_le    (FpiByteWriter *writer, gdouble val);


gboolean        fpi_byte_writer_put_data          (FpiByteWriter *writer, const guint8 *data, guint size);


gboolean        fpi_byte_writer_fill              (FpiByteWriter *writer, guint8 value, guint size);


gboolean        fpi_byte_writer_put_string_utf8   (FpiByteWriter *writer, const gchar *data);


gboolean        fpi_byte_writer_put_string_utf16  (FpiByteWriter *writer, const guint16 *data);


gboolean        fpi_byte_writer_put_string_utf32  (FpiByteWriter *writer, const guint32 *data);

/**
 * fpi_byte_writer_put_string:
 * @writer: #FpiByteWriter instance
 * @data: (in) (array zero-terminated=1): Null terminated string
 *
 * Write a NUL-terminated string to @writer (including the terminator). The
 * string is assumed to be in an 8-bit encoding (e.g. ASCII,UTF-8 or
 * ISO-8859-1).
 *
 * Returns: %TRUE if the string could be written
 */
#define fpi_byte_writer_put_string(writer, data) \
  fpi_byte_writer_put_string_utf8(writer, data)

static inline guint
fpi_byte_writer_next_pow2 (guint n)
{
  guint ret = 16;

  /* We start with 16, smaller allocations make no sense */

  while (ret < n && ret > 0)
    ret <<= 1;

  return ret ? ret : n;
}

static inline gboolean
fpi_byte_writer_ensure_free_space_inline (FpiByteWriter * writer, guint size)
{
  gpointer data;

  if (G_LIKELY (size <= writer->alloc_size - writer->parent.byte))
    return TRUE;
  if (G_UNLIKELY (writer->fixed || !writer->owned))
    return FALSE;
  if (G_UNLIKELY (writer->parent.byte > G_MAXUINT - size))
    return FALSE;

  writer->alloc_size = fpi_byte_writer_next_pow2 (writer->parent.byte + size);
  data = g_try_realloc ((guint8 *) writer->parent.data, writer->alloc_size);
  if (G_UNLIKELY (data == NULL))
    return FALSE;

  writer->parent.data = (guint8 *) data;

  return TRUE;
}

#define __FPI_BYTE_WRITER_CREATE_WRITE_FUNC(bits,type,name,write_func) \
static inline void \
fpi_byte_writer_put_##name##_unchecked (FpiByteWriter *writer, type val) \
{ \
  guint8 *write_data; \
  \
  write_data = (guint8 *) writer->parent.data + writer->parent.byte; \
  write_func (write_data, val); \
  writer->parent.byte += bits/8; \
  writer->parent.size = MAX (writer->parent.size, writer->parent.byte); \
} \
\
static inline gboolean \
fpi_byte_writer_put_##name##_inline (FpiByteWriter *writer, type val) \
{ \
  g_return_val_if_fail (writer != NULL, FALSE); \
  \
  if (G_UNLIKELY (!fpi_byte_writer_ensure_free_space_inline(writer, bits/8))) \
    return FALSE; \
  \
  fpi_byte_writer_put_##name##_unchecked (writer, val); \
  \
  return TRUE; \
}

__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (8, guint8, uint8, FP_WRITE_UINT8)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (8, gint8, int8, FP_WRITE_UINT8)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (16, guint16, uint16_le, FP_WRITE_UINT16_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (16, guint16, uint16_be, FP_WRITE_UINT16_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (16, gint16, int16_le, FP_WRITE_UINT16_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (16, gint16, int16_be, FP_WRITE_UINT16_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (24, guint32, uint24_le, FP_WRITE_UINT24_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (24, guint32, uint24_be, FP_WRITE_UINT24_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (24, gint32, int24_le, FP_WRITE_UINT24_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (24, gint32, int24_be, FP_WRITE_UINT24_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, guint32, uint32_le, FP_WRITE_UINT32_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, guint32, uint32_be, FP_WRITE_UINT32_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, gint32, int32_le, FP_WRITE_UINT32_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, gint32, int32_be, FP_WRITE_UINT32_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, guint64, uint64_le, FP_WRITE_UINT64_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, guint64, uint64_be, FP_WRITE_UINT64_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, gint64, int64_le, FP_WRITE_UINT64_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, gint64, int64_be, FP_WRITE_UINT64_BE)

__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, gfloat, float32_be, FP_WRITE_FLOAT_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (32, gfloat, float32_le, FP_WRITE_FLOAT_LE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, gdouble, float64_be, FP_WRITE_DOUBLE_BE)
__FPI_BYTE_WRITER_CREATE_WRITE_FUNC (64, gdouble, float64_le, FP_WRITE_DOUBLE_LE)

#undef __FPI_BYTE_WRITER_CREATE_WRITE_FUNC

static inline void
fpi_byte_writer_put_data_unchecked (FpiByteWriter * writer, const guint8 * data,
    guint size)
{
  memcpy ((guint8 *) & writer->parent.data[writer->parent.byte], data, size);
  writer->parent.byte += size;
  writer->parent.size = MAX (writer->parent.size, writer->parent.byte);
}

static inline gboolean
fpi_byte_writer_put_data_inline (FpiByteWriter * writer, const guint8 * data,
    guint size)
{
  g_return_val_if_fail (writer != NULL, FALSE);

  if (G_UNLIKELY (!fpi_byte_writer_ensure_free_space_inline (writer, size)))
    return FALSE;

  fpi_byte_writer_put_data_unchecked (writer, data, size);

  return TRUE;
}

static inline void
fpi_byte_writer_fill_unchecked (FpiByteWriter * writer, guint8 value, guint size)
{
  memset ((guint8 *) & writer->parent.data[writer->parent.byte], value, size);
  writer->parent.byte += size;
  writer->parent.size = MAX (writer->parent.size, writer->parent.byte);
}

static inline gboolean
fpi_byte_writer_fill_inline (FpiByteWriter * writer, guint8 value, guint size)
{
  g_return_val_if_fail (writer != NULL, FALSE);

  if (G_UNLIKELY (!fpi_byte_writer_ensure_free_space_inline (writer, size)))
    return FALSE;

  fpi_byte_writer_fill_unchecked (writer, value, size);

  return TRUE;
}

#ifndef FPI_BYTE_WRITER_DISABLE_INLINES

/* we use defines here so we can add the G_LIKELY() */

#define fpi_byte_writer_ensure_free_space(writer, size) \
    G_LIKELY (fpi_byte_writer_ensure_free_space_inline (writer, size))
#define fpi_byte_writer_put_uint8(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint8_inline (writer, val))
#define fpi_byte_writer_put_int8(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int8_inline (writer, val))
#define fpi_byte_writer_put_uint16_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint16_be_inline (writer, val))
#define fpi_byte_writer_put_uint16_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint16_le_inline (writer, val))
#define fpi_byte_writer_put_int16_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int16_be_inline (writer, val))
#define fpi_byte_writer_put_int16_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int16_le_inline (writer, val))
#define fpi_byte_writer_put_uint24_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint24_be_inline (writer, val))
#define fpi_byte_writer_put_uint24_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint24_le_inline (writer, val))
#define fpi_byte_writer_put_int24_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int24_be_inline (writer, val))
#define fpi_byte_writer_put_int24_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int24_le_inline (writer, val))
#define fpi_byte_writer_put_uint32_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint32_be_inline (writer, val))
#define fpi_byte_writer_put_uint32_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint32_le_inline (writer, val))
#define fpi_byte_writer_put_int32_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int32_be_inline (writer, val))
#define fpi_byte_writer_put_int32_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int32_le_inline (writer, val))
#define fpi_byte_writer_put_uint64_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint64_be_inline (writer, val))
#define fpi_byte_writer_put_uint64_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_uint64_le_inline (writer, val))
#define fpi_byte_writer_put_int64_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int64_be_inline (writer, val))
#define fpi_byte_writer_put_int64_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_int64_le_inline (writer, val))

#define fpi_byte_writer_put_float32_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_float32_be_inline (writer, val))
#define fpi_byte_writer_put_float32_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_float32_le_inline (writer, val))
#define fpi_byte_writer_put_float64_be(writer, val) \
    G_LIKELY (fpi_byte_writer_put_float64_be_inline (writer, val))
#define fpi_byte_writer_put_float64_le(writer, val) \
    G_LIKELY (fpi_byte_writer_put_float64_le_inline (writer, val))

#define fpi_byte_writer_put_data(writer, data, size) \
    G_LIKELY (fpi_byte_writer_put_data_inline (writer, data, size))
#define fpi_byte_writer_fill(writer, val, size) \
    G_LIKELY (fpi_byte_writer_fill_inline (writer, val, size))

#endif

G_END_DECLS
