/* GStreamer byte reader
 *
 * Copyright (C) 2008 Sebastian Dröge <sebastian.droege@collabora.co.uk>.
 * Copyright (C) 2009,2014 Tim-Philipp Müller <tim centricular net>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define FPI_BYTE_READER_DISABLE_INLINES
#include "fpi-byte-reader.h"

#include <string.h>

/**
 * SECTION:fpi-byte-reader
 * @title: FpiByteReader
 * @short_description: Reads different integer, string and floating point
 *     types from a memory buffer
 *
 * #FpiByteReader provides a byte reader that can read different integer and
 * floating point types from a memory buffer. It provides functions for reading
 * signed/unsigned, little/big endian integers of 8, 16, 24, 32 and 64 bits
 * and functions for reading little/big endian floating points numbers of
 * 32 and 64 bits. It also provides functions to read NUL-terminated strings
 * in various character encodings.
 */

/**
 * fpi_byte_reader_new: (skip)
 * @data: (in) (transfer none) (array length=size): data from which the
 *     #FpiByteReader should read
 * @size: Size of @data in bytes
 *
 * Create a new #FpiByteReader instance, which will read from @data.
 *
 * Free-function: fpi_byte_reader_free
 *
 * Returns: (transfer full): a new #FpiByteReader instance
 */
FpiByteReader *
fpi_byte_reader_new (const guint8 * data, guint size)
{
  FpiByteReader *ret = g_slice_new0 (FpiByteReader);

  ret->data = data;
  ret->size = size;

  return ret;
}

/**
 * fpi_byte_reader_free:
 * @reader: (in) (transfer full): a #FpiByteReader instance
 *
 * Frees a #FpiByteReader instance, which was previously allocated by
 * fpi_byte_reader_new().
 */
void
fpi_byte_reader_free (FpiByteReader * reader)
{
  g_return_if_fail (reader != NULL);

  g_slice_free (FpiByteReader, reader);
}

/**
 * fpi_byte_reader_init:
 * @reader: a #FpiByteReader instance
 * @data: (in) (transfer none) (array length=size): data from which
 *     the #FpiByteReader should read
 * @size: Size of @data in bytes
 *
 * Initializes a #FpiByteReader instance to read from @data. This function
 * can be called on already initialized instances.
 */
void
fpi_byte_reader_init (FpiByteReader * reader, const guint8 * data, guint size)
{
  g_return_if_fail (reader != NULL);

  reader->data = data;
  reader->size = size;
  reader->byte = 0;
}

/**
 * fpi_byte_reader_peek_sub_reader: (skip)
 * @reader: an existing and initialized #FpiByteReader instance
 * @sub_reader: a #FpiByteReader instance to initialize as sub-reader
 * @size: size of @sub_reader in bytes
 *
 * Initializes a #FpiByteReader sub-reader instance to contain @size bytes of
 * data from the current position of @reader. This is useful to read chunked
 * formats and make sure that one doesn't read beyond the size of the sub-chunk.
 *
 * Unlike fpi_byte_reader_get_sub_reader(), this function does not modify the
 * current position of @reader.
 *
 * Returns: FALSE on error or if @reader does not contain @size more bytes from
 *     the current position, and otherwise TRUE
 *
 * Since: 1.6
 */
gboolean
fpi_byte_reader_peek_sub_reader (FpiByteReader * reader,
    FpiByteReader * sub_reader, guint size)
{
  return fpi_byte_reader_peek_sub_reader_inline (reader, sub_reader, size);
}

/**
 * fpi_byte_reader_get_sub_reader: (skip)
 * @reader: an existing and initialized #FpiByteReader instance
 * @sub_reader: a #FpiByteReader instance to initialize as sub-reader
 * @size: size of @sub_reader in bytes
 *
 * Initializes a #FpiByteReader sub-reader instance to contain @size bytes of
 * data from the current position of @reader. This is useful to read chunked
 * formats and make sure that one doesn't read beyond the size of the sub-chunk.
 *
 * Unlike fpi_byte_reader_peek_sub_reader(), this function also modifies the
 * position of @reader and moves it forward by @size bytes.
 *
 * Returns: FALSE on error or if @reader does not contain @size more bytes from
 *     the current position, and otherwise TRUE
 *
 * Since: 1.6
 */
gboolean
fpi_byte_reader_get_sub_reader (FpiByteReader * reader,
    FpiByteReader * sub_reader, guint size)
{
  return fpi_byte_reader_get_sub_reader_inline (reader, sub_reader, size);
}

