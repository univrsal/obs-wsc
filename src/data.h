/******************************************************************************
    Copyright (C) 2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include "external/c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * OBS data settings storage (tweaked for obs-wsc)
 *
 *   This is used for retrieving or setting the data settings for things such
 * as sources, encoders, etc.  This is designed for JSON serialization.
 */

struct wsc_data;
struct wsc_data_item;
struct wsc_data_array;
typedef struct wsc_data wsc_data_t;
typedef struct wsc_data_item wsc_data_item_t;
typedef struct wsc_data_array wsc_data_array_t;

enum wsc_data_type {
    WSC_DATA_NULL,
    WSC_DATA_STRING,
    WSC_DATA_NUMBER,
    WSC_DATA_BOOLEAN,
    WSC_DATA_OBJECT,
    WSC_DATA_ARRAY
};

enum wsc_data_number_type { WSC_DATA_NUM_INVALID, WSC_DATA_NUM_INT, WSC_DATA_NUM_DOUBLE };

/* ------------------------------------------------------------------------- */
/* Main usage functions */

EXPORT wsc_data_t *wsc_data_create();
EXPORT wsc_data_t *wsc_data_create_from_json(const char *json_string);
EXPORT wsc_data_t *wsc_data_create_from_json_file(const char *json_file);
EXPORT wsc_data_t *wsc_data_create_from_json_file_safe(const char *json_file, const char *backup_ext);
EXPORT void wsc_data_addref(wsc_data_t *data);
EXPORT void wsc_data_release(wsc_data_t *data);

EXPORT const char *wsc_data_get_json(wsc_data_t *data);
//EXPORT bool wsc_data_save_json(wsc_data_t *data, const char *file);
//EXPORT bool wsc_data_save_json_safe(wsc_data_t *data, const char *file,
//                    const char *temp_ext,
//                    const char *backup_ext);

EXPORT void wsc_data_apply(wsc_data_t *target, wsc_data_t *apply_data);

EXPORT void wsc_data_erase(wsc_data_t *data, const char *name);
EXPORT void wsc_data_clear(wsc_data_t *data);

/* Set functions */
EXPORT void wsc_data_set_string(wsc_data_t *data, const char *name, const char *val);
EXPORT void wsc_data_set_int(wsc_data_t *data, const char *name, long long val);
EXPORT void wsc_data_set_double(wsc_data_t *data, const char *name, double val);
EXPORT void wsc_data_set_bool(wsc_data_t *data, const char *name, bool val);
EXPORT void wsc_data_set_obj(wsc_data_t *data, const char *name, wsc_data_t *obj);
EXPORT void wsc_data_set_array(wsc_data_t *data, const char *name, wsc_data_array_t *array);

/*
 * Default value functions.
 */
EXPORT void wsc_data_set_default_string(wsc_data_t *data, const char *name, const char *val);
EXPORT void wsc_data_set_default_int(wsc_data_t *data, const char *name, long long val);
EXPORT void wsc_data_set_default_double(wsc_data_t *data, const char *name, double val);
EXPORT void wsc_data_set_default_bool(wsc_data_t *data, const char *name, bool val);
EXPORT void wsc_data_set_default_obj(wsc_data_t *data, const char *name, wsc_data_t *obj);

/*
 * Application overrides
 * Use these to communicate the actual values of settings in case the user
 * settings aren't appropriate
 */
EXPORT void wsc_data_set_autoselect_string(wsc_data_t *data, const char *name, const char *val);
EXPORT void wsc_data_set_autoselect_int(wsc_data_t *data, const char *name, long long val);
EXPORT void wsc_data_set_autoselect_double(wsc_data_t *data, const char *name, double val);
EXPORT void wsc_data_set_autoselect_bool(wsc_data_t *data, const char *name, bool val);
EXPORT void wsc_data_set_autoselect_obj(wsc_data_t *data, const char *name, wsc_data_t *obj);

/*
 * Get functions
 */
