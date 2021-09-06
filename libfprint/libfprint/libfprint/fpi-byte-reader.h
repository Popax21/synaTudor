/* GStreamer byte reader
 *
 * Copyright (C) 2008 Sebastian Dröge <sebastian.droege@collabora.co.uk>.
 * Copyright (C) 2009 Tim-Philipp Müller <tim centricular net>
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
#include "fpi-byte-utils.h"

G_BEGIN_DECLS

#define FPI_BYTE_READER(reader) ((FpiByteReader *) (reader))

/**
 * FpiByteReader:
 * @data: (array length=size): Data from which the bit reader will
 *   read
 * @size: Size of @data in bytes
 * @byte: Current byte position
 *
 * A byte reader instance.
 */
typedef struct {
  const guint8 *data;
  guint size;

  guint byte;  /* Byte position */

  /* < private > */
} FpiByteReader;


FpiByteReader * fpi_byte_reader_new             (const guint8 *data, guint size) G_GNUC_MALLOC;


void            fpi_byte_reader_free            (FpiByteReader *reader);


void            fpi_byte_reader_init            (FpiByteReader *reader, const guint8 *data, guint size);


gboolean        fpi_byte_reader_peek_sub_reader (FpiByteReader * reader,
                                                 FpiByteReader * sub_reader,
                                                 guint           size);

gboolean        fpi_byte_reader_get_sub_reader  (FpiByteReader * reader,
                                                 FpiByteReader * sub_reader,
                                                 guint           size);

gboolean        fpi_byte_reader_set_pos         (FpiByteReader *reader, guint pos);


guint           fpi_byte_reader_get_pos         (const FpiByteReader *reader);


guint           fpi_byte_reader_get_remaining   (const FpiByteReader *reader);


guint           fpi_byte_reader_get_size        (const FpiByteReader *reader);


gboolean        fpi_byte_reader_skip            (FpiByteReader *reader, guint nbytes);


gboolean        fpi_byte_reader_get_uint8       (FpiByteReader *reader, guint8 *val);


gboolean        fpi_byte_reader_get_int8        (FpiByteReader *reader, gint8 *val);


gboolean        fpi_byte_reader_get_uint16_le   (FpiByteReader *reader, guint16 *val);


gboolean        fpi_byte_reader_get_int16_le    (FpiByteReader *reader, gint16 *val);


gboolean        fpi_byte_reader_get_uint16_be   (FpiByteReader *reader, guint16 *val);


gboolean        fpi_byte_reader_get_int16_be    (FpiByteReader *reader, gint16 *val);


