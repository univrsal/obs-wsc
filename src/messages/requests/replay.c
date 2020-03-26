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

#include "replay.h"
#include "send.h"

bool wsc_toggle_replay_buffer(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StartStopReplayBuffer");
}

bool wsc_start_replay_buffer(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StartReplayBuffer");
}

bool wsc_stop_replay_buffer(wsc_connection_t *conn)
{
    return send_request_simple(conn, "StopReplayBuffer");
}

bool wsc_save_replay_buffer(wsc_connection_t *conn)
{
    return send_request_simple(conn, "SaveReplayBuffer");
}
