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

#include "profiles.h"
#include "../../send.h"
#include "../../util.h"

bool wsc_set_current_profile(wsc_connection_t *conn, const char *name)
{
    if (!conn || !name || strlen(name) < 1)
        return false;
    json_error_t err;
    bool result = false;
    json_t *data = json_pack_ex(&err, 0, "{ss}", "profile-name", name);

    if (data) {
        result = send_request_no_cb(conn, "SetCurrentProfile", data);
    } else {
        werr("Error packing json for SetCurrentProfile: %s at %i", err.text, err.line);
    }

    json_decref(data);
    return result;
}

request_result_t get_profile_callback(json_t *response, void *data)
{
    char **name = data;
    request_result_t result = REQUEST_ERROR;
    json_error_t err;
    char *tmp = NULL;

    if (json_unpack_ex(response, &err, 0, "{ss}", "profile-name", &tmp)) {
        werr("Unpacking response json for GetCurrentProfile: %s at %i", err.text, err.line);
    } else {
        *name = bstrdup(tmp);
        result = REQUEST_OK;
    }

    return result;
}

bool wsc_get_current_profile(wsc_connection_t *conn, char **name)
{
    if (!conn || !name || *name)
        return false;
    return send_request_no_data(conn, "GetCurrentProfile", get_profile_callback, name);
}

void wsc_free_profiles(wsc_profiles_t *p)
{
    if (p) {
        for (size_t i = 0; i < p->num_profiles; i++)
            bfree(p->profile_names[i]);
        bfree(p->profile_names);
    }
}

request_result_t list_profiles_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    json_t *arr = NULL, *val = NULL;
    struct darray names;
    wsc_profiles_t *ps = data;
    size_t index = 0;
    json_error_t err;

    if (json_unpack_ex(response, &err, 0, "{so}", "profiles", &arr)) {
        werr("Unpacking response json for ListProfiles: %s at %i", err.text, err.line);
    } else {
        darray_init(&names);
        json_array_foreach (arr, index, val) {
            char *name = NULL;
            if (json_unpack_ex(val, &err, 0, "{ss}", "profile-name", &name)) {
                werr("Error unpacking json for profile in ListProfiles: %s at %i", err.text, err.line);
            } else {
                char *dup = bstrdup(name);
                darray_push_back(sizeof(char *), &names, &dup);
            }
        }
        result = REQUEST_OK;
        ps->num_profiles = names.num;
        ps->profile_names = names.array;
        json_decref(arr);
    }
    return result;
}

bool wsc_list_profiles(wsc_connection_t *conn, wsc_profiles_t *profiles)
{
    if (!conn || !profiles)
        return false;

    return send_request_no_data(conn, "ListProfiles", list_profiles_callback, profiles);
}
