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
 * @brief Toggle the replay buffer on/off
 * @return true on success
 */
EXPORT bool wsc_toggle_replay_buffer(wsc_connection_t *conn);

/**
 * @brief Start recording the replay buffer, will error if it's already sarted
 *        or when the "Save Replay Buffer" hotkey is not set in OBS
 * @return true on success
 */
EXPORT bool wsc_start_replay_buffer(wsc_connection_t *conn);

/**
 * @brief Stop recording into the replay buffer, will error if the replay buffer
 *        is not active
 * @return true on success
 */
EXPORT bool wsc_stop_replay_buffer(wsc_connection_t *conn);

/**
 * @brief Flush and save the contents of the replay buffer to disk, will error
 *        if the replay buffer is not active
 * @return true on success
 */
EXPORT bool wsc_save_replay_buffer(wsc_connection_t *conn);
