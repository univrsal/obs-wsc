/* Copyright (c) 2020 github.com/univrsal <universailp@web.de>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef LIB_SIMPLE_BUFFER_H
#define LIB_SIMPLE_BUFFER_H
#include "c99defs.h"

/**
  * \struct libs_buffer
  * \brief Simple byte buffer
  * \defgroup libs_buffer 
  * that can be written to
  * either directly or with
  * helper methods
  */
struct libs_buffer {
    size_t size;
    size_t write_pos;
    size_t read_pos;
    char *data;
};

/**
  * \brief Create a buffer of provided size
  * data is zeroed
  * \ingroup libs_buffer 
  * \param size	size of buffer
  * \return		pointer to buffer structure
  */
struct libs_buffer *libs_buffer(size_t size);

/**
 * @brief Free a buffer
 * @param buf
 */
void libs_buffer_free(struct libs_buffer *buf);

/** \brief Returns the memory size of the buffer
  * \ingroup libs_buffer 
  * \param buf	the buffer
  * \return		the total size of the buffer in bytes
  */
extern size_t libs_buffer_size(const struct libs_buffer *buf);

/** \brief Resizes the buffer of the buffer
  * \ingroup libs_buffer 
  * \param buf	the buffer
  * \return		the total size of the buffer in bytes
  */
extern size_t libs_buffer_size(const struct libs_buffer *buf);

/**** Read  ****/

/** \brief Read a data chunk from the buffer
  * \ingroup libs_buffer 
  * \param buf	the buffer
  * \param data	the data
  * \param size	the size of data
  * \return 	true if the data was read
  */
extern bool libs_buffer_read(struct libs_buffer *buf, void *data, size_t size);

/** \brief Read a float from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param f    pointer to the output float
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_float(struct libs_buffer *buf, float *f)
{
    return libs_buffer_read(buf, f, sizeof(float));
}

/** \brief Read a double from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param d    pointer to the output double 
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_double(struct libs_buffer *buf, double *d)
{
    return libs_buffer_read(buf, d, sizeof(double));
}

/** \brief Read a byte from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param b    pointer to the output byte 
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_u8(struct libs_buffer *buf, uint8_t *d)
{
    return libs_buffer_read(buf, d, sizeof(uint8_t));
}

/** \brief Read a 16 bit integer from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param b    pointer to the output integer
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_u16(struct libs_buffer *buf, uint16_t *d)
{
    return libs_buffer_read(buf, d, sizeof(uint16_t));
}

/** \brief Read a 32 bit integer from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param b    pointer to the output integer
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_u32(struct libs_buffer *buf, uint32_t *d)
{
    return libs_buffer_read(buf, d, sizeof(uint32_t));
}

/** \brief Read a 64 bit integer from the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param b    pointer to the output integer
  * \return     true if the data was read 
  */
FORCE_INLINE
bool libs_buffer_read_u64(struct libs_buffer *buf, uint64_t *d)
{
    return libs_buffer_read(buf, d, sizeof(uint64_t));
}

/**** Macros ****/

#define libs_buffer_read_i8(b, i) libs_buffer_read_u8(b, (uint8_t *)i)
#define libs_buffer_read_i16(b, i) libs_buffer_read_u16(b, (uint16_t *)i)
#define libs_buffer_read_i32(b, i) libs_buffer_read_u32(b, (uint32_t *)i)
#define libs_buffer_read_i64(b, i) libs_buffer_read_u64(b, (uint64_t *)i)

/**** Write ****/

/** \brief Write a data chunk to the buffer
  * \ingroup libs_buffer 
  * \param buf	the buffer
  * \param data	the data
  * \param size	the size of data
  * \return 	true if the data fit into the buffer
  */
extern bool libs_buffer_write(struct libs_buffer *buf, const void *data, size_t size);

/** \brief Write a float to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param f    the float
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_float(struct libs_buffer *buf, float f)
{
    return libs_buffer_write(buf, &f, sizeof(float));
}

/** \brief Write a double to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param d    the double 
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_double(struct libs_buffer *buf, double d)
{
    return libs_buffer_write(buf, &d, sizeof(double));
}

/** \brief Write a byte to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param b    the byte 
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_u8(struct libs_buffer *buf, uint8_t b)
{
    return libs_buffer_write(buf, &b, sizeof(uint8_t));
}

/** \brief Write a 16 bit integer to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param i    the integer 
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_u16(struct libs_buffer *buf, uint16_t i)
{
    return libs_buffer_write(buf, &i, sizeof(uint16_t));
}

/** \brief Write a 32 bit integer to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param i    the integer 
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_u32(struct libs_buffer *buf, uint32_t i)
{
    return libs_buffer_write(buf, &i, sizeof(uint32_t));
}

/** \brief Write a 64 bit integer to the buffer
  * \ingroup libs_buffer
  * \param buf  the buffer
  * \param i    the integer 
  * \return     true if the data was written
  */
FORCE_INLINE
bool libs_buffer_write_u64(struct libs_buffer *buf, uint64_t i)
{
    return libs_buffer_write(buf, &i, sizeof(uint64_t));
}

/**** Macros ****/

#define libs_buffer_write_i8(b, i) libs_buffer_write_u8(b, (uint8_t)i)
#define libs_buffer_write_i16(b, i) libs_buffer_write_u16(b, (uint16_t)i)
#define libs_buffer_write_i32(b, i) libs_buffer_write_u16(b, (uint32_t)i)
#define libs_buffer_write_i64(b, i) libs_buffer_write_u64(b, (uint64_t)i)

#endif
