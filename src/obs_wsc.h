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

typedef struct obs_wsc_connection_s obs_wsc_connection_t;

#include "misc_types.h"
#include "messages/requests/general.h"
#include "external/base.h"

/**
 * @brief Opens a new connection to an obs websocket instance
 * @param addr The address (eg. ws://127.0.0.1:4444)
 * @return the new connection or NULL on error
 */
EXPORT obs_wsc_connection_t *obs_wsc_connect(const char *addr);

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
 * @brief Frees the challange and salt strings
 * @param data the data to free
 */
void obs_wsc_free_auth_data(obs_wsc_auth_data_t *data);

/**
 * @brief Creates the auth response, by hashing the password and encoding it
 *        as required by the protocol. This will free the salt and challange
 *        returned in obs_wsc_auth_required afterwards
 * @param auth The auth data struct containing a challange and a salt from
 *        obs_wcs_auth_required
 * @see obs_wcs_auth_required
 * @param password The password to login
 * @return true on success
 */
EXPORT bool obs_wsc_prepare_auth(obs_wsc_auth_data_t *auth, const char *password);

/**
 * @brief Initialize obs_wsc
 * @return true on success
 */
EXPORT void obs_wsc_init();

/**
 * @brief Check for memory leaks
 * @return The amount of memory leaks
 */
EXPORT long obs_wsc_shutdown();

/**
 * @brief Make obs-wsc use different allocators
 * @param defs Structure with pointers to memory allocation functions
 */
EXPORT void obs_wsc_set_allocator(struct base_allocator *defs);

/**
 * @brief Get logging function
 * @param handler Pointer to save the log function to
 * @param param Pointer to save the custom parameter to
 */
EXPORT void obs_wsc_get_logger(log_handler_t *handler, void **param);

/**
 * @brief Set the logging function
 * @param handler The log function
 * @param param Custom paramter
 */
EXPORT void obs_wsc_set_logger(log_handler_t handler, void *param);

/**
 * @brief Set the function to call if obs_wsc crashes
 * @param handler The handler function
 * @param param Custom parameter
 */
EXPORT void obs_wsc_set_crash_handler(void (*handler)(const char *, va_list, void *), void *param);
#ifdef __cplusplus
}
#endif
