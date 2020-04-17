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
 * @brief Toggle stream on or off
 * @return true on success
 */
EXPORT bool wsc_toggle_streaming(wsc_connection_t *conn);

/**
 * @brief Start streaming, will error if stream is already active
 * @param s Stream settings (optional)
 * @return true on success
 */
EXPORT bool wsc_start_streaming(wsc_connection_t *conn, const wsc_stream_settings_t *s);

/**
 * @brief Stop streaming, will error if stream is not active
 * @return true on success
 */
EXPORT bool wsc_stop_streaming(wsc_connection_t *conn);

/**
 * @brief Get current streaming and recording status
 * @param s Pointer to empty status struct, free with wsc_free_streaming_status
 * @return true on success
 */
EXPORT bool wsc_get_streaming_status(wsc_connection_t *conn, wsc_stream_status_t *s);

/**
 * @brief Free a status struct allocated by wsc_get_streaming_status
 */
EXPORT void wsc_free_streaming_status(wsc_stream_status_t *s);

/**
 * @brief Sets one or more attributes of the current streaming server settings.
 * @param s The settings struct
 * @return true on success
 */
EXPORT bool wsc_set_stream_settings(wsc_connection_t *conn, const wsc_stream_settings_t *s);

/**
 * @brief Get the current streaming server settings
 * @param s Pointer to empty stream settings struct, free with wsc_free_stream_settings
 * @return true on success
 */
EXPORT bool wsc_get_stream_settings(wsc_connection_t *conn, wsc_stream_settings_t *s);

/**
 * @brief Free a stream settings struct allocated by wsc_get_stream_settings
 * @param s
 */
EXPORT void wsc_free_stream_settings(wsc_stream_settings_t *s);

/**
 * @brief Save the current streaming sever settings to disk
 * @return true on success
 */
EXPORT bool wsc_save_stream_settings(wsc_connection_t *conn);

/**
 * @brief Send the provided string as embedded CEA-608 caption data
 * @param captions Caption text
 * @return true on success
 */
EXPORT bool wsc_send_captions(wsc_connection_t *conn, const char *captions);
