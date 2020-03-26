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
 * @brief Change the active scene collection
 * @param name The name of the collection
 * @return true on success
 */
EXPORT bool wsc_set_current_scene_collection(wsc_connection_t *conn, const char *name);

/**
 * @brief Get the name of the current scene collection
 * @param conn
 * @param name An empty string pointer
 * @return true on success
 */
EXPORT bool wsc_get_current_scene_collection(wsc_connection_t *conn, char **name);

/**
 * @brief Get a list of available scene collections
 * @param c An empy scene collection struct
 * @return true on success
 */
EXPORT bool wsc_list_scene_collections(wsc_connection_t *conn, wsc_scene_collections_t *c);
