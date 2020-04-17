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
 * @brief Gets the scene specific properties of the specified source item
 * @param i A pointer to an empty scene item structure
 * @param item The name of the scene item
 * @param id The scene item id (Optional, -1 if not used)
 * @param scene The scene the item is in, if NULL defaults to the current scene
 * @return true on success
 */
EXPORT bool wsc_get_scene_item_properties(wsc_connection_t *conn, wsc_scene_item_properties_t *i, const char *item,
                                          int id, const char *scene);

/**
 * @brief Sets the scene specific properties of the specified source item
 * @param i A pointer to a valid scene item structure containing at least
 *        the scene item's name
 * @param item The name of the scene item
 * @param id The scene item id (Optional, -1 if not used)
 * @param scene The scene the item is in, if NULL defaults to the current scene
 * @return true on success
 */
EXPORT bool wsc_set_scene_item_properties(wsc_connection_t *conn, const wsc_scene_item_properties_t *i,
                                          const char *scene);
