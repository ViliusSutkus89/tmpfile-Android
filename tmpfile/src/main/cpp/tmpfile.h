/*
 * tmpfile.h
 *
 * tmpfile function overload for broken implementations.
 *
 * Copyright (C) 2019 - 2021, 2024 Vilius Sutkus'89
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <android/log.h>

const char *choose_tmpfile_directory(void);
void set_tmpfile_directory(const char *tmpfile_dir);

extern bool s_is_jni;

#define LOG(LOG_LEVEL, ...) if (s_is_jni) { \
  __android_log_print(LOG_LEVEL, "tmpfile", __VA_ARGS__); \
} else {                                                          \
    printf(__VA_ARGS__);                                          \
}

#define LOGE(...) LOG(ANDROID_LOG_ERROR, __VA_ARGS__)

#ifdef NDEBUG
#define LOGD(...) {}
#else
#define LOGD(...) LOG(ANDROID_LOG_DEBUG, __VA_ARGS__)
#endif
