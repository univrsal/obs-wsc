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

#ifdef __cplusplus
extern "C" {
#endif

#include "opaque.h"
#include "misc_types.h"
#include "util.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Opens a new connection to an obs websocket instance
 * @param addr The address (ip or domain)
 * @param port The port
 * @return the new connection or NULL on error
 */
DECLSPEC obs_wsc_connection_t * LIB_CALL obs_wsc_connect(const char *addr,
                                                         uint16_t port);

/**
 * @brief Close an active connection
 */
DECLSPEC void LIB_CALL obs_wsc_disconnect(obs_wsc_connection_t *conn);

/**
 * @brief Checks if the connections needs a password
 * @param conn The connection
 * @param auth Pointer to a data struct which will receive a challange and a
 *        salt (optional)
 * @return true on success
 */
DECLSPEC bool LIB_CALL obs_wsc_auth_required(const obs_wsc_connection_t *conn,
                                             obs_wsc_auth_data_t *auth);

/**
 * @brief Tries to autenticate with the server with the provided data
 * @param conn The connection
 * @param auth The authentication data
 * @see obs_wsc_prepare_auth
 * @return true on success
 */
DECLSPEC bool LIB_CALL obs_wsc_authenticate(const obs_wsc_connection_t *conn,
                                            const obs_wsc_auth_data_t *auth);

/**
 * @brief Creates the auth response, by hashing the password and encoding it
 *        as required by the protocol
 * @param auth The auth data struct containing a challange and a salt from
 *        obs_wcs_auth_required
 * @see obs_wcs_auth_required
 * @param password The password to login
 * @return true on success
 */
DECLSPEC bool LIB_CALL obs_wsc_prepare_auth(obs_wsc_auth_data_t *auth,
                                            const char *password);
#ifdef __cplusplus
}
#endif
