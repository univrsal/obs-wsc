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

typedef struct obs_wsc_scene_item_s {
    int32_t cx, cy, id, source_cx, source_cy, volume, x, y;
    enum obs_wsc_align {
        WSC_ALIGN_INVALID = -1,
        WSC_ALIGN_CENTER,
        WSC_ALIGN_LEFT = 1 << 0,
        WSC_ALIGN_RIGHT = 1 << 1,
        WSC_ALIGN_TOP = 1 << 2,
        WSC_ALIGN_BOTTOM = 1 << 3
    } alignment;
    char name[STR_LEN];
    char type[STR_LEN];
    char parent_group_name[STR_LEN];
    bool visible, muted, locked;

    struct obs_wsc_scene_item_s *group_children;
    size_t num_children;
} obs_wsc_scene_item_t;

typedef struct obs_wsc_scene_item_transform_s {
    struct obs_wsc_pos_s {
        int32_t x, y;
        enum obs_wsc_align alignment;
    } position;

    struct obs_wsc_scale_s {
        double x, y;
    } scale;

    struct obs_wsc_crop_s {
        int32_t top, right, bottom, left;
    } crop;

    struct obs_wsc_bounds_s {
        enum obs_wsc_bound_type {
            WSC_BOUNDS_NONE,
            WSC_BOUNDS_STRETCH,
            WSC_BOUNDS_SCALE_INNER,
            WSC_BOUNDS_SCALE_OUTER,
            WSC_BOUNDS_SCALE_TO_WIDTH,
            WSC_BOUNDS_SCALE_TO_HEIGHT,
            WSC_BOUNDS_MAX_ONLY
        } type;

        enum obs_wsc_align alignment;
        double cx, cy;
    } bounds;

    double width, height, rotation;
    bool visible, locked;
    char parent_group_name[STR_LEN];

    struct obs_wsc_scene_item_transform_s *group_children;
    size_t num_children;
} obs_wsc_scene_item_transform_t;

typedef struct obs_wsc_stats_s {
    double fps, average_frame_time, cpu_usage, memory_usage, free_disk_space;
    uint32_t render_total_frames, render_missed_frames, output_total_frames, output_skipped_frames;
} obs_wsc_stats_t;

typedef struct obs_wsc_output_s {
    char name[STR_LEN];
    char type[STR_LEN];
    uint32_t widht, height;

    struct obs_wsc_output_flags_s {
        uint16_t raw_value;
        bool audio, video, encoded, multi_track, service;
    } flags;

    /* settings ? */

    bool active, reconnecting;
    double congestion;

    uint64_t total_frames, dropped_frames, total_bytes;
} obs_wsc_output_t;

typedef struct obs_wsc_scene_s {
    char scene_name[STR_LEN];

    obs_wsc_scene_item_t *sources;
    size_t num_sources;
} obs_wsc_scene_t;
