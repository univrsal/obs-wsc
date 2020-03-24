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

void wsc_free_outputs(wsc_ouputs_t *o)
{
    if (o) {
        for (size_t i = 0; i < o->count; i++) {
            wsc_data_release(o->arr[i].settings);
        }

        o->count = 0;
        bfree(o->arr);
    }
}

request_result_t outputs_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_OK;
    wsc_ouputs_t *o = data;
    json_error_t err;
    struct darray outputs;
    size_t index;
    json_t *arr = NULL, *val = NULL, *flags = NULL, *settings = NULL;
    char *name = NULL, *type = NULL;

    if (json_unpack_ex(response, &err, 0, "{so}", "outputs", &arr)) {
        werr("Error while unpacking response for ListOutputs: %s at %i", err.text, err.line);
        return REQUEST_ERROR;
    }

    darray_init(&outputs);
    json_array_foreach (arr, index, val) {
        wsc_init_struct(wsc_output_t, n);
        if (json_unpack_ex(val, &err, 0, "{ss,ss,si,si,so,so,sb,sb,sf,si,si,si}", "name", &name, "type", &type, "width",
                           &n.width, "height", &n.height, "flags", &flags, "settings", &settings, "active", &n.active,
                           "reconnecting", &n.reconnecting, "congestion", &n.congestion, "totalFrames", &n.total_frames,
                           "droppedFrames", &n.dropped_frames, "totalBytes", &n.total_bytes)) {
            werr("Error unpacking output json: %s at %i", err.text, err.line);
            result = REQUEST_ERROR;
            continue;
        }

        if (flags && json_unpack_ex(flags, &err, 0, "{si,sb,sb,sb,sb,sb}", "rawValue", &n.flags.raw_value, "audio",
                                    &n.flags.audio, "video", &n.flags.video, "encoded", &n.flags.encoded, "multiTrack",
                                    &n.flags.multi_track, "service", &n.flags.service)) {
            werr("Error unpacking output flags json: %s at %i", err.text, err.line);
            result = REQUEST_ERROR;
        } else {
            util_strncpy(n.name, STR_LEN, name, STR_LEN);
            util_strncpy(n.type, STR_LEN, type, STR_LEN);

            /* Process aditional json objects */
            char *settings_json = json_dumps(settings, 0);
            if (strcmp(settings_json, "{}"))
                n.settings = wsc_data_create_from_json(settings_json);
            else
                n.settings = NULL;
            bfree(settings_json);
            darray_push_back(sizeof(wsc_output_t), &outputs, &n);
        }

        json_decref(flags);
        json_decref(settings);
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
