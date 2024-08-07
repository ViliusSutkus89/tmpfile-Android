/*
 * Tmpfile.java
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

package com.viliussutkus89.android.tmpfile;

import androidx.annotation.NonNull;
import java.io.File;

public final class Tmpfile {
  static {
    System.loadLibrary("tmpfile");
  }

  public static void init(@NonNull File cacheDir) {
    File tmpfile_dir = new File(cacheDir, "tmpfile");
    set_tmpfile_dir(tmpfile_dir.getAbsolutePath());
    getTmpfileDir().mkdirs();
  }

  private static native void set_tmpfile_dir(String tmpfile_dir);

  private static native String get_tmpfile_dir_path();

  public static File getTmpfileDir() {
    return new File(get_tmpfile_dir_path());
  }

  public static native boolean self_test();
}
