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
 * @brief Toggle recording on or off
 * @return true on success
 */
EXPORT bool wsc_toggle_recording(wsc_connection_t *conn);

/**
 * @brief Starts recording, will error if already recording
 * @return true on success
 */
EXPORT bool wsc_start_recording(wsc_connection_t *conn);

/**
 * @brief Stop recording, will error if not recording
 * @return true on success
 */
EXPORT bool wsc_stop_recording(wsc_connection_t *conn);

/**
 * @brief Pause recording, will error if not recording or already paused
 * @return true on success
 */
EXPORT bool wsc_pause_recording(wsc_connection_t *conn);

/**
 * @brief Resume recording, will error if not recording or not paused
 * @return true on success
 */
EXPORT bool wsc_resume_recording(wsc_connection_t *conn);

/**
 * @brief Set the recording output folder
 * @param folder The target folder
 * @return true on succcess
 */
EXPORT bool wsc_set_recording_folder(wsc_connection_t *conn, const char *folder);

/**
 * @brief Get the recording output folder
 * @param folder Pointer to empty string, free with wsc_free
 * @return true on succcess
 */
EXPORT bool wsc_get_recording_folder(wsc_connection_t *conn, char **folder);