/**
 * fpi_byte_reader_set_pos:
 * @reader: a #FpiByteReader instance
 * @pos: The new position in bytes
 *
 * Sets the new position of a #FpiByteReader instance to @pos in bytes.
 *
 * Returns: %TRUE if the position could be set successfully, %FALSE
 * otherwise.
 */
gboolean
fpi_byte_reader_set_pos (FpiByteReader * reader, guint pos)
{
  g_return_val_if_fail (reader != NULL, FALSE);

  if (pos > reader->size)
    return FALSE;

  reader->byte = pos;

  return TRUE;
}

/**
 * fpi_byte_reader_get_pos:
 * @reader: a #FpiByteReader instance
 *
 * Returns the current position of a #FpiByteReader instance in bytes.
 *
 * Returns: The current position of @reader in bytes.
 */
guint
fpi_byte_reader_get_pos (const FpiByteReader * reader)
{
  return fpi_byte_reader_get_pos_inline (reader);
}

/**
 * fpi_byte_reader_get_remaining:
 * @reader: a #FpiByteReader instance
 *
 * Returns the remaining number of bytes of a #FpiByteReader instance.
 *
 * Returns: The remaining number of bytes of @reader instance.
 */
guint
fpi_byte_reader_get_remaining (const FpiByteReader * reader)
{
  return fpi_byte_reader_get_remaining_inline (reader);
}

/**
 * fpi_byte_reader_get_size:
 * @reader: a #FpiByteReader instance
 *
 * Returns the total number of bytes of a #FpiByteReader instance.
 *
 * Returns: The total number of bytes of @reader instance.
 */
guint
fpi_byte_reader_get_size (const FpiByteReader * reader)
{
  return fpi_byte_reader_get_size_inline (reader);
}

#define fpi_byte_reader_get_remaining fpi_byte_reader_get_remaining_inline
#define fpi_byte_reader_get_size fpi_byte_reader_get_size_inline

/**
 * fpi_byte_reader_skip:
 * @reader: a #FpiByteReader instance
 * @nbytes: the number of bytes to skip
 *
 * Skips @nbytes bytes of the #FpiByteReader instance.
 *
 * Returns: %TRUE if @nbytes bytes could be skipped, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_skip (FpiByteReader * reader, guint nbytes)
{
  return fpi_byte_reader_skip_inline (reader, nbytes);
}

/**
 * fpi_byte_reader_get_uint8:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint8 to store the result
 *
 * Read an unsigned 8 bit integer into @val and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int8:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint8 to store the result
 *
 * Read a signed 8 bit integer into @val and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint8:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint8 to store the result
 *
 * Read an unsigned 8 bit integer into @val but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int8:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint8 to store the result
 *
 * Read a signed 8 bit integer into @val but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint16_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint16 to store the result
 *
 * Read an unsigned 16 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int16_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint16 to store the result
 *
 * Read a signed 16 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint16_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint16 to store the result
 *
 * Read an unsigned 16 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int16_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint16 to store the result
 *
 * Read a signed 16 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint16_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint16 to store the result
 *
 * Read an unsigned 16 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int16_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint16 to store the result
 *
 * Read a signed 16 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint16_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint16 to store the result
 *
 * Read an unsigned 16 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int16_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint16 to store the result
 *
 * Read a signed 16 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint24_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 24 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int24_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 24 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint24_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 24 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int24_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 24 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint24_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 24 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int24_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 24 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint24_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 24 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int24_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 24 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */


/**
 * fpi_byte_reader_get_uint32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 32 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 32 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 32 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 32 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 32 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 32 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint32 to store the result
 *
 * Read an unsigned 32 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint32 to store the result
 *
 * Read a signed 32 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint64 to store the result
 *
 * Read an unsigned 64 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint64 to store the result
 *
 * Read a signed 64 bit little endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint64 to store the result
 *
 * Read an unsigned 64 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint64 to store the result
 *
 * Read a signed 64 bit little endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_uint64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint64 to store the result
 *
 * Read an unsigned 64 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_int64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint64 to store the result
 *
 * Read a signed 64 bit big endian integer into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_uint64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #guint64 to store the result
 *
 * Read an unsigned 64 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_int64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gint64 to store the result
 *
 * Read a signed 64 bit big endian integer into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

#define FPI_BYTE_READER_PEEK_GET(bits,type,name) \
gboolean \
fpi_byte_reader_get_##name (FpiByteReader * reader, type * val) \
{ \
  return fpi_byte_reader_get_##name##_inline (reader, val); \
} \
\
gboolean \
fpi_byte_reader_peek_##name (const FpiByteReader * reader, type * val) \
{ \
  return fpi_byte_reader_peek_##name##_inline (reader, val); \
}

/* *INDENT-OFF* */

