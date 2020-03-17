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

#include "external/base.h"
#include "external/mongoose.h"
#include <jansson.h>

typedef struct obs_wsc_msg_s {
    json_t *data;
    long time;
} obs_wsc_msg_t;

typedef struct obs_wsc_connection_s {
    int32_t timeout;
    char *domain;
    struct mg_mgr manager;
    struct mg_connection *connection;
    char **message_ids;
    size_t message_ids_len;
    volatile bool thread_flag;
    volatile bool connected;
    pthread_t poll_thread;
    pthread_mutex_t poll_mutex;
    obs_wsc_msg_t last_message;
} obs_wsc_connection_t;
