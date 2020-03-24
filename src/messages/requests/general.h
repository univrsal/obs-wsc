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
#include "../../misc_types.h"

typedef struct wsc_connection_s wsc_connection_t;

/**
 * @brief Checks if the connections needs a password
 * @param conn The connection
 * @param auth Pointer to a data struct which will receive a challange and a
 *        salt or required will be set to false.
 *        The salt and challange have to be freed with wsc_free_auth_data
 *        if it's not used for wsc_prepare_auth
 * @return true on success
 */
EXPORT bool wsc_auth_required(wsc_connection_t *conn, wsc_auth_data_t *auth);

/**
 * @brief Tries to autenticate with the server with the provided data
 * @param auth The authentication data
 * @see wsc_prepare_auth
 * @return true on success
 */
EXPORT bool wsc_authenticate(wsc_connection_t *conn, wsc_auth_data_t *auth);

/**
 * @brief Request obs-studio, websocket, available requests and supported image
 *        formats information
 * @param inf pointer to version info structure
 * @return true on success
 */
EXPORT bool wsc_get_version_info(wsc_connection_t *conn, wsc_version_info_t *inf);

/**
 * @brief Request basic OBS video info
 * @param inf pointer to video info structure
 * @return true on success
 */
EXPORT bool wsc_get_video_info(wsc_connection_t *conn, wsc_video_info_t *inf);

/**
 * @brief Set the sending of the Heartbeat event
 * @return true on success
 */
EXPORT bool wsc_set_heartbeat(wsc_connection_t *conn, bool state);

/**
 * @brief Set the filename formatting string
 * @return true on success
 */
EXPORT bool wsc_set_filename_format(wsc_connection_t *conn, const char *format);

/**
 * @brief Get the filename formatting string
 * @param format A pointer to an empty string, free it with wsc_free
 * @return true on success
 */
EXPORT bool wsc_get_filename_format(wsc_connection_t *conn, char **format);

/**
 * @brief Get current obs statistics
 * @param stats A pointer to a statistics struct
 * @return true on success
 */
EXPORT bool wsc_get_stats(wsc_connection_t *conn, wsc_stats_t *stats);

/**
 * @brief Broadcast custom message to all connected WebSocket clients
 * @param realm Identifier for this client
 * @param data Custom message data json string
 * @return true on success
 */
EXPORT bool wsc_broadcast_message(wsc_connection_t *conn, const char *realm, const char *data);

/**
 * @brief Sets the version and magic number, all other
 * parameters are set to zero
 */
EXPORT void wsc_prepare_geomery(wsc_geometry_t *geo);

/**
 * @brief Open a projector window or create a projector on a monitor
 *        (currently unreleased)
 * @param t The type of projector to open
 * @param monitor The monitor to open the projector on
 * @param geo The geometry of the projector, use wsc_prepare_geometry to
 *        configure it (optional)
 * @param src_name The name of the source or scene to be displayed (optional)
 * @return true on success
 */
EXPORT bool wsc_open_projector2(wsc_connection_t *conn, enum wsc_projector_type t, int32_t monitor,
                                const wsc_geometry_t *geo, const char *name);
/**
 * @brief Just opens a projector
 * @param conn
 * @return true on success
 */
EXPORT bool wsc_open_projector(wsc_connection_t *conn);
