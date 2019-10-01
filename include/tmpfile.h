/*
 * tmpfile.h
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

#pragma once

namespace Tmpfile {

    // Provide directory to be used in function tmpfile instead of the default /data/local/tmp.
    void set_tmpfile_dir(const char *tmpfile_dir);

    // Trigger tmpfile garbage collection.
    // Also called automatically at the end of the program by destructor of a static object in this library.
    void cleanup_tmpfiles();

}
