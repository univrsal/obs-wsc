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

request_result_t default_callback(json_t *j, void *d)
{
    UNUSED_PARAMETER(d);
    char *id = NULL;
    json_unpack(j, "{ss}", "message-id", &id);
    wdebug("Request with id %s fulfilled", id);
    return REQUEST_OK;
}

request_t *add_request(wsc_connection_t *conn, char *msg_id, request_callback_t cb, void *cb_data)
{
    pthread_mutex_lock(&conn->poll_mutex);
    darray_resize(sizeof(char *), &conn->ids.da, conn->ids.num + 1);
    conn->ids.array[conn->ids.num - 1] = msg_id;

    char *test = conn->ids.array[conn->ids.num - 1];

    request_t *rq = bzalloc(sizeof(request_t));

    if (cb)
        rq->cb = cb;
    else
        rq->cb = default_callback;

    rq->cb_data = cb_data;
    rq->id = msg_id;
    rq->next = conn->first_active_request;

    if (conn->first_active_request)
        conn->first_active_request->prev = rq;
    conn->first_active_request = rq;
    conn->request_count++;
    pthread_mutex_unlock(&conn->poll_mutex);

    return rq;
}

void remove_request(wsc_connection_t *conn, char *msg_id)
{
    pthread_mutex_lock(&conn->poll_mutex);
    request_t *r = conn->first_active_request;

    while (r) {
        if (strcmp(r->id, msg_id) == 0) {
            if (r->prev)
                r->prev->next = r->next;
            if (r->next)
                r->next->prev = r->prev;
            bfree(r);

            if (--conn->request_count < 1)
                conn->first_active_request = NULL;
            break;
        }
        r = r->next;
    }
    pthread_mutex_unlock(&conn->poll_mutex);
}

bool send_request(wsc_connection_t *conn, const char *request, json_t *additional_data, request_callback_t cb,
                  void *cb_data)
{
    if (!conn || !conn->connection || !conn->connected || !request) {
        werr("Invalid arguments provided to send_request");
        return false;
    }

    json_t *req = NULL;
    json_error_t err;
    bool result = false;

    wdebug("Sending %s request", request);

    pthread_mutex_lock(&conn->poll_mutex);
    char *msg_id = bstrdup(util_random_id(&conn->ids.da));
    pthread_mutex_unlock(&conn->poll_mutex);

    req = json_pack_ex(&err, 0, "{ss,ss}", "request-type", request, "message-id", msg_id);

    if (req) {
        if (additional_data) {
            const char *key;
            json_t *value;
            json_incref(additional_data);
            /* Stealing the reference here, results in Heap warnings */
            json_object_foreach (additional_data, key, value) {
                json_object_set(req, key, value);
            }
        }

        if (send_json(conn, req)) {
            request_t *rq = add_request(conn, msg_id, cb, cb_data);
            result = wait_timeout(conn, rq);
        } else {
            werr("Sending request json failed");
        }

        remove_request(conn, msg_id);
        json_decref(additional_data);
    } else {
        werr("Packing request json for %s failed with %s at line %i", request, err.text, err.line);
        bfree(msg_id);
    }

    json_decref(req);
    return result;
}

bool send_json(wsc_connection_t *conn, const json_t *json)
{
    if (!conn || !json)
        return false;

    char *dmp = json_dumps(json, 0);
    bool result = send_str(conn, dmp);
    bfree(dmp);
    return result;
}

bool send_str(wsc_connection_t *conn, const char *str)
{
    size_t len = strlen(str) + 1; /* Include \0 */

    if (!conn || !conn->connection || !conn->connected || !str || len < 1)
        return false;

    pthread_mutex_lock(&conn->poll_mutex);
    mg_send_websocket_frame(conn->connection, WEBSOCKET_OP_TEXT, str, len);
    pthread_mutex_unlock(&conn->poll_mutex);

    return true;
}

json_t *recv_json(unsigned char *data, size_t len)
{
    json_error_t err;
    json_t *loaded = json_loadb((char *)data, len, 0, &err);

    if (loaded)
        return loaded;
    werr("Failed to parse response json: %s at line %i", err.text, err.line);
    return NULL;
}

bool wait_timeout(wsc_connection_t *conn, request_t *rq)
{
    bool keep_waiting = true;
    int32_t timeout = 0;
    pthread_mutex_lock(&conn->poll_mutex);
    int32_t max_timeout = conn->timeout;
    char *id = bstrdup(rq->id);
    pthread_mutex_unlock(&conn->poll_mutex);

    while (timeout <= max_timeout && keep_waiting) {

        pthread_mutex_lock(&conn->poll_mutex);
        if (rq->status != REQUEST_PENDING) {
            wdebug("Received response for %s within timeout, callback status: "
                   "%s",
                   rq->id, rq->status == REQUEST_OK ? "OK" : "FAILED");
            keep_waiting = false;
        }
        pthread_mutex_unlock(&conn->poll_mutex);

        timeout += 2;
        os_sleep_ms(2);
    }

    if (keep_waiting)
        werr("Request %s timed out", id);

    bfree(id);
    return timeout <= max_timeout;
}

bool parse_basic_json(json_t *j)
{
    if (!j)
        return false;

    /* Message id isn't matched since the poll thread only
     * calls the callback on requests with a matching id */
    json_error_t err;
    char *status = NULL;
    bool status_ok = false;

    if (json_unpack_ex(j, &err, 0, "{ss}", "status", &status)) {
        werr("Error while parsing basic json: '%s' at line %i", err.text, err.line);
    } else {
        status_ok = strcmp(status, "ok") == 0;
    }

    if (!status_ok) {
        werr("Status %s is not ok", status);
        if (strcmp(status, "error") == 0) {
            char *err = NULL;
            json_unpack(j, "{ss}", "error", &err);
            werr("Error message: %s", err);
        }
    }

    return status_ok;
}
