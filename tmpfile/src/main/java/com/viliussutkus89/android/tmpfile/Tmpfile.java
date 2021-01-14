/*
 * Tmpfile.java
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

package com.viliussutkus89.android.tmpfile;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.File;

public final class Tmpfile {
  public static final String s_subfolderInCache = "tmpfiles";
  private static final String s_TAG = "Tmpfile";

  public static void init(@NonNull Context ctx) {
    System.loadLibrary("c++_shared");
    System.loadLibrary("tmpfile");

    File tmpfileDir = new File(ctx.getCacheDir(), s_subfolderInCache);
    if (!tmpfileDir.exists() && !tmpfileDir.mkdirs()) {
      Log.e(s_TAG, "Failed to create directory for tmpfiles: " + tmpfileDir.getAbsolutePath());
    }
    set_tmpfile_dir(tmpfileDir.getAbsolutePath());
  }

  private static native void set_tmpfile_dir(String tmpfile_dir);
}
