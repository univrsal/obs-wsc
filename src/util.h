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
#include <malloc.h>
#include <string.h>

#ifdef WINDOWS
#define DECLSPEC __declspec(dllexport)
#define LIB_CALL __cdecl
#define FORCE_INLINE __forceinline
#elif defined UNIX
#define DECLSPEC
#define LIB_CALL
#define FORCE_INLINE __attribute__((always_inline)) static __inline__
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#endif
#endif

static inline void *zalloc(size_t s)
{
    void * v = malloc(s);
    memset(v, 0, s);
    return v;
}
