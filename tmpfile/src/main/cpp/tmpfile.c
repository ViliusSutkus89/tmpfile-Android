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
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "strcat_path_array.h"
#include "mkdirs_for_file.h"

#define c_directory_in_cache "tmpfiles"
#define c_filename_template "tmpfile-XXXXXX"
#define c_default_directory "/data/local/tmp"

const char *s_tmpfile_directory = NULL;
atomic_bool s_tmpfile_directory_is_set = false;

// tmpfile-Android provides a workaround for both Android (JNI) apps and standalone executables
static bool s_is_jni = false;

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

// tmpfile_directory can be either (in this specific order):
// 1: set by JNI call to set_cache_dir
// 2: obtained from environment variable TMPDIR
// 3: /data/local/tmp
static const char *choose_tmpfile_directory() {
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

#ifdef __cplusplus
extern "C" {
#endif

void native_set_cache_dir(JNIEnv *env, __attribute__((unused)) jclass clazz, jstring cache_dir) {
  const char *cache_dir_c = (*env)->GetStringUTFChars(env, cache_dir, JNI_FALSE);
  LOGD("tmpfile::native_set_cache_dir('%s')\n", cache_dir_c)

  char *tmpfile_directory = strcat_path_array(
      (const char *[]) {cache_dir_c, "/" c_directory_in_cache, NULL});
  (*env)->ReleaseStringUTFChars(env, cache_dir, cache_dir_c);

  if (NULL == tmpfile_directory || !mkdirs_for_file(tmpfile_directory)) {
    LOGE("Failed to create directory: '%s'\n", tmpfile_directory)
    free(tmpfile_directory);
    tmpfile_directory = NULL;
    return;
  }

  const char *old_s_tmpfile_path_template = s_tmpfile_directory;
  s_tmpfile_directory = tmpfile_directory;
  atomic_store_explicit(&s_tmpfile_directory_is_set, true, memory_order_release);
  free((void *) old_s_tmpfile_path_template);
}

jstring native_get_tmpfile_dir_path(JNIEnv *env, __attribute__((unused)) jclass clazz) {
  return (*env)->NewStringUTF(env, choose_tmpfile_directory());
}

jboolean native_self_test(__attribute__((unused)) JNIEnv *env, __attribute__((unused)) jclass clazz) {
  const char *test_data = "Hello World!\n";
  size_t len = strlen(test_data);
  char read_back_buffer[len];

  memset(read_back_buffer, '\0', len);

  FILE *test_file = tmpfile();

  if (NULL != test_file) {
    fwrite(test_data, sizeof(char), len, test_file);
    rewind(test_file);
    fread(read_back_buffer, sizeof(char), len, test_file);
    fclose(test_file);
  }

  return (0 == memcmp(test_data, read_back_buffer, len)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, __attribute__((unused)) void *reserved) {
  s_is_jni = true;
  LOGD("tmpfile::JNI_OnLoad\n")

  JNIEnv *env;
  if (JNI_OK != (*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6)) {
    LOGE("Failed to get JNI env!\n")
    return JNI_ERR;
  }

  jclass ClassTmpfile = (*env)->FindClass(env, "com/viliussutkus89/android/tmpfile/Tmpfile");
  if (NULL == ClassTmpfile) {
    LOGE("Failed to get Tmpfile class!")
    return JNI_ERR;
  }

  const JNINativeMethod methods[] = {
      {"set_cache_dir",        "(Ljava/lang/String;)V", (void *) native_set_cache_dir},
      {"get_tmpfile_dir_path", "()Ljava/lang/String;",  (void *) native_get_tmpfile_dir_path},
      {"self_test",            "()Z",                   (void *) native_self_test}
  };

  int rc = (*env)->RegisterNatives(env, ClassTmpfile, methods, sizeof(methods)/sizeof(JNINativeMethod));
  if (JNI_OK != rc) {
    LOGE("Failed to register native methods!\n")
    return rc;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT FILE *tmpfile() {
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

#ifdef __cplusplus
} // extern "C"
#endif
