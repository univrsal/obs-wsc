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

#include "obs_wsc.h"
#include "opaque.h"
#include "send.h"
#include "util.h"
#include "external/bmem.h"
#include "external/platform.h"
#include <stdlib.h>
#include <time.h>

typedef struct obs_wsc_connection_s obs_wsc_connection_t;

bool obs_wsc_init()
{
    /* rand() is only used for generating message ids */
    time_t t;
    srand((unsigned) time(&t));

    return true;
}

void obs_wsc_shutdown()
{
    if (bnum_allocs() > 0)
        bwarn("Number of memory leaks: %li", bnum_allocs());
}

void obs_wsc_set_allocator(struct base_allocator *defs)
{
    base_set_allocator(defs);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    obs_wsc_connection_t *conn = nc->user_data;
    int status;
    struct http_message *msg;
    struct websocket_message *wm;

    pthread_mutex_lock(&conn->poll_mutex);
    switch (ev) {
        case MG_EV_CONNECT:
            status = *((int *) ev_data);
            if (status != 0) {
                berr("Connection for %s failed with code %i", conn->domain,
                     status);
            }
            conn->connected = true;
            break;
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
            msg = (struct http_message *) ev_data;
            if (msg->resp_code == 101)
                binfo("Connected to %s", conn->domain);
            break;
        case MG_EV_CLOSE:
            if (conn->connected)
                binfo("Disconnected from %s", conn->domain);
            conn->connected = false;
            conn->thread_flag = false;
            break;
        case MG_EV_WEBSOCKET_FRAME:
            wm = ev_data;
            binfo("Received message: %.*s", (int) wm->size, wm->data);
            json_decref(conn->last_message.data);
            conn->last_message.data = recv_json(wm->data, wm->size);
            conn->last_message.time = util_epoch();
            break;
    }
    pthread_mutex_unlock(&conn->poll_mutex);
}

static void *wsc_thread(void *arg)
{
    obs_wsc_connection_t *conn = arg;

    while (conn->thread_flag) {
        mg_mgr_poll(&conn->manager, conn->timeout);
    }

    return NULL;
}

obs_wsc_connection_t *obs_wsc_connect(const char *addr)
{
    static const char *local_host = "ws://127.0.0.1:4444";

    if (!addr)
        addr = local_host;

    obs_wsc_connection_t *n = bzalloc(sizeof(obs_wsc_connection_t));
    n->timeout = 500;

    mg_mgr_init(&n->manager, n);
    n->connection = mg_connect_ws(&n->manager, ev_handler, addr, "websocket", NULL);

    if (!n->connection) {
        berr("mg_connect_ws faild for %s", addr);
        goto fail;
    }

    if (pthread_mutex_init(&n->poll_mutex, NULL)) {
        berr("Couldn't initialize poll mutex");
        goto fail;
    }

    n->thread_flag = true;
    n->connection->user_data = n;
    n->domain = bstrdup(addr);

    if (pthread_create(&n->poll_thread, NULL, wsc_thread, n)) {
        berr("Couldn't initialize poll thread");
        goto fail;
    }

    /* Wait for connection */
    uint32_t wait = 0;
    while (!n->connected && wait < 1000) {
        wait += 10;
        os_sleep_ms(10);
    }

    if (wait >= 1000) {
        berr("Timed out while waiting for handshake response");
        goto fail;
    }

    return n;

    fail:
    obs_wsc_disconnect(n);
    return NULL;
}

void obs_wcs_set_timeout(obs_wsc_connection_t *conn, int32_t ms)
{
    if (conn)
        conn->timeout = ms;
}

void obs_wsc_disconnect(obs_wsc_connection_t *conn)
{
    if (!conn)
        return;

    for (size_t i = 0; i < conn->message_ids_len; i++)
        bfree(conn->message_ids[i]);
    bfree(conn->message_ids);

    mg_mgr_free(&conn->manager);

    pthread_mutex_lock(&conn->poll_mutex);
    conn->thread_flag = false;
    pthread_mutex_unlock(&conn->poll_mutex);

    if (pthread_join(conn->poll_thread, NULL))
        berr("Failed to join poll thread for %s", conn->domain);

    pthread_mutex_destroy(&conn->poll_mutex);
    if (conn->domain)
        bfree(conn->domain);
    bfree(conn);
}

bool obs_wsc_auth_required(obs_wsc_connection_t *conn, obs_wsc_auth_data_t
                           *auth)
{
    if (!conn || !conn->connection)
        return false;
    bool result = false;
    if (send_request(conn, "GetAuthRequired", NULL)) {
        result = true;
        /* TODO: parse salt and challenge */
        json_t *response = json_copy(conn->last_message.data);


        json_decref(response);
    }
    return true;
}

bool obs_wsc_authenticate(obs_wsc_connection_t *conn,
                                 const obs_wsc_auth_data_t *auth)
{
    return false;
}
