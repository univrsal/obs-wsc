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

#include <cheat.h>
#include <obs_wsc.h>

CHEAT_TEST(init,
    obs_wsc_init();
)

CHEAT_TEST(connect,
    obs_wsc_connection_t *conn = NULL;
    cheat_assert((conn = obs_wsc_connect(NULL)) != NULL);
    obs_wsc_disconnect(conn);
)

CHEAT_TEST(auth_required,
    obs_wsc_connection_t *conn = obs_wsc_connect(NULL);
    obs_wsc_auth_data_t auth;
    cheat_assert(obs_wsc_auth_required(conn, &auth));
    obs_wsc_disconnect(conn);
)

CHEAT_TEST(shutdown,
    obs_wsc_shutdown();
)
