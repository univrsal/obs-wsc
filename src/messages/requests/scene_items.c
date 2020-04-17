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

#include "scene_items.h"
#include "send.h"
#include "util.h"

bool unpack_scene_item_properties(json_t *j, wsc_scene_item_properties_t *i)
{
    json_error_t err;
    bool result = false;
    char *name = NULL, *bounds_type = NULL, *parent = NULL;
    json_t *children = NULL;

    if (json_unpack_ex(j, &err, 0,
                       "{ss, si, si, si, sf, sf, sf,"
                       "si, si, si, si, sb, sb, sb, ss, si, sf, sf,"
                       "si, si, sf, sf, si}",
                       "name", &name, "position.x", &i->position.x, "position.y", &i->position.y, "positon.alignment",
                       &i->position.alignment, "rotation", &i->rotation, "scale.x", &i->scale.x, "scale.y", &i->scale.y,
                       "crop.top", &i->crop.top, "crop.right", &i->crop.right, "crop.bottom", &i->crop.bottom,
                       "crop.left", &i->crop.left, "visible", &i->visible, "muted", &i->muted, "locked", &i->locked,
                       "bounds.type", &bounds_type, "bounds.alignment", &i->bounds.alignment, "bounds.x", &i->bounds.cx,
                       "bounds.y", &i->bounds.cy, "sourceWidth", &i->source_width, "sourceHeight", &i->source_height,
                       "width", &i->width, "height", &i->height, "alignment", &i->alignment)) {
        werr("Unpacking json for GetSceneITemProperties: %s at %i", err.text, err.line);
    } else {
        util_strncpy(i->name, STR_LEN, name, STR_LEN);

        if (json_unpack(j, "{ss}", "parentGroupName", &parent) == 0)
            util_strncpy(i->parent_group_name, STR_LEN, parent, STR_LEN);
        if (json_unpack(j, "{so}", "groupChildren", &children) == 0) {
            json_t *val = NULL;
            size_t index = 0;
            struct darray da;
            darray_init(&da);

            json_array_foreach (children, index, val) {
            }
        }
        result = true;
    }
    return result;
}

request_result_t scene_item_properties_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    wsc_scene_item_properties_t *item = data;

    return result;
}

bool wsc_set_scene_item_properties(wsc_connection_t *conn, const wsc_scene_item_properties_t *i, const char *scene)
{
    if (!i || !i->name[0])
        return false;

    bool result = false;
    json_t *data = NULL, *item_data;
    json_error_t err;
    char *bounds_type = NULL;

    if (i->id >= 0)
        item_data = json_pack("{ss,si}", "item.name", i->name, "item.id", i->id);
    else
        item_data = json_string(i->name);

    data = json_pack_ex(&err, 0,
                        "{so, si, si, si, sf, sf, sf,"
                        "si, si, si, si, sb, sb, sb, ss, si, sf, sf,"
                        "si, si, sf, sf, si}",
                        "item", item_data, "position.x", i->position.x, "position.y", i->position.y,
                        "positon.alignment", i->position.alignment, "rotation", i->rotation, "scale.x", i->scale.x,
                        "scale.y", i->scale.y, "crop.top", i->crop.top, "crop.right", i->crop.right, "crop.bottom",
                        i->crop.bottom, "crop.left", i->crop.left, "visible", i->visible, "muted", i->muted, "locked",
                        i->locked, "bounds.type", bounds_type, "bounds.alignment", i->bounds.alignment, "bounds.x",
                        i->bounds.cx, "bounds.y", i->bounds.cy, "sourceWidth", i->source_width, "sourceHeight",
                        i->source_height, "width", i->width, "height", i->height, "alignment", i->alignment);
    if (data) {
        result = send_request_no_cb(conn, "SetSceneItemProperties", data);
        json_decref(data);
    }
    json_decref(item_data);

    return result;
}
