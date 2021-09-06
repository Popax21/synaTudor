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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define FPI_BYTE_WRITER_DISABLE_INLINES
#include "fpi-byte-writer.h"

/**
 * SECTION:fpi-byte-writer
 * @title: FpiByteWriter
 * @short_description: Writes different integer, string and floating point
 *     types to a memory buffer and allows reading
 *
 * #FpiByteWriter provides a byte writer and reader that can write/read different
 * integer and floating point types to/from a memory buffer. It provides functions
 * for writing/reading signed/unsigned, little/big endian integers of 8, 16, 24,
 * 32 and 64 bits and functions for reading little/big endian floating points numbers of
 * 32 and 64 bits. It also provides functions to write/read NUL-terminated strings
 * in various character encodings.
 */

/**
 * fpi_byte_writer_new: (skip)
 *
 * Creates a new, empty #FpiByteWriter instance
 *
 * Free-function: fpi_byte_writer_free
 *
 * Returns: (transfer full): a new, empty #FpiByteWriter instance
 */
FpiByteWriter *
fpi_byte_writer_new (void)
{
  FpiByteWriter *ret = g_slice_new0 (FpiByteWriter);

  ret->owned = TRUE;
  return ret;
}

/**
 * fpi_byte_writer_new_with_size: (skip)
 * @size: Initial size of data
 * @fixed: If %TRUE the data can't be reallocated
 *
 * Creates a new #FpiByteWriter instance with the given
 * initial data size.
 *
 * Free-function: fpi_byte_writer_free
 *
 * Returns: (transfer full): a new #FpiByteWriter instance
 */
FpiByteWriter *
fpi_byte_writer_new_with_size (guint size, gboolean fixed)
{
  FpiByteWriter *ret = fpi_byte_writer_new ();

  ret->alloc_size = size;
  ret->parent.data = g_malloc (ret->alloc_size);
  ret->fixed = fixed;
  ret->owned = TRUE;

  return ret;
}

/**
 * fpi_byte_writer_new_with_data: (skip)
 * @data: Memory area for writing
 * @size: Size of @data in bytes
 * @initialized: If %TRUE the complete data can be read from the beginning
 *
 * Creates a new #FpiByteWriter instance with the given
 * memory area. If @initialized is %TRUE it is possible to
 * read @size bytes from the #FpiByteWriter from the beginning.
 *
 * Free-function: fpi_byte_writer_free
 *
 * Returns: (transfer full): a new #FpiByteWriter instance
 */
FpiByteWriter *
fpi_byte_writer_new_with_data (guint8 * data, guint size, gboolean initialized)
{
  FpiByteWriter *ret = fpi_byte_writer_new ();

  ret->parent.data = data;
  ret->parent.size = (initialized) ? size : 0;
  ret->alloc_size = size;
  ret->fixed = TRUE;
  ret->owned = FALSE;

  return ret;
}

/**
 * fpi_byte_writer_init:
 * @writer: #FpiByteWriter instance
 *
 * Initializes @writer to an empty instance
 */
void
fpi_byte_writer_init (FpiByteWriter * writer)
{
  g_return_if_fail (writer != NULL);

  memset (writer, 0, sizeof (FpiByteWriter));

  writer->owned = TRUE;
}

/**
 * fpi_byte_writer_init_with_size:
 * @writer: #FpiByteWriter instance
 * @size: Initial size of data
 * @fixed: If %TRUE the data can't be reallocated
 *
 * Initializes @writer with the given initial data size.
 */
void
fpi_byte_writer_init_with_size (FpiByteWriter * writer, guint size,
    gboolean fixed)
{
  g_return_if_fail (writer != NULL);

  fpi_byte_writer_init (writer);

  writer->parent.data = g_malloc (size);
  writer->alloc_size = size;
  writer->fixed = fixed;
  writer->owned = TRUE;
}

/**
 * fpi_byte_writer_init_with_data:
 * @writer: #FpiByteWriter instance
 * @data: (array length=size) (transfer none): Memory area for writing
 * @size: Size of @data in bytes
 * @initialized: If %TRUE the complete data can be read from the beginning
 *
 * Initializes @writer with the given
 * memory area. If @initialized is %TRUE it is possible to
 * read @size bytes from the #FpiByteWriter from the beginning.
 */
void
fpi_byte_writer_init_with_data (FpiByteWriter * writer, guint8 * data,
    guint size, gboolean initialized)
{
  g_return_if_fail (writer != NULL);

  fpi_byte_writer_init (writer);

  writer->parent.data = data;
  writer->parent.size = (initialized) ? size : 0;
  writer->alloc_size = size;
  writer->fixed = TRUE;
  writer->owned = FALSE;
}