FPI_BYTE_READER_PEEK_GET(8,guint8,uint8)
FPI_BYTE_READER_PEEK_GET(8,gint8,int8)

FPI_BYTE_READER_PEEK_GET(16,guint16,uint16_le)
FPI_BYTE_READER_PEEK_GET(16,guint16,uint16_be)
FPI_BYTE_READER_PEEK_GET(16,gint16,int16_le)
FPI_BYTE_READER_PEEK_GET(16,gint16,int16_be)

FPI_BYTE_READER_PEEK_GET(24,guint32,uint24_le)
FPI_BYTE_READER_PEEK_GET(24,guint32,uint24_be)
FPI_BYTE_READER_PEEK_GET(24,gint32,int24_le)
FPI_BYTE_READER_PEEK_GET(24,gint32,int24_be)

FPI_BYTE_READER_PEEK_GET(32,guint32,uint32_le)
FPI_BYTE_READER_PEEK_GET(32,guint32,uint32_be)
FPI_BYTE_READER_PEEK_GET(32,gint32,int32_le)
FPI_BYTE_READER_PEEK_GET(32,gint32,int32_be)

FPI_BYTE_READER_PEEK_GET(64,guint64,uint64_le)
FPI_BYTE_READER_PEEK_GET(64,guint64,uint64_be)
FPI_BYTE_READER_PEEK_GET(64,gint64,int64_le)
FPI_BYTE_READER_PEEK_GET(64,gint64,int64_be)

/**
 * fpi_byte_reader_get_float32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gfloat to store the result
 *
 * Read a 32 bit little endian floating point value into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_float32_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gfloat to store the result
 *
 * Read a 32 bit little endian floating point value into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_float32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gfloat to store the result
 *
 * Read a 32 bit big endian floating point value into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_float32_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gfloat to store the result
 *
 * Read a 32 bit big endian floating point value into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_float64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gdouble to store the result
 *
 * Read a 64 bit little endian floating point value into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_float64_le:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gdouble to store the result
 *
 * Read a 64 bit little endian floating point value into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_get_float64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gdouble to store the result
 *
 * Read a 64 bit big endian floating point value into @val
 * and update the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

/**
 * fpi_byte_reader_peek_float64_be:
 * @reader: a #FpiByteReader instance
 * @val: (out): Pointer to a #gdouble to store the result
 *
 * Read a 64 bit big endian floating point value into @val
 * but keep the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */

FPI_BYTE_READER_PEEK_GET(32,gfloat,float32_le)
FPI_BYTE_READER_PEEK_GET(32,gfloat,float32_be)
FPI_BYTE_READER_PEEK_GET(64,gdouble,float64_le)
FPI_BYTE_READER_PEEK_GET(64,gdouble,float64_be)

/* *INDENT-ON* */

/**
 * fpi_byte_reader_get_data:
 * @reader: a #FpiByteReader instance
 * @size: Size in bytes
 * @val: (out) (transfer none) (array length=size): address of a
 *     #guint8 pointer variable in which to store the result
 *
 * Returns a constant pointer to the current data
 * position if at least @size bytes are left and
 * updates the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_get_data (FpiByteReader * reader, guint size,
    const guint8 ** val)
{
  return fpi_byte_reader_get_data_inline (reader, size, val);
}

/**
 * fpi_byte_reader_peek_data:
 * @reader: a #FpiByteReader instance
 * @size: Size in bytes
 * @val: (out) (transfer none) (array length=size): address of a
 *     #guint8 pointer variable in which to store the result
 *
 * Returns a constant pointer to the current data
 * position if at least @size bytes are left and
 * keeps the current position.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_peek_data (const FpiByteReader * reader, guint size,
    const guint8 ** val)
{
  return fpi_byte_reader_peek_data_inline (reader, size, val);
}

/**
 * fpi_byte_reader_dup_data:
 * @reader: a #FpiByteReader instance
 * @size: Size in bytes
 * @val: (out) (transfer full) (array length=size): address of a
 *     #guint8 pointer variable in which to store the result
 *
 * Free-function: g_free
 *
 * Returns a newly-allocated copy of the current data
 * position if at least @size bytes are left and
 * updates the current position. Free with g_free() when no longer needed.
 *
 * Returns: %TRUE if successful, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_dup_data (FpiByteReader * reader, guint size, guint8 ** val)
{
  return fpi_byte_reader_dup_data_inline (reader, size, val);
}

/* Special optimized scan for mask 0xffffff00 and pattern 0x00000100 */
static inline gint
_scan_for_start_code (const guint8 * data, guint size)
{
  guint8 *pdata = (guint8 *) data;
  guint8 *pend = (guint8 *) (data + size - 4);

  while (pdata <= pend) {
    if (pdata[2] > 1) {
      pdata += 3;
    } else if (pdata[1]) {
      pdata += 2;
    } else if (pdata[0] || pdata[2] != 1) {
      pdata++;
    } else {
      return (pdata - data);
    }
  }

  /* nothing found */
  return -1;
}

