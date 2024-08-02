/*
 * tmpfile.c
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

#include <unistd.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <jni.h>

#include "strcat_path_array.h"
#include "tmpfile.h"

#define c_filename_template "tmpfile-XXXXXX"
#define c_default_directory "/data/local/tmp"

const char *s_tmpfile_directory = NULL;
atomic_bool s_tmpfile_directory_is_set = false;

// tmpfile-Android provides a workaround for both Android (JNI) apps and standalone executables
bool s_is_jni = false;

// tmpfile_directory can be either (in this specific order):
// 1: set by JNI call to set_tmpfile_dir
// 2: obtained from environment variable TMPDIR
// 3: /data/local/tmp
const char *choose_tmpfile_directory(void) {
  const char *tmpfile_directory;
  if (atomic_load_explicit(&s_tmpfile_directory_is_set, memory_order_acquire)) {
    tmpfile_directory = s_tmpfile_directory;
  } else {
    if (s_is_jni) {
      LOGE("Tmpfile is not initialized! Call Tmpfile::init() before trying to call POSIX tmpfile().\n")
    }
    tmpfile_directory = getenv("TMPDIR");
    if (NULL == tmpfile_directory || '\0' == tmpfile_directory[0]) {
      tmpfile_directory = c_default_directory;
    }
  }
  return tmpfile_directory;
}

void set_tmpfile_directory(const char *tmpfile_dir) {
  const char *old_s_tmpfile_path_template = s_tmpfile_directory;
  s_tmpfile_directory = tmpfile_dir;
  atomic_store_explicit(&s_tmpfile_directory_is_set, true, memory_order_release);
  free((void *) old_s_tmpfile_path_template);
}

JNIEXPORT FILE *tmpfile(void) {
  char *tmpfile_path = strcat_path_array((const char *[]) { choose_tmpfile_directory(),
                                                            "/" c_filename_template, NULL});
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
