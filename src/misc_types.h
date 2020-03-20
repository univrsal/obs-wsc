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
#include "external/c99defs.h"
#include <stdarg.h>

#define STR_LEN 33
#define LONG_STR_LEN 2048

#define obs_wsc_auth_init()                                                       \
    {                                                                             \
        .salt = NULL, .required = false, .challenge = NULL, .auth_response = NULL \
    }

#define obs_wsc_video_info_init()                                                                        \
    {                                                                                                    \
        .base_width = 0, .base_height = 0, .output_width = 0, .output_height = 0, .scale_type[0] = '\0', \
        .video_format[0] = '\0', .color_space[0] = '\0', .fps = 0                                        \
    }

#define obs_wsc_version_info_init()                                                                   \
    {                                                                                                 \
        .obs_version[0] = '\0', .supported_image_formats[0] = '\0', .obs_websocket_version[0] = '\0', \
        .available_requests[0] = '\0', .api_version = 0                                               \
    }

typedef void (*log_handler_t)(int lvl, const char *msg, va_list args, void *p);

typedef struct obs_wsc_auth_data_s {
    char *challenge;
    char *salt;
    char *auth_response;
    bool required;
} obs_wsc_auth_data_t;

struct base_allocator {
    void *(*malloc)(size_t);
    void *(*realloc)(void *, size_t);
    void (*free)(void *);
};

/* Request data types */

typedef struct obs_wsc_video_info_s {
    int32_t base_width, base_height, output_width, output_height;
    char scale_type[STR_LEN], video_format[STR_LEN], color_space[STR_LEN], color_range[STR_LEN];
    double fps;
} obs_wsc_video_info_t;

typedef struct obs_wsc_version_info_s {
    double api_version;
    char obs_version[STR_LEN], obs_websocket_version[STR_LEN], available_requests[LONG_STR_LEN],
        supported_image_formats[STR_LEN];
} obs_wsc_version_info_t;
