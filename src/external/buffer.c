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

#include "buffer.h"
#include "bmem.h"
#include <stdlib.h>

struct libs_buffer *libs_buffer(size_t size)
{
    struct libs_buffer *buf = bzalloc(sizeof(struct libs_buffer));
    buf->size = size;
    buf->data = bmalloc(size);
    return buf;
}

void libs_buffer_free(struct libs_buffer *buf)
{
    bfree(buf);
}

size_t libs_buffer_size(const struct libs_buffer *buf)
{
    if (buf)
        return buf->size;
    return 0;
}

bool libs_buffer_read(struct libs_buffer *buf, void *data, size_t size)
{
    if (!buf || !data)
        return false;
    if (buf->read_pos + size > buf->size)
        return false;
    memcpy(buf->data + buf->read_pos, data, size);
    buf->read_pos += size;
    return true;
}

bool libs_buffer_write(struct libs_buffer *buf, const void *data, size_t size)
{
    if (!buf || !data)
        return false;
    if (buf->write_pos + size > buf->size)
        return false;

    memcpy(buf->data + buf->write_pos, data, size);
    buf->write_pos += size;
    return true;
}
