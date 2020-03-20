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

    return send_request(conn, "GetAuthRequired", NULL, auth_required_callback, auth);
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
