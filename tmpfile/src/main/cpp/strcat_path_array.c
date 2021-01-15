/*
 * strcat_path_array.c
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

#include <stdlib.h>
#include <stdbool.h>
#include "strcat_path_array.h"

// wrs: without repeating slashes
static size_t get_string_length_wrs(const char *input, bool *is_slash) {
  size_t length = 0;
  bool was_slash, is_slash_ = *is_slash;
  for (size_t i = 0; '\0' != input[i]; i++) {
    was_slash = is_slash_;
    is_slash_ = ('/' == input[i]);
    if (!is_slash_ || !was_slash || is_slash_ != was_slash) {
      length++;
    }
  }
  *is_slash = is_slash_;
  return length;
}

static size_t copy_string_wrs(char *dst, const char *src, bool *is_slash) {
  size_t length = 0;
  bool was_slash, is_slash_ = *is_slash;

  for (size_t i = 0; '\0' != src[i]; i++) {
    was_slash = is_slash_;
    is_slash_ = ('/' == src[i]);
    if (!is_slash_ || !was_slash || is_slash_ != was_slash) {
      dst[length++] = src[i];
    }
  }
  *is_slash = is_slash_;
  return length;
}

// Concat array of strings
// Remove repeating forward slashes
// input terminator is either NULL ptr or empty string
char *strcat_path_array(const char *input[]) {
  char *output;
  size_t total_length = 0;
  bool is_slash = false;

  for (size_t i = 0; NULL != input[i] && '\0' != input[i][0]; i++) {
    total_length += get_string_length_wrs(input[i], &is_slash);
  }

  if (0 == total_length) {
    return NULL;
  }

  // +1 to account for NULL terminator
  output = malloc(total_length + 1);
  if (NULL == output) {
    return NULL;
  }

  is_slash = false;
  ptrdiff_t j = 0;
  for (size_t i = 0; NULL != input[i] && '\0' != input[i][0]; i++) {
    j += copy_string_wrs(&output[j], input[i], &is_slash);
  }
  output[j] = '\0';
  return output;
}
