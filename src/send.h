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

#pragma once
#include <stdbool.h>
#include <jansson.h>
#include "opaque.h"
#include "external/bmem.h"

bool send_request(obs_wsc_connection_t *conn, const char *request, json_t *additional_data, request_callback_t cb,
                  void *cb_data);

bool send_json(obs_wsc_connection_t *conn, const json_t *json);

bool send_str(obs_wsc_connection_t *conn, const char *str);

json_t *recv_json(unsigned char *data, size_t len);

bool parse_basic_json(json_t *j);

bool wait_timeout(obs_wsc_connection_t *conn, request_t *rq);

static inline bool send_request_no_data(obs_wsc_connection_t *conn, const char *req, request_callback_t cb,
                                        void *cb_data)
{
    return send_request(conn, req, NULL, cb, cb_data);
}

static inline bool send_request_no_cb(obs_wsc_connection_t *conn, const char *req, json_t *additional_data)
{
    return send_request(conn, req, additional_data, NULL, NULL);
}

static inline bool send_request_simple(obs_wsc_connection_t *conn, const char *req)
{
    return send_request(conn, req, NULL, NULL, NULL);
}
