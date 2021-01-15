/*
 * mkdir.c
 *
 * Copyright (C) 2021 Vilius Sutkus'89
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

#include <string.h>
#include <sys/stat.h>
#include "mkdirs_for_file.h"

static bool mkdirs(char * target) {
  char * pos;
  struct stat st;

  if (0 == stat(target, &st)) {
    return true;
  }

  // 1. Find last directory separator, to get parent path
  // 2. Change separator to null char (string copy prevention)
  // 3. Recursion
  // 4. Bring back separator, to have full path to current directory
  // 5. Create directory
  pos = strrchr(target, '/');
  if (NULL != pos && target != pos) {
    pos[0] = '\0';
    if (!mkdirs(target)) {
      pos[0] = '/';
      return false;
    }
    if (0 != mkdir(target, 0755)) {
      return false;
    }
    pos[0] = '/';
  }
  return true;
}

bool mkdirs_for_file(char * target) {
  char * posPtr = strrchr(target, '/');
  if (NULL == posPtr) {
    return false;
  }

  ptrdiff_t pos = posPtr - target;;

  target[pos] = '\0';
  bool ret_val = mkdirs(target);
  target[pos] = '/';
  return ret_val;
}
