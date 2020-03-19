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
#include "../../opaque.h"
#include "../../misc_types.h"

/**
 * @brief Checks if the connections needs a password
 * @param conn The connection
 * @param auth Pointer to a data struct which will receive a challange and a
 *        salt or required will be set to false.
 *        The salt and challange have to be freed with obs_wsc_free_auth_data
 *        if it's not used for obs_wsc_prepare_auth
 * @return true on success
 */
EXPORT bool obs_wsc_auth_required(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth);

/**
 * @brief Tries to autenticate with the server with the provided data
 * @param conn The connection
 * @param auth The authentication data
 * @see obs_wsc_prepare_auth
 * @return true on success
 */
EXPORT bool obs_wsc_authenticate(obs_wsc_connection_t *conn, obs_wsc_auth_data_t *auth);

/**
 * @brief Request basic OBS video info
 * @param inf pointer to video info structure
 * @return true on success
 */
EXPORT bool obs_wsc_get_video_info(obs_wsc_connection_t *conn, obs_wsc_video_info_t *inf);
