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

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <jni.h>

class TmpFile {
private:
  std::string m_path;
  int m_descriptor = -1;
  FILE *m_handle = nullptr;

public:
  explicit TmpFile(std::string_view path_template) : m_path(path_template) {
    m_descriptor = mkstemp(&m_path[0]);
    if (-1 != m_descriptor) {
      m_handle = fdopen(m_descriptor, "w+b");
    }
  }

  TmpFile(TmpFile && source) noexcept {
    m_path = std::move(source.m_path);

    m_descriptor = source.m_descriptor;
    source.m_descriptor = -1;

    m_handle = source.m_handle;
    source.m_handle = nullptr;
  }

  ~TmpFile() {
    if (-1 != m_descriptor) {
      if (!m_path.empty()) {
        std::remove(m_path.c_str());
      }
    }
  }

  std::string_view get_path() const {
    return m_path;
  }

  FILE *get_handle() const {
    return m_handle;
  }
};

class TmpFileManager {
private:
  std::string m_tmpfile_path_template = "/data/local/tmp/tmpfile-XXXXXX";
  std::unordered_map<std::string, TmpFile> m_created_tmpfiles;

  TmpFileManager() = default;

public:
  static TmpFileManager &getInstance() {
    static TmpFileManager s_fm;
    return s_fm;
  }

  void set_tmpfile_dir(const char *tmpfile_dir) {
    m_tmpfile_path_template = std::string(tmpfile_dir) + "/tmpfile-XXXXXX";
  }

  FILE *create_tmpfile() {
    TmpFile t(m_tmpfile_path_template);
    auto path = t.get_path();
    auto newly_created_file = m_created_tmpfiles.insert(std::make_pair(path, std::move(t)));
    if (!newly_created_file.second) {
      return nullptr;
    }
    return newly_created_file.first->second.get_handle();
  }

  void tmpfile_closed(const std::string & full_closed_file_path) {
    m_created_tmpfiles.erase(full_closed_file_path);
  }
};

extern "C" {

JNIEXPORT void JNICALL
Java_com_viliussutkus89_tmpfile_Tmpfile_set_1tmpfile_1dir(JNIEnv *env, jclass, jstring tmpfile_dir) {
  const char *tmpfile_dir_c = env->GetStringUTFChars(tmpfile_dir, nullptr);
  TmpFileManager::getInstance().set_tmpfile_dir(tmpfile_dir_c);
  env->ReleaseStringUTFChars(tmpfile_dir, tmpfile_dir_c);
}

JNIEXPORT void JNICALL
Java_com_viliussutkus89_tmpfile_Tmpfile_on_1file_1closed(JNIEnv *env, jclass,
                                                         jstring file_path) {
  const char *file_path_c = env->GetStringUTFChars(file_path, nullptr);
  TmpFileManager::getInstance().tmpfile_closed(file_path_c);
  env->ReleaseStringUTFChars(file_path, file_path_c);
}

JNIEXPORT FILE *tmpfile() {
  return TmpFileManager::getInstance().create_tmpfile();
}

}
