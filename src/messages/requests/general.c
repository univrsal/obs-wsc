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

#include "general.h"
#include "../../send.h"
#include "../../util.h"

bool obs_wsc_authenticate(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth)
{
    if (!conn || !conn->connection || !auth || !auth->auth_response)
        return false;
    bool result = false;
    json_error_t err;
    json_t *auth_json = json_pack_ex(&err, 0, "{ss}", "auth", auth->auth_response);

    if (auth_json) {
        /* we make a copy of the json here and then
         * free it in the callback */
        result = send_request_no_cb(conn, "Authenticate", auth_json);
        json_decref(auth_json);
        bfree(auth->auth_response);
        auth->auth_response = NULL;
    } else {
        berr("Error packing auth response json: %s at line %i", err.text, err.line);
    }

    return result;
}

request_result_t auth_required_callback(json_t *response, void *data)
{
    obs_wsc_auth_data_t *auth = data;
    json_error_t err;
    request_result_t result = REQUEST_ERROR;
    char *salt = NULL, *challenge = NULL;

    if (parse_basic_json(response)) {
        bfree(auth->auth_response);
        auth->auth_response = NULL;

        if (json_unpack_ex(response, &err, 0, "{sb}", "authRequired", &auth->required)) {
            berr("Error unpacking auth response json: %s at %i", err.text, err.line);
        } else if (auth->required) {
            if (json_unpack_ex(response, &err, 0, "{ss,ss}", "challenge", &challenge, "salt", &salt)) {
                berr("Couldn't unpack auth json: %s at line %i", err.text, err.line);
            } else {
                auth->challenge = bstrdup(challenge);
                auth->salt = bstrdup(salt);
                result = REQUEST_OK;
            }
        }
    }

    return result;
}

bool obs_wsc_auth_required(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth)
{
    if (!conn || !conn->connection || !auth)
        return false;

    return send_request_no_data(conn, "GetAuthRequired", auth_required_callback, auth);
}

request_result_t version_info_callback(json_t *response, void *data)
{
    char *obs = NULL, *ws = NULL, *reqs = NULL, *formats = NULL;
    obs_wsc_version_info_t *inf = data;
    json_error_t err;
    bool basic = parse_basic_json(response);
    request_result_t result = REQUEST_ERROR;

    if (basic &&
        json_unpack_ex(response, &err, 0, "{sf,ss,ss,ss}", "version", &inf->api_version, "obs-websocket-version", &ws,
                       "obs-studio-version", &obs, "available-requests", &reqs) == 0) {
        util_strncpy(inf->obs_version, STR_LEN, obs, STR_LEN);
        util_strncpy(inf->obs_websocket_version, STR_LEN, ws, STR_LEN);
        util_strncpy(inf->available_requests, LONG_STR_LEN, reqs, LONG_STR_LEN);

        /* this doesn't seem to be sent currently so no error checking for it */
        json_unpack(response, "{ss}", "supported-image-export-formats", &formats);
        if (formats)
            util_strncpy(inf->supported_image_formats, STR_LEN, formats, STR_LEN);
        result = REQUEST_OK;
    } else if (basic) {
        berr("Error unpacking response for GetVersion: %s at %i", err.text, err.line);
    }
    return result;
}

bool obs_wsc_get_version_info(obs_wsc_connection_t *conn, obs_wsc_version_info_t *inf)
{
    if (!conn || !inf)
        return false;

    return send_request(conn, "GetVersion", NULL, version_info_callback, inf);
}

request_result_t video_info_callback(json_t *response, void *data)
{
    char *scale_type, *video_format, *color_space, *color_range;
    obs_wsc_video_info_t *inf = data;
    json_error_t err;
    bool basic = parse_basic_json(response);
    request_result_t result = REQUEST_ERROR;

    if (basic && json_unpack_ex(response, &err, 0, "{si,si,si,si,sf,ss,ss,ss,ss}", "baseWidth", &inf->base_width,
                                "baseHeight", &inf->base_height, "outputWidth", &inf->output_width, "outputHeight",
                                &inf->output_height, "fps", &inf->fps, "scaleType", &scale_type, "videoFormat",
                                &video_format, "colorSpace", &color_space, "colorRange", &color_range) == 0) {
        util_strncpy(inf->scale_type, STR_LEN, scale_type, STR_LEN);
        util_strncpy(inf->video_format, STR_LEN, video_format, STR_LEN);
        util_strncpy(inf->color_range, STR_LEN, color_range, STR_LEN);
        util_strncpy(inf->color_space, STR_LEN, color_space, STR_LEN);
        result = REQUEST_OK;
    } else if (basic) {
        berr("Error unpacking response for GetVideoInfo: %s at %i", err.text, err.line);
    }
    return result;
}

bool obs_wsc_get_video_info(obs_wsc_connection_t *conn, obs_wsc_video_info_t *inf)
{
    if (!conn || !inf)
        return false;

    return send_request(conn, "GetVideoInfo", NULL, video_info_callback, inf);
}

bool obs_wsc_set_heartbeat(obs_wsc_connection_t *conn, bool state)
{
    if (!conn)
        return false;
    json_t *state_json = json_pack("{sb}", "enable", state);
    bool result = send_request_no_cb(conn, "SetHeartbeat", state_json);
    json_decref(state_json);
    return result;
}