/**
 * fpi_byte_writer_reset:
 * @writer: #FpiByteWriter instance
 *
 * Resets @writer and frees the data if it's
 * owned by @writer.
 */
void
fpi_byte_writer_reset (FpiByteWriter * writer)
{
  g_return_if_fail (writer != NULL);

  if (writer->owned)
    g_free ((guint8 *) writer->parent.data);
  memset (writer, 0, sizeof (FpiByteWriter));
}

/**
 * fpi_byte_writer_reset_and_get_data:
 * @writer: #FpiByteWriter instance
 *
 * Resets @writer and returns the current data.
 *
 * Free-function: g_free
 *
 * Returns: (array) (transfer full): the current data. g_free() after
 * usage.
 */
guint8 *
fpi_byte_writer_reset_and_get_data (FpiByteWriter * writer)
{
  guint8 *data;

  g_return_val_if_fail (writer != NULL, NULL);

  data = (guint8 *) writer->parent.data;
  if (!writer->owned)
    data = g_memdup (data, writer->parent.size);
  writer->parent.data = NULL;
  fpi_byte_writer_reset (writer);

  return data;
}

/**
 * fpi_byte_writer_free:
 * @writer: (in) (transfer full): #FpiByteWriter instance
 *
 * Frees @writer and all memory allocated by it.
 */
void
fpi_byte_writer_free (FpiByteWriter * writer)
{
  g_return_if_fail (writer != NULL);

  fpi_byte_writer_reset (writer);
  g_slice_free (FpiByteWriter, writer);
}

/**
 * fpi_byte_writer_free_and_get_data:
 * @writer: (in) (transfer full): #FpiByteWriter instance
 *
 * Frees @writer and all memory allocated by it except
 * the current data, which is returned.
 *
 * Free-function: g_free
 *
 * Returns: (transfer full): the current data. g_free() after usage.
 */
guint8 *
fpi_byte_writer_free_and_get_data (FpiByteWriter * writer)
{
  guint8 *data;

  g_return_val_if_fail (writer != NULL, NULL);

  data = fpi_byte_writer_reset_and_get_data (writer);
  g_slice_free (FpiByteWriter, writer);

  return data;
}

/**
 * fpi_byte_writer_get_remaining:
 * @writer: #FpiByteWriter instance
 *
 * Returns the remaining size of data that can still be written. If
 * -1 is returned the remaining size is only limited by system resources.
 *
 * Returns: the remaining size of data that can still be written
 */
guint
fpi_byte_writer_get_remaining (const FpiByteWriter * writer)
{
  g_return_val_if_fail (writer != NULL, -1);

  if (!writer->fixed)
    return -1;
  else
    return writer->alloc_size - writer->parent.byte;
}

/**
 * fpi_byte_writer_ensure_free_space:
 * @writer: #FpiByteWriter instance
 * @size: Number of bytes that should be available
 *
 * Checks if enough free space from the current write cursor is
 * available and reallocates if necessary.
 *
 * Returns: %TRUE if at least @size bytes are still available
 */
gboolean
fpi_byte_writer_ensure_free_space (FpiByteWriter * writer, guint size)
{
  return fpi_byte_writer_ensure_free_space_inline (writer, size);
}


#define CREATE_WRITE_FUNC(bits,type,name,write_func) \
gboolean \
fpi_byte_writer_put_##name (FpiByteWriter *writer, type val) \
{ \
  return fpi_byte_writer_put_##name##_inline (writer, val); \
}

CREATE_WRITE_FUNC (8, guint8, uint8, GST_WRITE_UINT8);
CREATE_WRITE_FUNC (8, gint8, int8, GST_WRITE_UINT8);
CREATE_WRITE_FUNC (16, guint16, uint16_le, GST_WRITE_UINT16_LE);
CREATE_WRITE_FUNC (16, guint16, uint16_be, GST_WRITE_UINT16_BE);
CREATE_WRITE_FUNC (16, gint16, int16_le, GST_WRITE_UINT16_LE);
CREATE_WRITE_FUNC (16, gint16, int16_be, GST_WRITE_UINT16_BE);
CREATE_WRITE_FUNC (24, guint32, uint24_le, GST_WRITE_UINT24_LE);
CREATE_WRITE_FUNC (24, guint32, uint24_be, GST_WRITE_UINT24_BE);
CREATE_WRITE_FUNC (24, gint32, int24_le, GST_WRITE_UINT24_LE);
CREATE_WRITE_FUNC (24, gint32, int24_be, GST_WRITE_UINT24_BE);
CREATE_WRITE_FUNC (32, guint32, uint32_le, GST_WRITE_UINT32_LE);
CREATE_WRITE_FUNC (32, guint32, uint32_be, GST_WRITE_UINT32_BE);
CREATE_WRITE_FUNC (32, gint32, int32_le, GST_WRITE_UINT32_LE);
CREATE_WRITE_FUNC (32, gint32, int32_be, GST_WRITE_UINT32_BE);
CREATE_WRITE_FUNC (64, guint64, uint64_le, GST_WRITE_UINT64_LE);
CREATE_WRITE_FUNC (64, guint64, uint64_be, GST_WRITE_UINT64_BE);
CREATE_WRITE_FUNC (64, gint64, int64_le, GST_WRITE_UINT64_LE);
CREATE_WRITE_FUNC (64, gint64, int64_be, GST_WRITE_UINT64_BE);

