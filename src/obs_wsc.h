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
#include "external/base.h"
/**
 * @brief Opens a new connection to an obs websocket instance
 * @param addr The address (eg. ws://127.0.0.1:4444)
 * @return the new connection or NULL on error
 */
EXPORT obs_wsc_connection_t * obs_wsc_connect(const char *addr);

/**
 * @brief Close an active connection
 */
EXPORT void obs_wsc_disconnect(obs_wsc_connection_t *conn);

/**
 * @brief Set the wait timeout when waiting for responses, use -1 for no timeout
 * @param conn The connection
 * @param ms The timeout in miliseconds
 */
EXPORT void obs_wsc_set_timeout(obs_wsc_connection_t *conn, int32_t ms);

/**
 * @brief Send a request to the websocket API
 * @param conn The connection
 * @param req The request id
 * @param additional_data Optional data if needed for the request
 * @return the servers response or null on failure
 */
//EXPORT  *obs_wcs_send_request(obs_wsc_connection_t *conn, const char *req,
//                                 const json_t *additional_data);

/**
 * @brief Checks if the connections needs a password
 * @param conn The connection
 * @param auth Pointer to a data struct which will receive a challange and a
 *        salt (optional)
 * @return true on success
 */
EXPORT bool obs_wsc_auth_required(obs_wsc_connection_t *conn,
                                  obs_wsc_auth_data_t *auth);

/**
 * @brief Tries to autenticate with the server with the provided data
 * @param conn The connection
 * @param auth The authentication data
 * @see obs_wsc_prepare_auth
 * @return true on success
 */
EXPORT bool obs_wsc_authenticate(obs_wsc_connection_t *conn,
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
EXPORT bool obs_wsc_prepare_auth(obs_wsc_auth_data_t *auth,
                                 const char *password);

/**
 * @brief Initialize network
 * @return true on success
 */
EXPORT bool obs_wsc_init();

/**
 * @brief Shutdown network and check for memory leaks
 */
EXPORT void obs_wsc_shutdown();

/**
 * @brief Make obs-wsc use different allocators
 * @param defs Structure with pointers to memory allocation functions
 */
EXPORT void obs_wsc_set_allocator(struct base_allocator *defs);

#ifdef __cplusplus
}
#endif
