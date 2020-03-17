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

#include "websocket.h"
#include "send.h"

bool ws_handshake(const obs_wsc_connection_t *conn)
{
    static char *header =
           "GET /chat HTTP/1.1\r\n"
           "Host: server.example.com\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
           "Origin: http://example.com\r\n"
           "Sec-WebSocket-Protocol: chat, superchat\r\n"
           "Sec-WebSocket-Version: 13\r\n";

    if (send_str(conn, header)) {
        if (wait_timeout(conn)) {
            json_t *response = recv_json(conn);
        }
    }
    return false;
}
