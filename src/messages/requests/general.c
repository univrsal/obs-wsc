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

bool obs_wsc_authenticate(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth)
{
    if (!conn || !conn->connection || !auth || !auth->auth_response)
        return false;
    bool result = false;
    json_error_t err;
    json_t *auth_json = json_pack_ex(&err, 0, "{ss}", "auth", auth->auth_response);

    if (auth_json) {
        result = send_request_no_cb(conn, "Authenticate", auth_json);
        json_decref(auth_json);
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

request_result_t video_info_callback(json_t *response, void *data)
{
    char *scale_type, *video_format, *color_space, *color_range;
    obs_wsc_video_info_t *inf = data;
    json_error_t err;
    request_result_t result = REQUEST_ERROR;

    if (parse_basic_json(response) &&
        json_unpack_ex(response, &err, 0, "{si,si,si,si,si,ss,sf,ss,ss,ss}", "baseWidth", &inf->base_width,
                       "baseHeight", &inf->base_height, "outputWidth", &inf->output_width, "outputHeight",
                       &inf->output_height, "fps", &inf->fps, "scaleType", &scale_type, "videoFormat", &video_format,
                       "colorSpace", &color_space, "colorRange", &color_range)) {
        strncpy(inf->scale_type, scale_type, 33);
        strncpy(inf->video_format, video_format, 33);
        strncpy(inf->color_range, color_space, 33);
        strncpy(inf->color_space, color_space, 33);
        result = REQUEST_OK;
    } else {
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