static inline guint
_masked_scan_uint32_peek (const FpiByteReader * reader,
    guint32 mask, guint32 pattern, guint offset, guint size, guint32 * value)
{
  const guint8 *data;
  guint32 state;
  guint i;

  g_return_val_if_fail (size > 0, -1);
  g_return_val_if_fail ((guint64) offset + size <= reader->size - reader->byte,
      -1);

  /* we can't find the pattern with less than 4 bytes */
  if (G_UNLIKELY (size < 4))
    return -1;

  data = reader->data + reader->byte + offset;

  /* Handle special case found in MPEG and H264 */
  if ((pattern == 0x00000100) && (mask == 0xffffff00)) {
    gint ret = _scan_for_start_code (data, size);

    if (ret == -1)
      return ret;

    if (value != NULL)
      *value = (1 << 8) | data[ret + 3];

    return ret + offset;
  }

  /* set the state to something that does not match */
  state = ~pattern;

  /* now find data */
  for (i = 0; i < size; i++) {
    /* throw away one byte and move in the next byte */
    state = ((state << 8) | data[i]);
    if (G_UNLIKELY ((state & mask) == pattern)) {
      /* we have a match but we need to have skipped at
       * least 4 bytes to fill the state. */
      if (G_LIKELY (i >= 3)) {
        if (value)
          *value = state;
        return offset + i - 3;
      }
    }
  }

  /* nothing found */
  return -1;
}


/**
 * fpi_byte_reader_masked_scan_uint32:
 * @reader: a #FpiByteReader
 * @mask: mask to apply to data before matching against @pattern
 * @pattern: pattern to match (after mask is applied)
 * @offset: offset from which to start scanning, relative to the current
 *     position
 * @size: number of bytes to scan from offset
 *
 * Scan for pattern @pattern with applied mask @mask in the byte reader data,
 * starting from offset @offset relative to the current position.
 *
 * The bytes in @pattern and @mask are interpreted left-to-right, regardless
 * of endianness.  All four bytes of the pattern must be present in the
 * byte reader data for it to match, even if the first or last bytes are masked
 * out.
 *
 * It is an error to call this function without making sure that there is
 * enough data (offset+size bytes) in the byte reader.
 *
 * Returns: offset of the first match, or -1 if no match was found.
 *
 * Example:
 * |[
 * // Assume the reader contains 0x00 0x01 0x02 ... 0xfe 0xff
 *
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffffffff, 0x00010203, 0, 256);
 * // -> returns 0
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffffffff, 0x00010203, 1, 255);
 * // -> returns -1
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffffffff, 0x01020304, 1, 255);
 * // -> returns 1
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffff, 0x0001, 0, 256);
 * // -> returns -1
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffff, 0x0203, 0, 256);
 * // -> returns 0
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffff0000, 0x02030000, 0, 256);
 * // -> returns 2
 * fpi_byte_reader_masked_scan_uint32 (reader, 0xffff0000, 0x02030000, 0, 4);
 * // -> returns -1
 * ]|
 */
guint
fpi_byte_reader_masked_scan_uint32 (const FpiByteReader * reader, guint32 mask,
    guint32 pattern, guint offset, guint size)
{
  return _masked_scan_uint32_peek (reader, mask, pattern, offset, size, NULL);
}

