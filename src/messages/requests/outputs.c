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

#include "outputs.h"
#include "external/darray.h"
#include "../../data.h"
#include "../../send.h"
#include "../../util.h"

void wsc_free_output(wsc_output_t *o)
{
    if (o)
        wsc_data_release(o->settings);
}

void wsc_free_outputs(wsc_ouputs_t *o)
{
    if (o) {
        for (size_t i = 0; i < o->count; i++)
            wsc_data_release(o->arr[i].settings);

        o->count = 0;
        bfree(o->arr);
    }
}

bool unpack_output(json_t *j, wsc_output_t *n)
{
    json_error_t err;
    json_t *flags = NULL, *settings = NULL;
    char *name = NULL, *type = NULL;
    bool result = false;
    if (json_unpack_ex(j, &err, 0, "{ss,ss,si,si,so,so,sb,sb,sf,si,si,si}", "name", &name, "type", &type, "width",
                       &n->width, "height", &n->height, "flags", &flags, "settings", &settings, "active", &n->active,
                       "reconnecting", &n->reconnecting, "congestion", &n->congestion, "totalFrames", &n->total_frames,
                       "droppedFrames", &n->dropped_frames, "totalBytes", &n->total_bytes)) {
        werr("Error unpacking output json: %s at %i", err.text, err.line);
        return false;
    }

    if (flags && json_unpack_ex(flags, &err, 0, "{si,sb,sb,sb,sb,sb}", "rawValue", &n->flags.raw_value, "audio",
                                &n->flags.audio, "video", &n->flags.video, "encoded", &n->flags.encoded, "multiTrack",
                                &n->flags.multi_track, "service", &n->flags.service)) {
        werr("Error unpacking output flags json: %s at %i", err.text, err.line);
    } else {
        util_strncpy(n->name, STR_LEN, name, STR_LEN);
        util_strncpy(n->type, STR_LEN, type, STR_LEN);

        /* Process aditional json objects */
        char *settings_json = json_dumps(settings, 0);
        if (strcmp(settings_json, "{}"))
            n->settings = wsc_data_create_from_json(settings_json);
        else
            n->settings = NULL;
        bfree(settings_json);
        result = true;
    }

    json_decref(settings);
    json_decref(flags);
    return result;
}

request_result_t outputs_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_OK;
    wsc_ouputs_t *o = data;
    json_error_t err;
    struct darray outputs;
    size_t index;
    json_t *arr = NULL, *val = NULL;

    if (json_unpack_ex(response, &err, 0, "{so}", "outputs", &arr)) {
        werr("Error while unpacking response for ListOutputs: %s at %i", err.text, err.line);
        return REQUEST_ERROR;
    }

    darray_init(&outputs);
    json_array_foreach (arr, index, val) {
        wsc_init_struct(wsc_output_t, n);
        if (unpack_output(val, &n))
            darray_push_back(sizeof(wsc_output_t), &outputs, &n);
    }

    json_decref(arr);
    o->arr = outputs.array;
    o->count = outputs.num;
    return result;
}

bool wsc_list_outputs(wsc_connection_t *conn, wsc_ouputs_t *o)
{
    if (!o)
        return false;
    return send_request_no_data(conn, "ListOutputs", outputs_callback, o);
}

request_result_t output_callback(json_t *response, void *data)
{
    json_t *output_info = NULL;
    json_error_t err;
    request_result_t result = REQUEST_ERROR;
    if (json_unpack_ex(response, &err, 0, "{ss}", "outputInfo", &output_info)) {
        werr("Error getting output info from message response");
    } else {
        result = unpack_output(output_info, data) ? REQUEST_OK : REQUEST_ERROR;
        json_decref(output_info);
    }
    return result;
}

bool wsc_get_output_info(wsc_connection_t *conn, const char *name, wsc_output_t *o)
{
    if (!o || !name || strlen(name) < 1)
        return false;
    bool result = false;
    json_error_t err;
    json_t *data = json_pack_ex(&err, 0, "{ss}", "outputName", name);

    if (data) {
        result = send_request(conn, "GetOutputInfo", data, output_callback, o);
        json_decref(data);
    } else {
        werr("Error while packing json for GetOutputInfo: %s at %i", err.text, err.line);
    }

    return result;
}

bool wsc_start_output(wsc_connection_t *conn, const char *name)
{
    if (!conn || !name || strlen(name) < 1)
        return false;
    bool result = false;
    json_error_t err;
    json_t *data = json_pack_ex(&err, 0, "{ss}", "outputName", name);

    if (data) {
        result = send_request_no_cb(conn, "StartOutput", data);
        json_decref(data);
    } else {
        werr("Error while packing json for StartOutput: %s at %i", err.text, err.line);
    }

    return result;
}

bool wsc_stop_output(wsc_connection_t *conn, const char *name, bool force)
{
    if (!conn || !name || strlen(name) < 1)
        return false;
    bool result = false;
    json_error_t err;
    json_t *data = json_pack_ex(&err, 0, "{ss,sb}", "outputName", name, "force", force);

    if (data) {
        result = send_request_no_cb(conn, "StopOutput", data);
        json_decref(data);
    } else {
        werr("Error while packing json for StopOutput: %s at %i", err.text, err.line);
    }

    return result;
}