bool obs_wsc_set_filename_format(obs_wsc_connection_t *conn, const char *format)
{
    if (!conn)
        return false;
    json_t *format_json = json_pack("{ss}", "enable", format);
    bool result = send_request_no_cb(conn, "SetFilenameFormatting", format_json);
    json_decref(format_json);
    return result;
}

request_result_t filename_format_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    json_error_t err;
    char **fn = data;
    char *tmp = NULL;
    bool basic = parse_basic_json(response);

    if (basic && json_unpack_ex(response, &err, 0, "{ss}", &tmp) == 0) {
        *fn = bstrdup(tmp);
        result = REQUEST_OK;
    } else if (basic) {
        berr("Error unpacking response for GetFilenameFormatting: %s at %i", err.text, err.line);
    }
    return result;
}

bool obs_wsc_get_filename_format(obs_wsc_connection_t *conn, char **format)
{
    if (!conn || !format || *format)
        return false;
    return send_request_no_data(conn, "GetFilenameFormatting", filename_format_callback, format);
}

request_result_t stats_callback(json_t *response, void *data)
{
    request_result_t result = REQUEST_ERROR;
    obs_wsc_stats_t *stats = data;
    bool basic = parse_basic_json(response);
    json_error_t err;
    json_t *stats_obj = NULL;

    if (basic && json_unpack_ex(response, &err, 0, "{so}", "stats", &stats_obj) == 0) {
        if (json_unpack_ex(stats_obj, &err, 0, "{sf,si,si,si,si,sf,sf,sf,sf}", "fps", &stats->fps,
                           "render-total-frames", &stats->render_total_frames, "render-missed-frames",
                           &stats->render_missed_frames, "output-total-frames", &stats->output_total_frames,
                           "output-skipped-frames", &stats->output_skipped_frames, "average-frame-time",
                           &stats->average_frame_time, "cpu-usage", &stats->cpu_usage, "memory-usage",
                           &stats->memory_usage, "free-disk-space", &stats->free_disk_space) == 0) {
            result = REQUEST_OK;
        } else {
            berr("Error unpacking response for GetStats: %s at %i", err.text, err.line);
        }
    } else if (basic) {
        berr("Error unpacking response for GetStats: %s at %i", err.text, err.line);
    }
    return result;
}

bool obs_wsc_get_stats(obs_wsc_connection_t *conn, obs_wsc_stats_t *stats)
{
    if (!conn || !stats)
        return false;
    return send_request_no_data(conn, "GetStats", stats_callback, stats);
}

bool obs_wsc_broadcast_message(obs_wsc_connection_t *conn, const char *realm, const char *data)
{
    if (!conn || !realm)
        return false;

    json_error_t err;
    json_t *d = NULL;
    json_t *ad = json_loads(data, 0, &err);

    if (!ad) {
        bwarn("obs_wsc_broadcast_message was provided with invalid json: %s at %i", err.text, err.line);
    }

    d = json_pack_ex(&err, 0, "{ss,so?}", "realm", realm, "data", ad);

    if (!d) {
        bwarn("Packing json for BroadcastCustomMessage failed: %s at %i", err.text, err.line);
    }

    bool result = send_request_no_cb(conn, "BroadcastCustomMessage", d);
    json_decref(ad);
    json_decref(d);
    return result;
}

void obs_wsc_prepare_geomery(obs_wsc_geometry_t *geo)
{
    if (geo) {
        memset(geo, 0, sizeof(obs_wsc_geometry_t));
        geo->magic_number = 0x1D9D0CB;
        geo->version_major = 3;
        geo->version_minor = 0;
    }
}

bool obs_wsc_open_projector(obs_wsc_connection_t *conn)
{
    return obs_wsc_open_projector2(conn, WSC_PROJECTOR_PREVIEW, -1, NULL, NULL);
}

bool obs_wsc_open_projector2(obs_wsc_connection_t *conn, enum obs_wsc_projector_type t, int32_t monitor,
                             const obs_wsc_geometry_t *geo, const char *name)
{
    if (!conn)
        return false;
    if (monitor < 0 && !geo)
        return false;
    if ((t == WSC_PROJECTOR_SCENE || t == WSC_PROJECTOR_SOURCE) && !name)
        return false;

    bool result = false;
    char *b64_geo = NULL;
    const char *type = util_projector_type(t);
    if (geo)
        b64_geo = util_qt_geometry_string(geo);
    json_error_t err;
    json_t *data = NULL;

    if (b64_geo && name) {
        data =
            json_pack_ex(&err, 0, "{ss,si,ss,ss}", "type", type, "monitor", monitor, "geometry", b64_geo, "name", name);
    } else if (b64_geo) {
        data = json_pack_ex(&err, 0, "{ss,si,ss}", "type", type, "monitor", monitor, "geometry", b64_geo);
    } else {
        data = json_pack_ex(&err, 0, "{ss,si,ss}", "type", type, "monitor", monitor);
    }

    if (data) {
        result = send_request_no_cb(conn, "OpenProjector", data);
        json_decref(data);
    } else {
        berr("Failed to pack json for OpenProjector: %s at %i", err.text, err.line);
    }

    bfree(b64_geo);
    return result;
}
