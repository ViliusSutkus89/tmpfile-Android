/*
 * tmpfile_jni.c
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

#include <stdio.h>
#include <string.h>
#include <jni.h>

#include "tmpfile.h"

void native_set_tmpfile_dir(JNIEnv *env, __attribute__((unused)) jclass clazz, jstring tmpfile_dir) {
    const char *tmpfile_dir_c = (*env)->GetStringUTFChars(env, tmpfile_dir, JNI_FALSE);
    const char *tmpfile_dir_duplicate = strdup(tmpfile_dir_c);
    (*env)->ReleaseStringUTFChars(env, tmpfile_dir, tmpfile_dir_c);
    LOGE("tmpfile::native_set_tmpfile_dir('%s')\n", tmpfile_dir_duplicate)
    set_tmpfile_directory(tmpfile_dir_duplicate);
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
      {"set_tmpfile_dir",      "(Ljava/lang/String;)V", (void *) native_set_tmpfile_dir},
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