CREATE_WRITE_FUNC (32, gfloat, float32_be, GST_WRITE_FLOAT_BE);
CREATE_WRITE_FUNC (32, gfloat, float32_le, GST_WRITE_FLOAT_LE);
CREATE_WRITE_FUNC (64, gdouble, float64_be, GST_WRITE_DOUBLE_BE);
CREATE_WRITE_FUNC (64, gdouble, float64_le, GST_WRITE_DOUBLE_LE);

gboolean
fpi_byte_writer_put_data (FpiByteWriter * writer, const guint8 * data,
    guint size)
{
  return fpi_byte_writer_put_data_inline (writer, data, size);
}

gboolean
fpi_byte_writer_fill (FpiByteWriter * writer, guint8 value, guint size)
{
  return fpi_byte_writer_fill_inline (writer, value, size);
}

#define CREATE_WRITE_STRING_FUNC(bits,type) \
gboolean \
fpi_byte_writer_put_string_utf##bits (FpiByteWriter *writer, const type * data) \
{ \
  guint size = 0; \
  \
  g_return_val_if_fail (writer != NULL, FALSE); \
  \
  /* endianness does not matter if we are looking for a NUL terminator */ \
  while (data[size] != 0) { \
    /* have prevent overflow */ \
    if (G_UNLIKELY (size == G_MAXUINT)) \
      return FALSE; \
    ++size; \
  } \
  ++size; \
  \
  if (G_UNLIKELY (!fpi_byte_writer_ensure_free_space_inline(writer, size * (bits / 8)))) \
    return FALSE; \
  \
  fpi_byte_writer_put_data_inline (writer, (const guint8 *) data, size * (bits / 8)); \
  \
  return TRUE; \
}

CREATE_WRITE_STRING_FUNC (8, gchar);
CREATE_WRITE_STRING_FUNC (16, guint16);
CREATE_WRITE_STRING_FUNC (32, guint32);
/**
 * fpi_byte_writer_put_uint8:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned 8 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint16_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned big endian 16 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint24_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned big endian 24 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint32_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned big endian 32 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint64_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned big endian 64 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint16_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned little endian 16 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint24_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned little endian 24 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint32_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned little endian 32 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_uint64_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a unsigned little endian 64 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int8:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed 8 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int16_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed big endian 16 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int24_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed big endian 24 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int32_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed big endian 32 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int64_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed big endian 64 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int16_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed little endian 16 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int24_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed little endian 24 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int32_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed little endian 32 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_int64_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a signed little endian 64 bit integer to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_float32_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a big endian 32 bit float to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_float64_be:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a big endian 64 bit float to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_float32_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a little endian 32 bit float to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_float64_le:
 * @writer: #FpiByteWriter instance
 * @val: Value to write
 *
 * Writes a little endian 64 bit float to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_string_utf8:
 * @writer: #FpiByteWriter instance
 * @data: (transfer none): UTF8 string to write
 *
 * Writes a NUL-terminated UTF8 string to @writer (including the terminator).
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_string_utf16:
 * @writer: #FpiByteWriter instance
 * @data: (transfer none) (array zero-terminated=1): UTF16 string to write
 *
 * Writes a NUL-terminated UTF16 string to @writer (including the terminator).
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_string_utf32:
 * @writer: #FpiByteWriter instance
 * @data: (transfer none) (array zero-terminated=1): UTF32 string to write
 *
 * Writes a NUL-terminated UTF32 string to @writer (including the terminator).
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_put_data:
 * @writer: #FpiByteWriter instance
 * @data: (transfer none) (array length=size): Data to write
 * @size: Size of @data in bytes
 *
 * Writes @size bytes of @data to @writer.
 *
 * Returns: %TRUE if the value could be written
 */
/**
 * fpi_byte_writer_fill:
 * @writer: #FpiByteWriter instance
 * @value: Value to be written
 * @size: Number of bytes to be written
 *
 * Writes @size bytes containing @value to @writer.
 *
 * Returns: %TRUE if the value could be written
 */

