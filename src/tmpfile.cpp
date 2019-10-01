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
#include <unordered_set>

#include "tmpfile_export.h"

class TmpFile {
private:
  std::string m_path;
  int m_descriptor = -1;
  FILE *m_handle = nullptr;

public:
  explicit TmpFile(const std::string &path_template) : m_path(path_template) {
    m_descriptor = mkstemp(&m_path[0]);
    if (-1 != m_descriptor) {
      m_handle = fdopen(m_descriptor, "w+b");
    }
  }

  ~TmpFile() {
    if (-1 != m_descriptor) {
      close(m_descriptor);
      if (!m_path.empty()) {
        std::remove(m_path.c_str());
      }
    }
  }

  FILE *get_handle() const {
    return m_handle;
  }

  bool operator==(const TmpFile &af) const {
    return 0 == m_path.compare(af.m_path);
  }

  class Hash {
  public:
    size_t operator()(const TmpFile &af) const {
      return std::hash<std::string>{}(af.m_path);
    }
  };
};

class TmpFileManager {
private:
  std::string m_tmpfile_path_template = "/data/local/tmp/tmpfile-XXXXXX";
  std::unordered_set<TmpFile, TmpFile::Hash> m_created_tmpfiles;

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
    auto newly_create_file = m_created_tmpfiles.emplace(m_tmpfile_path_template);
    if (!newly_create_file.second) {
      return nullptr;
    }
    return newly_create_file.first->get_handle();
  }

  void cleanup_tmpfiles() {
    m_created_tmpfiles.clear();
  }
};

namespace Tmpfile {
  TMPFILE_EXPORT void set_tmpfile_dir(const char *tmpfile_dir) {
    TmpFileManager::getInstance().set_tmpfile_dir(tmpfile_dir);
  }

  TMPFILE_EXPORT void cleanup_tmpfiles() {
    return TmpFileManager::getInstance().cleanup_tmpfiles();
  }
}

TMPFILE_EXPORT FILE *tmpfile() {
  return TmpFileManager::getInstance().create_tmpfile();
}
