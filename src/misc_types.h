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
#include "data.h"
#include <stdarg.h>

#define STR_LEN 33
#define LONG_STR_LEN 2048

/* For Windows which doesn't support the {} initializer */
#define wsc_init_struct(t, n) \
    t n;                      \
    memset(&n, 0, sizeof(t))

typedef void (*wsc_log_handler_t)(int lvl, const char *msg, va_list args, void *p);

typedef struct wsc_auth_data_s {
    char *challenge;
    char *salt;
    char *auth_response;
    bool required;
} wsc_auth_data_t;

struct wsc_allocator {
    void *(*malloc)(size_t);
    void *(*realloc)(void *, size_t);
    void (*free)(void *);
};

/* Request data types */

enum wsc_projector_type {
    WSC_PROJECTOR_PREVIEW,
    WSC_PROJECTOR_SOURCE,
    WSC_PROJECTOR_SCENE,
    WSC_PROJECTOR_STUDIO_PROGRAM,
    WSC_PROJECTOR_MULTIVIEW
};

/* This is a data struct containing
 * all information in the order it
 * is saved in Qt when saving the
 * state of a widget
 */
typedef struct wsc_geometry_s {
    uint32_t magic_number;
    uint16_t version_major, version_minor;

    struct wsc_rect_s {
        int32_t x, y, w, h;
    } frame_geometry, normal_geometry;

    int32_t screen_number;
    bool maximized, fullscreen;
    int32_t screen_width;
    struct wsc_rect_s geometry;
} wsc_geometry_t;

typedef struct wsc_video_info_s {
    int32_t base_width, base_height, output_width, output_height;
    char scale_type[STR_LEN], video_format[STR_LEN], color_space[STR_LEN], color_range[STR_LEN];
    double fps;
} wsc_video_info_t;

typedef struct wsc_version_info_s {
    double api_version;
    char obs_version[STR_LEN], obs_websocket_version[STR_LEN], available_requests[LONG_STR_LEN],
        supported_image_formats[STR_LEN];
} wsc_version_info_t;

typedef struct wsc_scene_item_s {
    int32_t cx, cy, id, source_cx, source_cy, volume, x, y;
    enum wsc_align {
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

    struct wsc_scene_item_s *group_children;
    size_t num_children;
} wsc_scene_item_t;

typedef struct wsc_scene_item_transform_s {
    struct wsc_pos_s {
        int32_t x, y;
        enum wsc_align alignment;
    } position;

    struct wsc_scale_s {
        double x, y;
    } scale;

    struct wsc_crop_s {
        int32_t top, right, bottom, left;
    } crop;

    struct wsc_bounds_s {
        enum wsc_bound_type {
            WSC_BOUNDS_NONE,
            WSC_BOUNDS_STRETCH,
            WSC_BOUNDS_SCALE_INNER,
            WSC_BOUNDS_SCALE_OUTER,
            WSC_BOUNDS_SCALE_TO_WIDTH,
            WSC_BOUNDS_SCALE_TO_HEIGHT,
            WSC_BOUNDS_MAX_ONLY
        } type;

        enum wsc_align alignment;
        double cx, cy;
    } bounds;

    double width, height, rotation;
    bool visible, locked;
    char parent_group_name[STR_LEN];

    struct wsc_scene_item_transform_s *group_children;
    size_t num_children;
} wsc_scene_item_transform_t;

typedef struct wsc_scene_item_properties_s {
    char name[STR_LEN], parent_group_name[STR_LEN];
    size_t num_children;
    wsc_scene_item_transform_t *children;
    double rotation;
    struct wsc_pos_s position;
    struct wsc_scale_s scale;
    struct wsc_crop_s crop;
    struct wsc_bounds_s bounds;
    enum wsc_align alignment;
    int32_t source_width, source_height, width, height, id;
    bool visible, muted, locked;
} wsc_scene_item_properties_t;

typedef struct wsc_stats_s {
    double fps, average_frame_time, cpu_usage, memory_usage, free_disk_space;
    uint32_t render_total_frames, render_missed_frames, output_total_frames, output_skipped_frames;
} wsc_stats_t;

typedef struct wsc_output_s {
    char name[STR_LEN];
    char type[STR_LEN];
    uint32_t width, height;

    struct wsc_output_flags_s {
        uint16_t raw_value;
        bool audio, video, encoded, multi_track, service;
    } flags;

    wsc_data_t *settings;
    bool active, reconnecting;
    double congestion;

    int64_t total_frames, dropped_frames, total_bytes;
} wsc_output_t;

typedef struct wsc_outputs_s {
    size_t count;
    wsc_output_t *arr;
} wsc_ouputs_t;

typedef struct wsc_scene_s {
    char scene_name[STR_LEN];

    wsc_scene_item_t *sources;
    size_t num_sources;
} wsc_scene_t;

typedef struct wsc_scene_collections_s {
    char **names;
    size_t num_names;
} wsc_scene_collections_t;

typedef struct wsc_profiles_s {
    char **profile_names;
    size_t num_profiles;
} wsc_profiles_t;

typedef struct wsc_stream_status_s {
    bool streaming;
    bool recording;
    char *stream_timecode;
    char *rec_timecode;
} wsc_stream_status_t;

typedef struct wsc_stream_settings_s {
    struct {
        char *type;
        char *metadata;
    } stream;
    struct {
        char *server;
        char *key;
        bool use_auth;
        char *username;
        char *password;
    } settings;
} wsc_stream_settings_t;
