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

#include "recording.h"
#include "send.h"

bool wsc_toggle_recording(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StartStopRecording");
}

bool wsc_start_recording(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StartRecording");
}

bool wsc_stop_recording(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StopRecording");
}

bool wsc_pause_recording(wsc_connection_t *conn)
{
    return send_request_simple(conn, "PauseRecording");
}

bool wsc_resume_recording(wsc_connection_t *conn)
{
    return send_request_simple(conn, "ResumeRecording");
}

bool wsc_set_recording_folder(wsc_connection_t *conn, const char *folder)
{
    if (!folder || strlen(folder) < 1)
        return false;
    json_t *data = NULL;
    json_error_t err;
    bool result = false;

    if (json_pack_ex(&err, 0, "{ss}", "rec-folder", folder)) {
        werr("Packing json for SetRecordingFolder: %s at %i", err.text, err.line);
    } else {
        result = send_request_no_cb(conn, "SetRecordingFolder", data);
        json_decref(data);
    }

    return result;
}

request_result_t get_recording_folder_callback(json_t *data, void *d)
{
    char **f = d;
    char *tmp = NULL;
    request_result_t result = REQUEST_ERROR;
    json_error_t err;

    if (json_unpack_ex(data, &err, 0, "{ss}", "rec-folder", &tmp)) {
        werr("Unpacking json for GetRecordingFolder: %s at %i", err.text, err.line);
    } else {
        *f = bstrdup(tmp);
        result = REQUEST_OK;
    }
    return result;
}

bool wsc_get_recording_folder(wsc_connection_t *conn, char **folder)
{
    if (!folder || *folder)
        return false;
    return send_request_no_data(conn, "GetRecordingFolder", get_recording_folder_callback, folder);
}
