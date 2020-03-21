/*************************************************************************
 * This file is part of obs-wsc
 * github.con/univrsal/obs-wsc
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "util.h"
#include "external/base64.h"
#include "external/bmem.h"
#include "external/buffer.h"
#include <stdlib.h>
#include <string.h>

char *util_random_id(const struct darray *ids)
{
    static char id[MSG_ID_LENGTH];
    int idx = 0;
    bool unique = false;

    while (!unique) {
        while (idx < MSG_ID_LENGTH) {
            int rnd = rand() % (26 + 26 + 10);
            if (rnd < 26)
                id[idx] = (char)('a' + rnd);
            else if (rnd < 26 + 26)
                id[idx] = (char)('A' + rnd - 26);
            else
                id[idx] = (char)('0' + rnd - 26 - 26);
            idx++;
        }

        id[MSG_ID_LENGTH - 1] = '\0';

        unique = true;
        for (size_t i = 0; i < ids->num; i++) {
            if (strcmp(darray_item(sizeof(char *), ids, i), id) == 0) {
                unique = false;
                break;
            }
        }
    }

    return id;
}

char *util_qt_geometry_string(const obs_wsc_geometry_t *geo)
{
    struct libs_buffer *buf = libs_buffer(sizeof(obs_wsc_geometry_t));
    /* I hope the QDataStream works like this */
    libs_buffer_write_u32(buf, geo->magic_number);
    libs_buffer_write_u16(buf, geo->version_major);
    libs_buffer_write_u16(buf, geo->version_minor);

    libs_buffer_write_i32(buf, geo->frame_geometry.x);
    libs_buffer_write_i32(buf, geo->frame_geometry.y);
    libs_buffer_write_i32(buf, geo->frame_geometry.w);
    libs_buffer_write_i32(buf, geo->frame_geometry.h);

    libs_buffer_write_i32(buf, geo->normal_geometry.x);
    libs_buffer_write_i32(buf, geo->normal_geometry.y);
    libs_buffer_write_i32(buf, geo->normal_geometry.w);
    libs_buffer_write_i32(buf, geo->normal_geometry.h);

    libs_buffer_write_i32(buf, geo->screen_number);

    libs_buffer_write_u8(buf, geo->maximized);
    libs_buffer_write_u8(buf, geo->fullscreen);

    libs_buffer_write_i32(buf, geo->screen_width);

    libs_buffer_write_i32(buf, geo->geometry.x);
    libs_buffer_write_i32(buf, geo->geometry.y);
    libs_buffer_write_i32(buf, geo->geometry.w);
    libs_buffer_write_i32(buf, geo->geometry.h);

    size_t len = base64_encode(buf->data, NULL, buf->size, 0);
    char *ret = bzalloc(sizeof(char) * (len + 1));
    base64_encode(buf->data, (BYTE *)ret, buf->size, 1);
    return ret;
}

const char *util_projector_type(enum obs_wsc_projector_type t)
{
    switch (t) {
    case WSC_PROJECTOR_SCENE:
        return "Scene";
    case WSC_PROJECTOR_SOURCE:
        return "Source";
    case WSC_PROJECTOR_MULTIVIEW:
        return "Multiview";
    case WSC_PROJECTOR_STUDIO_PROGRAM:
        return "StudioProgram";
    case WSC_PROJECTOR_PREVIEW:
    default:
        return "Preview";
    }
}
