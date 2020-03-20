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

#define MSG_ID_LENGTH 17
#include "opaque.h"
#include "external/darray.h"

char *util_random_id(const struct darray *ids);

static inline void util_strncpy(char *dest, size_t len, const char *src, size_t max)
{
#ifdef WIN32
    strncpy_s(dest, len, src, max);
#else
    strncpy(dest, src, max);
#endif
}
