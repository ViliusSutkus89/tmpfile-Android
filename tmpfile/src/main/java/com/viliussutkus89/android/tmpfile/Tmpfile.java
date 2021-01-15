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

import androidx.annotation.NonNull;

public final class Tmpfile {
  static {
    System.loadLibrary("tmpfile");
  }

  public final static String s_subfolderInCache = "tmpfiles";

  public static void init(@NonNull Context ctx) {
    set_cache_dir(ctx.getCacheDir().getAbsolutePath());
  }

  private static native void set_cache_dir(String cache_dir);

  public static native boolean self_test();
}
