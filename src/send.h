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

bool send_request(obs_wsc_connection_t *conn, const char *request, const json_t *additional_data);

bool send_json(const obs_wsc_connection_t *conn, const json_t *json);

bool send_str(const obs_wsc_connection_t *conn, const char *str);

json_t *recv_json(unsigned char *data, size_t len);

bool parse_basic_json(json_t *j, const char *last_msg_id);

bool wait_timeout(obs_wsc_connection_t *conn, uint64_t start);