EXPORT const char *wsc_data_get_string(wsc_data_t *data, const char *name);
EXPORT long long wsc_data_get_int(wsc_data_t *data, const char *name);
EXPORT double wsc_data_get_double(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_get_bool(wsc_data_t *data, const char *name);
EXPORT wsc_data_t *wsc_data_get_obj(wsc_data_t *data, const char *name);
EXPORT wsc_data_array_t *wsc_data_get_array(wsc_data_t *data, const char *name);

EXPORT const char *wsc_data_get_default_string(wsc_data_t *data, const char *name);
EXPORT long long wsc_data_get_default_int(wsc_data_t *data, const char *name);
EXPORT double wsc_data_get_default_double(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_get_default_bool(wsc_data_t *data, const char *name);
EXPORT wsc_data_t *wsc_data_get_default_obj(wsc_data_t *data, const char *name);
EXPORT wsc_data_array_t *wsc_data_get_default_array(wsc_data_t *data, const char *name);

EXPORT const char *wsc_data_get_autoselect_string(wsc_data_t *data, const char *name);
EXPORT long long wsc_data_get_autoselect_int(wsc_data_t *data, const char *name);
EXPORT double wsc_data_get_autoselect_double(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_get_autoselect_bool(wsc_data_t *data, const char *name);
EXPORT wsc_data_t *wsc_data_get_autoselect_obj(wsc_data_t *data, const char *name);
EXPORT wsc_data_array_t *wsc_data_get_autoselect_array(wsc_data_t *data, const char *name);

/* Array functions */
EXPORT wsc_data_array_t *wsc_data_array_create();
EXPORT void wsc_data_array_addref(wsc_data_array_t *array);
EXPORT void wsc_data_array_release(wsc_data_array_t *array);

EXPORT size_t wsc_data_array_count(wsc_data_array_t *array);
EXPORT wsc_data_t *wsc_data_array_item(wsc_data_array_t *array, size_t idx);
EXPORT size_t wsc_data_array_push_back(wsc_data_array_t *array, wsc_data_t *obj);
EXPORT void wsc_data_array_insert(wsc_data_array_t *array, size_t idx, wsc_data_t *obj);
EXPORT void wsc_data_array_push_back_array(wsc_data_array_t *array, wsc_data_array_t *array2);
EXPORT void wsc_data_array_erase(wsc_data_array_t *array, size_t idx);

/* ------------------------------------------------------------------------- */
/* Item status inspection */

EXPORT bool wsc_data_has_user_value(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_has_default_value(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_has_autoselect_value(wsc_data_t *data, const char *name);

EXPORT bool wsc_data_item_has_user_value(wsc_data_item_t *data);
EXPORT bool wsc_data_item_has_default_value(wsc_data_item_t *data);
EXPORT bool wsc_data_item_has_autoselect_value(wsc_data_item_t *data);

/* ------------------------------------------------------------------------- */
/* Clearing data values */

EXPORT void wsc_data_unset_user_value(wsc_data_t *data, const char *name);
EXPORT void wsc_data_unset_default_value(wsc_data_t *data, const char *name);
EXPORT void wsc_data_unset_autoselect_value(wsc_data_t *data, const char *name);

EXPORT void wsc_data_item_unset_user_value(wsc_data_item_t *data);
EXPORT void wsc_data_item_unset_default_value(wsc_data_item_t *data);
EXPORT void wsc_data_item_unset_autoselect_value(wsc_data_item_t *data);

/* ------------------------------------------------------------------------- */
/* Item iteration */

EXPORT wsc_data_item_t *wsc_data_first(wsc_data_t *data);
EXPORT wsc_data_item_t *wsc_data_item_byname(wsc_data_t *data, const char *name);
EXPORT bool wsc_data_item_next(wsc_data_item_t **item);
EXPORT void wsc_data_item_release(wsc_data_item_t **item);
EXPORT void wsc_data_item_remove(wsc_data_item_t **item);

/* Gets Item type */
EXPORT enum wsc_data_type wsc_data_item_gettype(wsc_data_item_t *item);
EXPORT enum wsc_data_number_type wsc_data_item_numtype(wsc_data_item_t *item);
EXPORT const char *wsc_data_item_get_name(wsc_data_item_t *item);

/* Item set functions */
EXPORT void wsc_data_item_set_string(wsc_data_item_t **item, const char *val);
EXPORT void wsc_data_item_set_int(wsc_data_item_t **item, long long val);
EXPORT void wsc_data_item_set_double(wsc_data_item_t **item, double val);
EXPORT void wsc_data_item_set_bool(wsc_data_item_t **item, bool val);
EXPORT void wsc_data_item_set_obj(wsc_data_item_t **item, wsc_data_t *val);
EXPORT void wsc_data_item_set_array(wsc_data_item_t **item, wsc_data_array_t *val);

EXPORT void wsc_data_item_set_default_string(wsc_data_item_t **item, const char *val);
EXPORT void wsc_data_item_set_default_int(wsc_data_item_t **item, long long val);
EXPORT void wsc_data_item_set_default_double(wsc_data_item_t **item, double val);
EXPORT void wsc_data_item_set_default_bool(wsc_data_item_t **item, bool val);
EXPORT void wsc_data_item_set_default_obj(wsc_data_item_t **item, wsc_data_t *val);
EXPORT void wsc_data_item_set_default_array(wsc_data_item_t **item, wsc_data_array_t *val);

EXPORT void wsc_data_item_set_autoselect_string(wsc_data_item_t **item, const char *val);
EXPORT void wsc_data_item_set_autoselect_int(wsc_data_item_t **item, long long val);
EXPORT void wsc_data_item_set_autoselect_double(wsc_data_item_t **item, double val);
EXPORT void wsc_data_item_set_autoselect_bool(wsc_data_item_t **item, bool val);
EXPORT void wsc_data_item_set_autoselect_obj(wsc_data_item_t **item, wsc_data_t *val);
EXPORT void wsc_data_item_set_autoselect_array(wsc_data_item_t **item, wsc_data_array_t *val);

/* Item get functions */
EXPORT const char *wsc_data_item_get_string(wsc_data_item_t *item);
EXPORT long long wsc_data_item_get_int(wsc_data_item_t *item);
EXPORT double wsc_data_item_get_double(wsc_data_item_t *item);
EXPORT bool wsc_data_item_get_bool(wsc_data_item_t *item);
EXPORT wsc_data_t *wsc_data_item_get_obj(wsc_data_item_t *item);
EXPORT wsc_data_array_t *wsc_data_item_get_array(wsc_data_item_t *item);

EXPORT const char *wsc_data_item_get_default_string(wsc_data_item_t *item);
EXPORT long long wsc_data_item_get_default_int(wsc_data_item_t *item);
EXPORT double wsc_data_item_get_default_double(wsc_data_item_t *item);
EXPORT bool wsc_data_item_get_default_bool(wsc_data_item_t *item);
EXPORT wsc_data_t *wsc_data_item_get_default_obj(wsc_data_item_t *item);
EXPORT wsc_data_array_t *wsc_data_item_get_default_array(wsc_data_item_t *item);

EXPORT const char *wsc_data_item_get_autoselect_string(wsc_data_item_t *item);
EXPORT long long wsc_data_item_get_autoselect_int(wsc_data_item_t *item);
EXPORT double wsc_data_item_get_autoselect_double(wsc_data_item_t *item);
EXPORT bool wsc_data_item_get_autoselect_bool(wsc_data_item_t *item);
EXPORT wsc_data_t *wsc_data_item_get_autoselect_obj(wsc_data_item_t *item);
EXPORT wsc_data_array_t *wsc_data_item_get_autoselect_array(wsc_data_item_t *item);

/* ------------------------------------------------------------------------- */
/* Helper functions for certain structures */
//EXPORT void wsc_data_set_vec2(wsc_data_t *data, const char *name,
//                  const struct vec2 *val);
//EXPORT void wsc_data_set_vec3(wsc_data_t *data, const char *name,
//                  const struct vec3 *val);
//EXPORT void wsc_data_set_vec4(wsc_data_t *data, const char *name,
//                  const struct vec4 *val);
//EXPORT void wsc_data_set_quat(wsc_data_t *data, const char *name,
//                  const struct quat *val);

//EXPORT void wsc_data_set_default_vec2(wsc_data_t *data, const char *name,
//                      const struct vec2 *val);
//EXPORT void wsc_data_set_default_vec3(wsc_data_t *data, const char *name,
//                      const struct vec3 *val);
//EXPORT void wsc_data_set_default_vec4(wsc_data_t *data, const char *name,
//                      const struct vec4 *val);
//EXPORT void wsc_data_set_default_quat(wsc_data_t *data, const char *name,
//                      const struct quat *val);

//EXPORT void wsc_data_set_autoselect_vec2(wsc_data_t *data, const char *name,
//                     const struct vec2 *val);
//EXPORT void wsc_data_set_autoselect_vec3(wsc_data_t *data, const char *name,
//                     const struct vec3 *val);
//EXPORT void wsc_data_set_autoselect_vec4(wsc_data_t *data, const char *name,
//                     const struct vec4 *val);
//EXPORT void wsc_data_set_autoselect_quat(wsc_data_t *data, const char *name,
//                     const struct quat *val);

//EXPORT void wsc_data_get_vec2(wsc_data_t *data, const char *name,
//                  struct vec2 *val);
//EXPORT void wsc_data_get_vec3(wsc_data_t *data, const char *name,
//                  struct vec3 *val);
//EXPORT void wsc_data_get_vec4(wsc_data_t *data, const char *name,
//                  struct vec4 *val);
//EXPORT void wsc_data_get_quat(wsc_data_t *data, const char *name,
//                  struct quat *val);

//EXPORT void wsc_data_get_default_vec2(wsc_data_t *data, const char *name,
//                      struct vec2 *val);
//EXPORT void wsc_data_get_default_vec3(wsc_data_t *data, const char *name,
//                      struct vec3 *val);
//EXPORT void wsc_data_get_default_vec4(wsc_data_t *data, const char *name,
//                      struct vec4 *val);
//EXPORT void wsc_data_get_default_quat(wsc_data_t *data, const char *name,
//                      struct quat *val);

//EXPORT void wsc_data_get_autoselect_vec2(wsc_data_t *data, const char *name,
//                     struct vec2 *val);
//EXPORT void wsc_data_get_autoselect_vec3(wsc_data_t *data, const char *name,
//                     struct vec3 *val);
//EXPORT void wsc_data_get_autoselect_vec4(wsc_data_t *data, const char *name,
//                     struct vec4 *val);
//EXPORT void wsc_data_get_autoselect_quat(wsc_data_t *data, const char *name,
//                     struct quat *val);

/* ------------------------------------------------------------------------- */
/* Helper functions for media_frames_per_second/wsc_PROPERTY_FRAME_RATE */
//EXPORT void wsc_data_set_frames_per_second(wsc_data_t *data, const char *name,
//                       struct media_frames_per_second fps,
//                       const char *option);
//EXPORT void
//wsc_data_set_default_frames_per_second(wsc_data_t *data, const char *name,
//                       struct media_frames_per_second fps,
//                       const char *option);
//EXPORT void
//wsc_data_set_autoselect_frames_per_second(wsc_data_t *data, const char *name,
//                      struct media_frames_per_second fps,
//                      const char *option);

//EXPORT bool wsc_data_get_frames_per_second(wsc_data_t *data, const char *name,
//                       struct media_frames_per_second *fps,
//                       const char **option);
//EXPORT bool
//wsc_data_get_default_frames_per_second(wsc_data_t *data, const char *name,
//                       struct media_frames_per_second *fps,
//                       const char **option);
//EXPORT bool
//wsc_data_get_autoselect_frames_per_second(wsc_data_t *data, const char *name,
//                      struct media_frames_per_second *fps,
//                      const char **option);

//EXPORT void
//wsc_data_item_set_frames_per_second(wsc_data_item_t **item,
//                    struct media_frames_per_second fps,
//                    const char *option);
//EXPORT void
//wsc_data_item_set_default_frames_per_second(wsc_data_item_t **item,
//                        struct media_frames_per_second fps,
//                        const char *option);
//EXPORT void wsc_data_item_set_autoselect_frames_per_second(
//    wsc_data_item_t **item, struct media_frames_per_second fps,
//    const char *option);

//EXPORT bool
//wsc_data_item_get_frames_per_second(wsc_data_item_t *item,
//                    struct media_frames_per_second *fps,
//                    const char **option);
//EXPORT bool
//wsc_data_item_get_default_frames_per_second(wsc_data_item_t *item,
//                        struct media_frames_per_second *fps,
//                        const char **option);
//EXPORT bool wsc_data_item_get_autoselect_frames_per_second(
//    wsc_data_item_t *item, struct media_frames_per_second *fps,
//    const char **option);

/* ------------------------------------------------------------------------- */
/* OBS-specific functions */

static inline wsc_data_t *wsc_data_newref(wsc_data_t *data)
{
    if (data)
        wsc_data_addref(data);
    else
        data = wsc_data_create();

    return data;
}

#ifdef __cplusplus
}
#endif