gboolean        fpi_byte_reader_get_uint24_le   (FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_get_int24_le    (FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_get_uint24_be   (FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_get_int24_be    (FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_get_uint32_le   (FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_get_int32_le    (FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_get_uint32_be   (FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_get_int32_be    (FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_get_uint64_le   (FpiByteReader *reader, guint64 *val);


gboolean        fpi_byte_reader_get_int64_le    (FpiByteReader *reader, gint64 *val);


gboolean        fpi_byte_reader_get_uint64_be   (FpiByteReader *reader, guint64 *val);


gboolean        fpi_byte_reader_get_int64_be    (FpiByteReader *reader, gint64 *val);


gboolean        fpi_byte_reader_peek_uint8      (const FpiByteReader *reader, guint8 *val);


gboolean        fpi_byte_reader_peek_int8       (const FpiByteReader *reader, gint8 *val);


gboolean        fpi_byte_reader_peek_uint16_le  (const FpiByteReader *reader, guint16 *val);


gboolean        fpi_byte_reader_peek_int16_le   (const FpiByteReader *reader, gint16 *val);


gboolean        fpi_byte_reader_peek_uint16_be  (const FpiByteReader *reader, guint16 *val);


gboolean        fpi_byte_reader_peek_int16_be   (const FpiByteReader *reader, gint16 *val);


gboolean        fpi_byte_reader_peek_uint24_le  (const FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_peek_int24_le   (const FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_peek_uint24_be  (const FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_peek_int24_be   (const FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_peek_uint32_le  (const FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_peek_int32_le   (const FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_peek_uint32_be  (const FpiByteReader *reader, guint32 *val);


gboolean        fpi_byte_reader_peek_int32_be   (const FpiByteReader *reader, gint32 *val);


gboolean        fpi_byte_reader_peek_uint64_le  (const FpiByteReader *reader, guint64 *val);


gboolean        fpi_byte_reader_peek_int64_le   (const FpiByteReader *reader, gint64 *val);


gboolean        fpi_byte_reader_peek_uint64_be  (const FpiByteReader *reader, guint64 *val);


gboolean        fpi_byte_reader_peek_int64_be   (const FpiByteReader *reader, gint64 *val);


gboolean        fpi_byte_reader_get_float32_le  (FpiByteReader *reader, gfloat *val);


gboolean        fpi_byte_reader_get_float32_be  (FpiByteReader *reader, gfloat *val);


gboolean        fpi_byte_reader_get_float64_le  (FpiByteReader *reader, gdouble *val);


gboolean        fpi_byte_reader_get_float64_be  (FpiByteReader *reader, gdouble *val);


gboolean        fpi_byte_reader_peek_float32_le (const FpiByteReader *reader, gfloat *val);


gboolean        fpi_byte_reader_peek_float32_be (const FpiByteReader *reader, gfloat *val);


gboolean        fpi_byte_reader_peek_float64_le (const FpiByteReader *reader, gdouble *val);


gboolean        fpi_byte_reader_peek_float64_be (const FpiByteReader *reader, gdouble *val);


gboolean        fpi_byte_reader_dup_data        (FpiByteReader * reader, guint size, guint8       ** val);


gboolean        fpi_byte_reader_get_data        (FpiByteReader * reader, guint size, const guint8 ** val);


gboolean        fpi_byte_reader_peek_data       (const FpiByteReader * reader, guint size, const guint8 ** val);

#define fpi_byte_reader_dup_string(reader,str) \
    fpi_byte_reader_dup_string_utf8(reader,str)


gboolean        fpi_byte_reader_dup_string_utf8  (FpiByteReader * reader, gchar   ** str);


gboolean        fpi_byte_reader_dup_string_utf16 (FpiByteReader * reader, guint16 ** str);


gboolean        fpi_byte_reader_dup_string_utf32 (FpiByteReader * reader, guint32 ** str);

#define fpi_byte_reader_skip_string(reader) \
    fpi_byte_reader_skip_string_utf8(reader)


gboolean        fpi_byte_reader_skip_string_utf8  (FpiByteReader * reader);


gboolean        fpi_byte_reader_skip_string_utf16 (FpiByteReader * reader);


gboolean        fpi_byte_reader_skip_string_utf32 (FpiByteReader * reader);

#define fpi_byte_reader_get_string(reader,str) \
    fpi_byte_reader_get_string_utf8(reader,str)

#define fpi_byte_reader_peek_string(reader,str) \
    fpi_byte_reader_peek_string_utf8(reader,str)


gboolean        fpi_byte_reader_get_string_utf8    (FpiByteReader * reader, const gchar ** str);


gboolean        fpi_byte_reader_peek_string_utf8   (const FpiByteReader * reader, const gchar ** str);


guint           fpi_byte_reader_masked_scan_uint32 (const FpiByteReader * reader,
                                                    guint32               mask,
                                                    guint32               pattern,
                                                    guint                 offset,
                                                    guint                 size);

guint           fpi_byte_reader_masked_scan_uint32_peek (const FpiByteReader * reader,
                                                         guint32 mask,
                                                         guint32 pattern,
                                                         guint offset,
                                                         guint size,
                                                         guint32 * value);

/**
 * FPI_BYTE_READER_INIT:
 * @data: Data from which the #FpiByteReader should read
 * @size: Size of @data in bytes
 *
 * A #FpiByteReader must be initialized with this macro, before it can be
 * used. This macro can used be to initialize a variable, but it cannot
 * be assigned to a variable. In that case you have to use
 * fpi_byte_reader_init().
 */
#define FPI_BYTE_READER_INIT(data, size) {data, size, 0}

/* unchecked variants */
static inline void
fpi_byte_reader_skip_unchecked (FpiByteReader * reader, guint nbytes)
{
  reader->byte += nbytes;
}

#define __FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(bits,type,lower,upper,adj) \
\
static inline type \
fpi_byte_reader_peek_##lower##_unchecked (const FpiByteReader * reader) \
{ \
  type val = (type) FP_READ_##upper (reader->data + reader->byte); \
  adj \
  return val; \
} \
\
static inline type \
fpi_byte_reader_get_##lower##_unchecked (FpiByteReader * reader) \
{ \
  type val = fpi_byte_reader_peek_##lower##_unchecked (reader); \
  reader->byte += bits / 8; \
  return val; \
}

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(8,guint8,uint8,UINT8,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(8,gint8,int8,UINT8,/* */)

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(16,guint16,uint16_le,UINT16_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(16,guint16,uint16_be,UINT16_BE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(16,gint16,int16_le,UINT16_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(16,gint16,int16_be,UINT16_BE,/* */)

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,guint32,uint32_le,UINT32_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,guint32,uint32_be,UINT32_BE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,gint32,int32_le,UINT32_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,gint32,int32_be,UINT32_BE,/* */)

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(24,guint32,uint24_le,UINT24_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(24,guint32,uint24_be,UINT24_BE,/* */)

/* fix up the sign for 24-bit signed ints stored in 32-bit signed ints */
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(24,gint32,int24_le,UINT24_LE,
    if (val & 0x00800000) val |= 0xff000000;)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(24,gint32,int24_be,UINT24_BE,
    if (val & 0x00800000) val |= 0xff000000;)

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,guint64,uint64_le,UINT64_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,guint64,uint64_be,UINT64_BE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,gint64,int64_le,UINT64_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,gint64,int64_be,UINT64_BE,/* */)

__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,gfloat,float32_le,FLOAT_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(32,gfloat,float32_be,FLOAT_BE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,gdouble,float64_le,DOUBLE_LE,/* */)
__FPI_BYTE_READER_GET_PEEK_BITS_UNCHECKED(64,gdouble,float64_be,DOUBLE_BE,/* */)

#undef __GET_PEEK_BITS_UNCHECKED

static inline const guint8 *
fpi_byte_reader_peek_data_unchecked (const FpiByteReader * reader)
{
  return (const guint8 *) (reader->data + reader->byte);
}

static inline const guint8 *
fpi_byte_reader_get_data_unchecked (FpiByteReader * reader, guint size)
{
  const guint8 *data;

  data = fpi_byte_reader_peek_data_unchecked (reader);
  fpi_byte_reader_skip_unchecked (reader, size);
  return data;
}

static inline guint8 *
fpi_byte_reader_dup_data_unchecked (FpiByteReader * reader, guint size)
{
  gconstpointer data = fpi_byte_reader_get_data_unchecked (reader, size);
  return (guint8 *) g_memdup (data, size);
}

/* Unchecked variants that should not be used */
static inline guint
fpi_byte_reader_get_pos_unchecked (const FpiByteReader * reader)
{
  return reader->byte;
}

static inline guint
fpi_byte_reader_get_remaining_unchecked (const FpiByteReader * reader)
{
  return reader->size - reader->byte;
}

static inline guint
fpi_byte_reader_get_size_unchecked (const FpiByteReader * reader)
{
  return reader->size;
}

/* inlined variants (do not use directly) */

static inline guint
fpi_byte_reader_get_remaining_inline (const FpiByteReader * reader)
{
  g_return_val_if_fail (reader != NULL, 0);

  return fpi_byte_reader_get_remaining_unchecked (reader);
}

static inline guint
fpi_byte_reader_get_size_inline (const FpiByteReader * reader)
{
  g_return_val_if_fail (reader != NULL, 0);

  return fpi_byte_reader_get_size_unchecked (reader);
}

#define __FPI_BYTE_READER_GET_PEEK_BITS_INLINE(bits,type,name) \
\
static inline gboolean \
fpi_byte_reader_peek_##name##_inline (const FpiByteReader * reader, type * val) \
{ \
  g_return_val_if_fail (reader != NULL, FALSE); \
  g_return_val_if_fail (val != NULL, FALSE); \
  \
  if (fpi_byte_reader_get_remaining_unchecked (reader) < (bits / 8)) \
    return FALSE; \
\
  *val = fpi_byte_reader_peek_##name##_unchecked (reader); \
  return TRUE; \
} \
\
static inline gboolean \
fpi_byte_reader_get_##name##_inline (FpiByteReader * reader, type * val) \
{ \
  g_return_val_if_fail (reader != NULL, FALSE); \
  g_return_val_if_fail (val != NULL, FALSE); \
  \
  if (fpi_byte_reader_get_remaining_unchecked (reader) < (bits / 8)) \
    return FALSE; \
\
  *val = fpi_byte_reader_get_##name##_unchecked (reader); \
  return TRUE; \
}

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(8,guint8,uint8)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(8,gint8,int8)

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(16,guint16,uint16_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(16,guint16,uint16_be)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(16,gint16,int16_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(16,gint16,int16_be)

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,guint32,uint32_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,guint32,uint32_be)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,gint32,int32_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,gint32,int32_be)

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(24,guint32,uint24_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(24,guint32,uint24_be)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(24,gint32,int24_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(24,gint32,int24_be)

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,guint64,uint64_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,guint64,uint64_be)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,gint64,int64_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,gint64,int64_be)

__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,gfloat,float32_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(32,gfloat,float32_be)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,gdouble,float64_le)
__FPI_BYTE_READER_GET_PEEK_BITS_INLINE(64,gdouble,float64_be)

#undef __FPI_BYTE_READER_GET_PEEK_BITS_INLINE

#ifndef FPI_BYTE_READER_DISABLE_INLINES

#define fpi_byte_reader_init(reader,data,size) \
    fpi_byte_reader_init_inline(reader,data,size)

#define fpi_byte_reader_get_remaining(reader) \
    fpi_byte_reader_get_remaining_inline(reader)

#define fpi_byte_reader_get_size(reader) \
    fpi_byte_reader_get_size_inline(reader)

#define fpi_byte_reader_get_pos(reader) \
    fpi_byte_reader_get_pos_inline(reader)

/* we use defines here so we can add the G_LIKELY() */
#define fpi_byte_reader_get_uint8(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint8_inline(reader,val))
#define fpi_byte_reader_get_int8(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int8_inline(reader,val))
#define fpi_byte_reader_get_uint16_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint16_le_inline(reader,val))
#define fpi_byte_reader_get_int16_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int16_le_inline(reader,val))
#define fpi_byte_reader_get_uint16_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint16_be_inline(reader,val))
#define fpi_byte_reader_get_int16_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int16_be_inline(reader,val))
#define fpi_byte_reader_get_uint24_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint24_le_inline(reader,val))
#define fpi_byte_reader_get_int24_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int24_le_inline(reader,val))
#define fpi_byte_reader_get_uint24_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint24_be_inline(reader,val))
#define fpi_byte_reader_get_int24_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int24_be_inline(reader,val))
#define fpi_byte_reader_get_uint32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint32_le_inline(reader,val))
#define fpi_byte_reader_get_int32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int32_le_inline(reader,val))
#define fpi_byte_reader_get_uint32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint32_be_inline(reader,val))
#define fpi_byte_reader_get_int32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int32_be_inline(reader,val))
#define fpi_byte_reader_get_uint64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint64_le_inline(reader,val))
#define fpi_byte_reader_get_int64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int64_le_inline(reader,val))
#define fpi_byte_reader_get_uint64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_uint64_be_inline(reader,val))
#define fpi_byte_reader_get_int64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_int64_be_inline(reader,val))

#define fpi_byte_reader_peek_uint8(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint8_inline(reader,val))
#define fpi_byte_reader_peek_int8(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int8_inline(reader,val))
#define fpi_byte_reader_peek_uint16_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint16_le_inline(reader,val))
#define fpi_byte_reader_peek_int16_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int16_le_inline(reader,val))
#define fpi_byte_reader_peek_uint16_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint16_be_inline(reader,val))
#define fpi_byte_reader_peek_int16_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int16_be_inline(reader,val))
#define fpi_byte_reader_peek_uint24_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint24_le_inline(reader,val))
#define fpi_byte_reader_peek_int24_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int24_le_inline(reader,val))
#define fpi_byte_reader_peek_uint24_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint24_be_inline(reader,val))
#define fpi_byte_reader_peek_int24_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int24_be_inline(reader,val))
#define fpi_byte_reader_peek_uint32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint32_le_inline(reader,val))
#define fpi_byte_reader_peek_int32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int32_le_inline(reader,val))
#define fpi_byte_reader_peek_uint32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint32_be_inline(reader,val))
#define fpi_byte_reader_peek_int32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int32_be_inline(reader,val))
#define fpi_byte_reader_peek_uint64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint64_le_inline(reader,val))
#define fpi_byte_reader_peek_int64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int64_le_inline(reader,val))
#define fpi_byte_reader_peek_uint64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_uint64_be_inline(reader,val))
#define fpi_byte_reader_peek_int64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_int64_be_inline(reader,val))

#define fpi_byte_reader_get_float32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_float32_le_inline(reader,val))
#define fpi_byte_reader_get_float32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_float32_be_inline(reader,val))
#define fpi_byte_reader_get_float64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_get_float64_le_inline(reader,val))
#define fpi_byte_reader_get_float64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_get_float64_be_inline(reader,val))
#define fpi_byte_reader_peek_float32_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_float32_le_inline(reader,val))
#define fpi_byte_reader_peek_float32_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_float32_be_inline(reader,val))
#define fpi_byte_reader_peek_float64_le(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_float64_le_inline(reader,val))
#define fpi_byte_reader_peek_float64_be(reader,val) \
    G_LIKELY(fpi_byte_reader_peek_float64_be_inline(reader,val))

#endif /* FPI_BYTE_READER_DISABLE_INLINES */

static inline void
fpi_byte_reader_init_inline (FpiByteReader * reader, const guint8 * data, guint size)
{
  g_return_if_fail (reader != NULL);

  reader->data = data;
  reader->size = size;
  reader->byte = 0;
}

static inline gboolean
fpi_byte_reader_peek_sub_reader_inline (FpiByteReader * reader,
    FpiByteReader * sub_reader, guint size)
{
  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (sub_reader != NULL, FALSE);

  if (fpi_byte_reader_get_remaining_unchecked (reader) < size)
    return FALSE;

  sub_reader->data = reader->data + reader->byte;
  sub_reader->byte = 0;
  sub_reader->size = size;
  return TRUE;
}

static inline gboolean
fpi_byte_reader_get_sub_reader_inline (FpiByteReader * reader,
    FpiByteReader * sub_reader, guint size)
{
  if (!fpi_byte_reader_peek_sub_reader_inline (reader, sub_reader, size))
    return FALSE;
  fpi_byte_reader_skip_unchecked (reader, size);
  return TRUE;
}

static inline gboolean
fpi_byte_reader_dup_data_inline (FpiByteReader * reader, guint size, guint8 ** val)
{
  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (val != NULL, FALSE);

  if (G_UNLIKELY (size > reader->size || fpi_byte_reader_get_remaining_unchecked (reader) < size))
    return FALSE;

  *val = fpi_byte_reader_dup_data_unchecked (reader, size);
  return TRUE;
}

static inline gboolean
fpi_byte_reader_get_data_inline (FpiByteReader * reader, guint size, const guint8 ** val)
{
  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (val != NULL, FALSE);

  if (G_UNLIKELY (size > reader->size || fpi_byte_reader_get_remaining_unchecked (reader) < size))
    return FALSE;

  *val = fpi_byte_reader_get_data_unchecked (reader, size);
  return TRUE;
}

static inline gboolean
fpi_byte_reader_peek_data_inline (const FpiByteReader * reader, guint size, const guint8 ** val)
{
  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (val != NULL, FALSE);

  if (G_UNLIKELY (size > reader->size || fpi_byte_reader_get_remaining_unchecked (reader) < size))
    return FALSE;

  *val = fpi_byte_reader_peek_data_unchecked (reader);
  return TRUE;
}

static inline guint
fpi_byte_reader_get_pos_inline (const FpiByteReader * reader)
{
  g_return_val_if_fail (reader != NULL, 0);

  return fpi_byte_reader_get_pos_unchecked (reader);
}

static inline gboolean
fpi_byte_reader_skip_inline (FpiByteReader * reader, guint nbytes)
{
  g_return_val_if_fail (reader != NULL, FALSE);

  if (G_UNLIKELY (fpi_byte_reader_get_remaining_unchecked (reader) < nbytes))
    return FALSE;

  reader->byte += nbytes;
  return TRUE;
}

#ifndef FPI_BYTE_READER_DISABLE_INLINES

#define fpi_byte_reader_dup_data(reader,size,val) \
    G_LIKELY(fpi_byte_reader_dup_data_inline(reader,size,val))
#define fpi_byte_reader_get_data(reader,size,val) \
    G_LIKELY(fpi_byte_reader_get_data_inline(reader,size,val))
#define fpi_byte_reader_peek_data(reader,size,val) \
    G_LIKELY(fpi_byte_reader_peek_data_inline(reader,size,val))
#define fpi_byte_reader_skip(reader,nbytes) \
    G_LIKELY(fpi_byte_reader_skip_inline(reader,nbytes))

#endif /* FPI_BYTE_READER_DISABLE_INLINES */

G_END_DECLS
