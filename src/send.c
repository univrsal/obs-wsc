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

#include "send.h"
#include "util.h"
#include "external/c99defs.h"
#include "external/platform.h"
#include "external/bmem.h"
#include <string.h>

json_t *send_request(obs_wsc_connection_t *conn, const char *request,
                  const json_t *additional_data)
{
    json_t *response = NULL, *req = NULL;
    json_error_t err;

    if (!conn || !conn->connection || !request)
        return NULL;

    char *msg_id = random_id(conn);
    req = json_pack_ex(&err, 0, "{ss,ss}", "request-type", request,
                       "message-id", msg_id);
    if (req) {
        if (additional_data) {
            /* TODO: append json */
        }
        conn->message_ids = brealloc(conn->message_ids,sizeof(char*)
                                     * (conn->message_ids_len + 1));
        conn->message_ids[conn->message_ids_len++] = msg_id;

        if (!send_json(conn, req)) {
            berr("Sending request json failed");
            goto fail;
        }

        if (!wait_timeout(conn))
            goto fail;

        response = recv_json(conn);

        if (!response)
            berr("Couldn't receive response for request %s", request);
    } else {
        berr("Packing request json for %s failed with %s at line %i",
             request, err.text, err.line);
        goto free;
    }

    json_decref(req);
    return response;

    free:
    bfree(msg_id);
    fail:
    json_decref(req);
    return NULL;
}

bool send_json(const obs_wsc_connection_t *conn, const json_t *json)
{
    if (!conn || !json)
        return false;

    char *dmp = json_dumps(json, 0);
    bool result = send_str(conn, dmp);
    bfree(dmp);
    return result;
}

bool send_str(const obs_wsc_connection_t *conn, const char *str)
{
    /* TODO: Are strings sent with length attached? */
    size_t len = strlen(str) + 1; /* Include \0 */

    if (!conn || !conn->connection || !str || len < 1)
        return false;

    if (len > INT32_MAX) {
        berr("String length exceeds int32 max");
        return false;
    }

    return false;
}

json_t *recv_json(const obs_wsc_connection_t *conn)
{
    if (!conn || !conn->connection)
        return NULL;
    char buf[32];
    return NULL;
}

bool wait_timeout(const obs_wsc_connection_t *conn)
{
    bool keep_waiting = true;
    int32_t timeout = 0;

    if (!keep_waiting) {
        berr("Waiting for response exceeded timeout of %i ms",
             conn->timeout);
    }
    return keep_waiting;
}
