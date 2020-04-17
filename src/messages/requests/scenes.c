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

#include "scenes.h"
#include "send.h"

void wsc_free_scene_collections(wsc_scene_collections_t *s)
{
    if (s) {
        for (size_t i = 0; i < s->num_names; i++)
            bfree(s->names[i]);
        bfree(s->names);
        s->names = NULL;
        s->num_names = 0;
    }
}

bool wsc_set_current_scene_collection(wsc_connection_t *conn, const char *name)
{
    if (!name || strlen(name) < 1)
        return false;
    json_t *data = NULL;
    json_error_t err;
    bool result = false;

    if (json_pack_ex(&err, 0, "{ss}", "sc-name", name)) {
        werr("Packing json for SetCurrentSceneCollection: %s at %i", err.text, err.line);
    } else {
        result = send_request_no_cb(conn, "SetCurrentSceneCollection", data);
        json_decref(data);
    }
    return result;
}

request_result_t current_scene_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    char **name = data;
    char *tmp = NULL;
    json_error_t err;

    if (json_unpack_ex(response, &err, 0, "{ss}", "sc-name", &tmp)) {
        werr("Unpacking json for GetCurrentSceneCollection: %s at %i", err.text, err.line);
    } else {
        *name = bstrdup(tmp);
        result = REQUEST_OK;
    }

    return result;
}

bool wsc_get_current_scene_collection(wsc_connection_t *conn, char **name)
{
    if (!name || *name)
        return false;
    return send_request_no_data(conn, "GetCurrentSceneCollection", current_scene_callback, name);
}

request_result_t list_scene_collections_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    wsc_scene_collections_t *c = data;
    json_t *arr = NULL, *val = NULL;
    size_t index = 0;
    json_error_t err;

    if (json_unpack_ex(response, &err, 0, "{so}", "scene-collections", &arr)) {
        werr("Unpacking scene collections array for ListSceneCollections: %s at %i", err.text, err.line);
    } else {
        struct darray scenes;
        darray_init(&scenes);

        json_array_foreach (arr, index, val) {
            char *dup = bstrdup(json_string_value(val));
            if (dup)
                darray_push_back(sizeof(char *), &scenes, &dup);
        }

        c->names = scenes.array;
        c->num_names = scenes.num;
        result = REQUEST_OK;
        json_decref(arr);
    }
    return result;
}

bool wsc_list_scene_collections(wsc_connection_t *conn, wsc_scene_collections_t *c)
{
    if (!c || c->names || c->num_names)
        return false;
    return send_request_no_data(conn, "ListSceneCollections", list_scene_collections_callback, c);
}
