/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "c99defs.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

void os_breakpoint(void);

void os_sleep_ms(uint32_t duratiion);

uint64_t os_gettime_ns(void);

size_t os_mbs_to_wcs(const char *str, size_t str_len, wchar_t *dst, size_t dst_size);
size_t os_utf8_to_wcs(const char *str, size_t len, wchar_t *dst, size_t dst_size);
size_t os_wcs_to_mbs(const wchar_t *str, size_t len, char *dst, size_t dst_size);
size_t os_wcs_to_utf8(const wchar_t *str, size_t len, char *dst, size_t dst_size);

size_t os_mbs_to_wcs_ptr(const char *str, size_t len, wchar_t **pstr);
size_t os_utf8_to_wcs_ptr(const char *str, size_t len, wchar_t **pstr);
size_t os_wcs_to_mbs_ptr(const wchar_t *str, size_t len, char **pstr);
size_t os_wcs_to_utf8_ptr(const wchar_t *str, size_t len, char **pstr);

size_t os_utf8_to_mbs_ptr(const char *str, size_t len, char **pstr);
size_t os_mbs_to_utf8_ptr(const char *str, size_t len, char **pstr);
