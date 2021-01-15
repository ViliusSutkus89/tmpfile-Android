/*
 * tmpfile.cpp
 *
 * tmpfile function overload for broken implementations.
 *
 * Copyright (C) 2019 - 2021 Vilius Sutkus'89
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <jni.h>
#include <android/log.h>
#include "strcat_path_array.h"
#include "mkdirs_for_file.h"

#define c_directory_in_cache "tmpfiles"
#define c_filename_template "tmpfile-XXXXXX"
#define c_default_directory "/data/local/tmp"

const char *s_tmpfile_directory = NULL;
atomic_bool s_tmpfile_directory_is_set = false;

#define LOGE(...) if (atomic_load_explicit(&s_tmpfile_directory_is_set, memory_order_acquire)) { \
  __android_log_print(ANDROID_LOG_ERROR, "tmpfile", __VA_ARGS__); \
} else {                                                          \
    printf(__VA_ARGS__);                                          \
}

// directory containing tmpfile can be either (in this specific order):
// 1: set by JNI call to set_cache_dir
// 2: obtained from environment variable TMPDIR
// 3: /data/local/tmp
static char *choose_tmpfile_template() {
  const char *tmpfile_directory;
  if (atomic_load_explicit(&s_tmpfile_directory_is_set, memory_order_acquire)) {
    tmpfile_directory = s_tmpfile_directory;
  } else if (NULL == (tmpfile_directory = getenv("TMPDIR")) || ('\0' == tmpfile_directory[0])) {
    tmpfile_directory = c_default_directory;
  }

  char *tmpfile_template = strcat_path_array((const char *[]) {
      tmpfile_directory,
      "/" c_filename_template,
      NULL});
  return tmpfile_template;
}


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_viliussutkus89_android_tmpfile_Tmpfile_set_1cache_1dir(JNIEnv *env,
                                                                __attribute__((unused)) jclass clazz,
                                                                jstring cache_dir) {
  const char *cache_dir_c = (*env)->GetStringUTFChars(env, cache_dir, JNI_FALSE);
  char *tmpfile_directory = strcat_path_array(
      (const char *[]) {cache_dir_c, "/" c_directory_in_cache, NULL});
  (*env)->ReleaseStringUTFChars(env, cache_dir, cache_dir_c);

  if (NULL == tmpfile_directory || !mkdirs_for_file(tmpfile_directory)) {
    LOGE("Failed to create directory: '%s'\n", tmpfile_directory);
    free(tmpfile_directory);
    tmpfile_directory = NULL;
    return;
  }

  const char *old_s_tmpfile_path_template = s_tmpfile_directory;
  s_tmpfile_directory = tmpfile_directory;
  atomic_store_explicit(&s_tmpfile_directory_is_set, true, memory_order_release);
  free((void *) old_s_tmpfile_path_template);
}

JNIEXPORT FILE *tmpfile() {
  char *tmpfile_path = choose_tmpfile_template();
  if (NULL == tmpfile_path) {
    LOGE("tmpfile failure: failed to obtain path for tmpfiles!\n")
    return NULL;
  }

  int descriptor = mkstemp(tmpfile_path);
  if (-1 == descriptor) {
    LOGE("tmpfile failure: mkstemp('%s')", tmpfile_path)
    free(tmpfile_path);
    return NULL;
  }

  FILE *handle = fdopen(descriptor, "w+b");
  // File already open, can be unbound from the file system
  unlink(tmpfile_path);
  free(tmpfile_path);

  if (NULL == handle) {
    close(descriptor);
    LOGE("tmpfile failure: failed to open created tmpfile!\n")
  }
  return handle;
}

#ifdef __cplusplus
} // extern "C"
#endif
