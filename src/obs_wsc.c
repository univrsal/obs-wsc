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
#include "websocket.h"
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
    switch (ev) {

    }
}

obs_wsc_connection_t *obs_wsc_connect(const char *addr)
{
    static const char *local_host = "ws://127.0.0.1:4444";

    if (!addr)
        addr = local_host;

    obs_wsc_connection_t *n = bzalloc(sizeof(obs_wsc_connection_t));
    n->timeout = 500;

    mg_mgr_init(&n->manager, n);
    n->connection = mg_connect_ws(&n->manager, ev_handler, addr, "obs-websocket", NULL);

    if (!n->connection) {
        berr("mg_connect_ws faild for %s", addr);
        goto fail;
    }

    n->domain = bstrdup(addr);
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
    if (conn->sock)
        netlib_tcp_close(conn->sock);
    if (conn->domain)
        bfree(conn->domain);

    for (size_t i = 0; i < conn->message_ids_len; i++)
        bfree(conn->message_ids[i]);

    bfree(conn->message_ids);
    bfree(conn);
}

bool obs_wsc_auth_required(obs_wsc_connection_t *conn, obs_wsc_auth_data_t
                           *auth)
{
    if (!conn || !conn->sock)
        return false;
    bool result = false;
    json_t *response = send_request(conn, "GetAuthRequired", NULL);
    if (response) {
        result = true;
        /* TODO: parse salt and challenge */
        json_decref(response);
    }
    return true;
}

bool obs_wsc_authenticate(obs_wsc_connection_t *conn,
                                 const obs_wsc_auth_data_t *auth)
{
    return false;
}
