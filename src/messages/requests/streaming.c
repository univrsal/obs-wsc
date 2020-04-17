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

#include "streaming.h"
#include "send.h"

bool wsc_toggle_streaming(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StartStopStreaming");
}

bool wsc_start_streaming(wsc_connection_t *conn, const wsc_stream_settings_t *s)
{
    bool result = false;
    json_t *data = NULL;

    if (s) {
        json_error_t err;

        data = json_pack_ex(&err, 0, "{s{ss?,ss?},s{ss?,ss?,sb,ss?,ss?}", "stream", "type", s->stream.type, "metadata",
                            s->stream.metadata, "server", s->settings.server, "settings", "key", s->settings.key,
                            "use_auth", s->settings.use_auth, "username", s->settings.username, "password",
                            s->settings.password);

        if (!data)
            werr("Error while packing json for StartStreaming: %s at %i", err.text, err.line);
    }

    if (data) {
        result = send_request_no_cb(conn, "StartStreaming", data);
        json_decref(data);
    } else {
        result = send_request_simple(conn, "StartStreaming");
    }

    return result;
}

bool wsc_stop_streaming(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StopStreaming");
}

request_result_t get_status_cb(json_t *response, void *data)
{
    wsc_stream_status_t *s = data;
    request_result_t result = REQUEST_ERROR;

    json_error_t err;
    char *st = NULL, *rt = NULL;

    if (json_unpack_ex(response, &err, 0, "{sb,sb,s?s,s?s}", "streaming", &s->streaming, "recording", &s->recording,
                       "stream-timecode", &st, "rec-timecode", &rt)) {
        werr("Unpacking json for GetStreamingStatus: %s at %i", err.text, err.line);
    } else {
        result = REQUEST_OK;
        s->rec_timecode = bstrdup(rt);
        s->stream_timecode = bstrdup(st);
    }

    return result;
}

bool wsc_get_streaming_status(wsc_connection_t *conn, wsc_stream_status_t *s)
{
    if (!s)
        return false;
    wsc_free_streaming_status(s);

    return send_request_no_data(conn, "GetStreamingStatus", get_status_cb, s);
}

void wsc_free_streaming_status(wsc_stream_status_t *s)
{
    if (s) {
        bfree(s->rec_timecode);
        bfree(s->stream_timecode);
        s->rec_timecode = NULL;
        s->stream_timecode = NULL;
        s->streaming = false;
        s->recording = false;
    }
}

void wsc_free_stream_settings(wsc_stream_settings_t *s)
{
    if (s) {
        bfree(s->stream.type);
        bfree(s->settings.key);
        bfree(s->settings.server);
        bfree(s->settings.username);
        bfree(s->settings.password);

        s->stream.type = NULL;
        s->settings.key = NULL;
        s->settings.server = NULL;
        s->settings.username = NULL;
        s->settings.password = NULL;
    }
}

bool wsc_set_stream_settings(wsc_connection_t *conn, const wsc_stream_settings_t *s)
{
    if (!s)
        return false;

    bool result = false;
    json_t *data = NULL;
    json_error_t err;

    data = json_pack_ex(&err, 0, "{s{ss?,ss?},s{ss?,ss?,sb,ss?,ss?}", "stream", "type", s->stream.type, "metadata",
                        s->stream.metadata, "server", s->settings.server, "settings", "key", s->settings.key,
                        "use_auth", s->settings.use_auth, "username", s->settings.username, "password",
                        s->settings.password);

    if (!data) {
        werr("Error while packing json for SetStreamSetting: %s at %i", err.text, err.line);
    } else {
        result = send_request_simple(conn, "SetStreamSettings");
        json_decref(data);
    }

    return result;
}

request_result_t get_stream_settings_cb(json_t *response, void *data)
{
    wsc_stream_settings_t *s = data;
    request_result_t result = REQUEST_ERROR;

    json_error_t err;
    char *type = NULL, *server = NULL, *key = NULL, *username = NULL, *password = NULL;

    if (json_unpack_ex(response, &err, 0, "{s{s?s,s?s},s{s?s,s?s,sb,s?s,s?s}", "type", &type, "settings", "server",
                       server, "key", &key, "use_auth", &s->settings.use_auth, "username", &s->settings.username,
                       "password", &s->settings.password)) {
        werr("Unpacking json for GetStreamSettings: %s at %i", err.text, err.line);
    } else {
        result = REQUEST_OK;
        s->stream.type = bstrdup(type);
        s->settings.server = bstrdup(server);
        s->settings.key = bstrdup(key);
        s->settings.username = bstrdup(username);
        s->settings.password = bstrdup(password);
    }

    return result;
}

bool wsc_get_stream_settings(wsc_connection_t *conn, wsc_stream_settings_t *s)
{
    if (!s)
        return false;
    return send_request_no_data(conn, "GetStreamSettings", get_stream_settings_cb, s);
}

bool wsc_save_stream_settings(wsc_connection_t *conn)
{
    return send_request_simple(conn, "SaveStreamSettings");
}

bool wsc_send_captions(wsc_connection_t *conn, const char *captions)
{
    if (!captions || strlen(captions) < 1)
        return false;
    bool result = false;
    json_error_t err;
    json_t *data = json_pack_ex(&err, 0, "{ss}", "text", captions);

    if (data) {
        result = send_request_no_cb(conn, "SendCaptions", data);
        json_decref(data);
    } else {
        werr("Error while packing json for SendCaptions: %s at %i", err.text, err.line);
    }

    return result;
}
