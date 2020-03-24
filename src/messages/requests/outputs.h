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
 * @brief Free a list of outputs populated by wsc_list_outputs
 */
EXPORT void wsc_free_outputs(wsc_ouputs_t *o);

/**
 * @brief List existing outputs
 * @param o Pointer to outputs structure, free with wsc_free_outputs
 * @return true on success
 */
EXPORT bool wsc_list_outputs(wsc_connection_t *conn, wsc_ouputs_t *o);

/**
 * @brief Get information about a single output
 * @param name the name of the output to query
 * @param o Pointer to an output structure, free with wsc_free_output
 * @return true on success
 */
EXPORT bool wsc_get_output_info(wsc_connection_t *conn, const char *name, wsc_output_t *o);

/**
 * @brief Start an output
 * @return true on success
 */
EXPORT bool wsc_start_output(wsc_connection_t *conn, const char *name);

/**
 * @brief Stop an output
 * @param name The name of the output
 * @param force Force stop
 * @return true on success
 */
EXPORT bool wsc_stop_output(wsc_connection_t *conn, const char *name, bool force);
