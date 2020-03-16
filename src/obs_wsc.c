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
#include "external/bmem.h"
#include "external/base.h"
#include <netlib.h>

#define err(a) berr("%s: %s\n", #a, netlib_get_error())

typedef struct obs_wsc_connection_s {
    tcp_socket sock;
    ip_address addr;
    char *domain;
} obs_wsc_connection_t;

obs_wsc_connection_t *obs_wsc_connect(const char *addr, uint16_t port)
{
    static const char *local_host = "localhost";
    if (netlib_init() == -1) {
        err(netlib_init);
        return NULL;
    }

    if (!addr)
        addr = local_host;

    obs_wsc_connection_t *n = bzalloc(sizeof(obs_wsc_connection_t));

    if (netlib_resolve_host(&n->addr, addr, port) < 0) {
        err(netlib_resolve_host);
        goto fail;
    }

    if ((n->sock = netlib_tcp_open(&n->addr)) == NULL) {
        err(netlib_tcp_open);
        goto fail;
    }
    n->domain = bstrdup(addr);

    return n;
    fail:
    obs_wsc_disconnect(n);
    return NULL;
}

void obs_wsc_disconnect(obs_wsc_connection_t *conn)
{
    if (!conn)
        return;
    if (conn->sock)
        netlib_tcp_close(conn->sock);
    if (conn->domain)
        bfree(conn->domain);

    bfree(conn);
}

bool obs_wsc_auth_required(const obs_wsc_connection_t *conn, obs_wsc_auth_data_t
                           *auth)
{
    if (!conn)
        return false;

    return true;
}