/**
 * fpi_byte_reader_masked_scan_uint32_peek:
 * @reader: a #FpiByteReader
 * @mask: mask to apply to data before matching against @pattern
 * @pattern: pattern to match (after mask is applied)
 * @offset: offset from which to start scanning, relative to the current
 *     position
 * @size: number of bytes to scan from offset
 * @value: (out): pointer to uint32 to return matching data
 *
 * Scan for pattern @pattern with applied mask @mask in the byte reader data,
 * starting from offset @offset relative to the current position.
 *
 * The bytes in @pattern and @mask are interpreted left-to-right, regardless
 * of endianness.  All four bytes of the pattern must be present in the
 * byte reader data for it to match, even if the first or last bytes are masked
 * out.
 *
 * It is an error to call this function without making sure that there is
 * enough data (offset+size bytes) in the byte reader.
 *
 * Returns: offset of the first match, or -1 if no match was found.
 *
 * Since: 1.6
 */
guint
fpi_byte_reader_masked_scan_uint32_peek (const FpiByteReader * reader,
    guint32 mask, guint32 pattern, guint offset, guint size, guint32 * value)
{
  return _masked_scan_uint32_peek (reader, mask, pattern, offset, size, value);
}

#define FPI_BYTE_READER_SCAN_STRING(bits) \
static guint \
fpi_byte_reader_scan_string_utf##bits (const FpiByteReader * reader) \
{ \
  guint len, off, max_len; \
  \
  max_len = (reader->size - reader->byte) / sizeof (guint##bits); \
  \
  /* need at least a single NUL terminator */ \
  if (max_len < 1) \
    return 0; \
  \
  len = 0; \
  off = reader->byte; \
  /* endianness does not matter if we are looking for a NUL terminator */ \
  while (FP_READ_UINT##bits##_LE (&reader->data[off]) != 0) { \
    ++len; \
    off += sizeof (guint##bits); \
    /* have we reached the end without finding a NUL terminator? */ \
    if (len == max_len) \
      return 0; \
  } \
  /* return size in bytes including the NUL terminator (hence the +1) */ \
  return (len + 1) * sizeof (guint##bits); \
}

#define FP_READ_UINT8_LE FP_READ_UINT8
FPI_BYTE_READER_SCAN_STRING (8);
#undef FP_READ_UINT8_LE
FPI_BYTE_READER_SCAN_STRING (16);
FPI_BYTE_READER_SCAN_STRING (32);

#define FPI_BYTE_READER_SKIP_STRING(bits) \
gboolean \
fpi_byte_reader_skip_string_utf##bits (FpiByteReader * reader) \
{ \
  guint size; /* size in bytes including the terminator */ \
  \
  g_return_val_if_fail (reader != NULL, FALSE); \
  \
  size = fpi_byte_reader_scan_string_utf##bits (reader); \
  reader->byte += size; \
  return (size > 0); \
}

/**
 * fpi_byte_reader_skip_string:
 * @reader: a #FpiByteReader instance
 *
 * Skips a NUL-terminated string in the #FpiByteReader instance, advancing
 * the current position to the byte after the string. This will work for
 * any NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
/**
 * fpi_byte_reader_skip_string_utf8:
 * @reader: a #FpiByteReader instance
 *
 * Skips a NUL-terminated string in the #FpiByteReader instance, advancing
 * the current position to the byte after the string. This will work for
 * any NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc. No input checking for valid UTF-8 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
FPI_BYTE_READER_SKIP_STRING (8);

/**
 * fpi_byte_reader_skip_string_utf16:
 * @reader: a #FpiByteReader instance
 *
 * Skips a NUL-terminated UTF-16 string in the #FpiByteReader instance,
 * advancing the current position to the byte after the string.
 *
 * No input checking for valid UTF-16 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
FPI_BYTE_READER_SKIP_STRING (16);

/**
 * fpi_byte_reader_skip_string_utf32:
 * @reader: a #FpiByteReader instance
 *
 * Skips a NUL-terminated UTF-32 string in the #FpiByteReader instance,
 * advancing the current position to the byte after the string.
 *
 * No input checking for valid UTF-32 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
FPI_BYTE_READER_SKIP_STRING (32);

/**
 * fpi_byte_reader_peek_string:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer none) (array zero-terminated=1): address of a
 *     #gchar pointer variable in which to store the result
 *
 * Returns a constant pointer to the current data position if there is
 * a NUL-terminated string in the data (this could be just a NUL terminator).
 * The current position will be maintained. This will work for any
 * NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
/**
 * fpi_byte_reader_peek_string_utf8:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer none) (array zero-terminated=1): address of a
 *     #gchar pointer variable in which to store the result
 *
 * Returns a constant pointer to the current data position if there is
 * a NUL-terminated string in the data (this could be just a NUL terminator).
 * The current position will be maintained. This will work for any
 * NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc.
 *
 * No input checking for valid UTF-8 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be skipped, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_peek_string_utf8 (const FpiByteReader * reader,
    const gchar ** str)
{
  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (str != NULL, FALSE);

  if (fpi_byte_reader_scan_string_utf8 (reader) > 0) {
    *str = (const gchar *) (reader->data + reader->byte);
  } else {
    *str = NULL;
  }
  return (*str != NULL);
}

/**
 * fpi_byte_reader_get_string_utf8:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer none) (array zero-terminated=1): address of a
 *     #gchar pointer variable in which to store the result
 *
 * Returns a constant pointer to the current data position if there is
 * a NUL-terminated string in the data (this could be just a NUL terminator),
 * advancing the current position to the byte after the string. This will work
 * for any NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc.
 *
 * No input checking for valid UTF-8 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be found, %FALSE otherwise.
 */
gboolean
fpi_byte_reader_get_string_utf8 (FpiByteReader * reader, const gchar ** str)
{
  guint size;                   /* size in bytes including the terminator */

  g_return_val_if_fail (reader != NULL, FALSE);
  g_return_val_if_fail (str != NULL, FALSE);

  size = fpi_byte_reader_scan_string_utf8 (reader);
  if (size == 0) {
    *str = NULL;
    return FALSE;
  }

  *str = (const gchar *) (reader->data + reader->byte);
  reader->byte += size;
  return TRUE;
}

#define FPI_BYTE_READER_DUP_STRING(bits,type) \
gboolean \
fpi_byte_reader_dup_string_utf##bits (FpiByteReader * reader, type ** str) \
{ \
  guint size; /* size in bytes including the terminator */ \
  \
  g_return_val_if_fail (reader != NULL, FALSE); \
  g_return_val_if_fail (str != NULL, FALSE); \
  \
  size = fpi_byte_reader_scan_string_utf##bits (reader); \
  if (size == 0) { \
    *str = NULL; \
    return FALSE; \
  } \
  *str = g_memdup (reader->data + reader->byte, size); \
  reader->byte += size; \
  return TRUE; \
}

/**
 * fpi_byte_reader_dup_string_utf8:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer full) (array zero-terminated=1): address of a
 *     #gchar pointer variable in which to store the result
 *
 * Free-function: g_free
 *
 * FIXME:Reads (copies) a NUL-terminated string in the #FpiByteReader instance,
 * advancing the current position to the byte after the string. This will work
 * for any NUL-terminated string with a character width of 8 bits, so ASCII,
 * UTF-8, ISO-8859-N etc. No input checking for valid UTF-8 is done.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Returns: %TRUE if a string could be read into @str, %FALSE otherwise. The
 *     string put into @str must be freed with g_free() when no longer needed.
 */
FPI_BYTE_READER_DUP_STRING (8, gchar);

/**
 * fpi_byte_reader_dup_string_utf16:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer full) (array zero-terminated=1): address of a
 *     #guint16 pointer variable in which to store the result
 *
 * Free-function: g_free
 *
 * Returns a newly-allocated copy of the current data position if there is
 * a NUL-terminated UTF-16 string in the data (this could be an empty string
 * as well), and advances the current position.
 *
 * No input checking for valid UTF-16 is done. This function is endianness
 * agnostic - you should not assume the UTF-16 characters are in host
 * endianness.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Note: there is no peek or get variant of this function to ensure correct
 * byte alignment of the UTF-16 string.
 *
 * Returns: %TRUE if a string could be read, %FALSE otherwise. The
 *     string put into @str must be freed with g_free() when no longer needed.
 */
FPI_BYTE_READER_DUP_STRING (16, guint16);

/**
 * fpi_byte_reader_dup_string_utf32:
 * @reader: a #FpiByteReader instance
 * @str: (out) (transfer full) (array zero-terminated=1): address of a
 *     #guint32 pointer variable in which to store the result
 *
 * Free-function: g_free
 *
 * Returns a newly-allocated copy of the current data position if there is
 * a NUL-terminated UTF-32 string in the data (this could be an empty string
 * as well), and advances the current position.
 *
 * No input checking for valid UTF-32 is done. This function is endianness
 * agnostic - you should not assume the UTF-32 characters are in host
 * endianness.
 *
 * This function will fail if no NUL-terminator was found in in the data.
 *
 * Note: there is no peek or get variant of this function to ensure correct
 * byte alignment of the UTF-32 string.
 *
 * Returns: %TRUE if a string could be read, %FALSE otherwise. The
 *     string put into @str must be freed with g_free() when no longer needed.
 */
FPI_BYTE_READER_DUP_STRING (32, guint32);
