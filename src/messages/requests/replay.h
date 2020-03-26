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
 * @brief A request to start the replay buffer
 * @return true on success
 */
EXPORT bool wsc_replay_starting(wsc_connection_t *conn);

/**
 * @brief Replay buffer started successfully
 * @return true on success
 */
EXPORT bool wsc_replay_started(wsc_connection_t *conn);

/**
 * @brief A request to stop the replay buffer
 * @return true on success
 */
EXPORT bool wsc_replay_stopping(wsc_connection_t *conn);

/**
 * @brief Replay buffer stopped successfully
 * @return true on success
 */
EXPORT bool wsc_replay_stopped(wsc_connection_t *conn);
