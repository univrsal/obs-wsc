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
#include "external/darray.h"
#include "external/mongoose.h"
#include "external/threading.h"
#include <jansson.h>

typedef enum { REQUEST_PENDING, REQUEST_OK, REQUEST_ERROR } request_result_t;

/**
 * Each request comes with a callback
 * which is called, once the server sends a reply
 * with the matching message id, the callback will
 * return wether it succeeded
 */
typedef request_result_t (*request_callback_t)(json_t *response, void *request_data);

typedef struct request_s {
    request_callback_t cb;
    void *cb_data;
    char *id;
    request_result_t status;
    struct request_s *next;
    struct request_s *prev;
    uint64_t request_start;
} request_t;

typedef struct obs_wsc_connection_s {
    int32_t timeout;
    char *domain;

    struct mg_mgr manager;
    struct mg_connection *connection;

    volatile bool thread_flag;
    volatile bool connected;

    DARRAY(char *) ids;
    request_t *first_active_request;

    pthread_t poll_thread;
    pthread_mutex_t poll_mutex;
} obs_wsc_connection_t;
