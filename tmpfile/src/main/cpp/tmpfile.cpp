/*
 * tmpfile.cpp
 *
 * tmpfile function overload for broken implementations.
 *
 * Copyright (C) 2019 Vilius Sutkus'89
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

#include <cstdio>
#include <unistd.h>
#include <string>
#include <jni.h>

static std::string s_tmpfile_path_template("/data/local/tmp/tmpfile-XXXXXX");

extern "C" {

JNIEXPORT void JNICALL
Java_com_viliussutkus89_tmpfile_Tmpfile_set_1tmpfile_1dir(JNIEnv *env, jclass, jstring tmpfile_dir) {
  const char *tmpfile_dir_c = env->GetStringUTFChars(tmpfile_dir, nullptr);
  s_tmpfile_path_template = std::string(tmpfile_dir_c) + "/tmpfile-XXXXXX";
  env->ReleaseStringUTFChars(tmpfile_dir, tmpfile_dir_c);
}

JNIEXPORT FILE *tmpfile() {
  FILE * handle = nullptr;
  std::string path = s_tmpfile_path_template;
  int descriptor = mkstemp(&path[0]);
  if (-1 != descriptor) {
    handle = fdopen(descriptor, "w+b");
    if (nullptr == handle) {
      close(descriptor);
    }

    // File already open,
    // can be unbound from the file system
    std::remove(path.c_str());
  }
  return handle;
}

}
