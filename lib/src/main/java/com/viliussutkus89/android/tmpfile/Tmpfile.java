/*
 * Tmpfile.java
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

/*
 * Library auto initialization uses ContentProvider.
 * Based on example published by André Tietz at
 * https://medium.com/@andretietz/auto-initialize-your-android-library-2349daf06920
 */

package com.viliussutkus89.android.tmpfile;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.ProviderInfo;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;

public final class Tmpfile extends ContentProvider {

  static {
    System.loadLibrary("tmpfile");
  }

  public static final String s_subfolderInCache = "tmpfiles";
  private static final String s_TAG = "Tmpfile";

  @Override
  public boolean onCreate() {
    Context ctx = getContext();
    if (null == ctx) {
      Log.e(s_TAG, "Failed to get Context in Tmpfile::onCreate()!");
      return false;
    }

    File tmpfileDir = new File(ctx.getCacheDir(), s_subfolderInCache);
    if (!tmpfileDir.exists() && !tmpfileDir.mkdirs()) {
      Log.e(s_TAG, "Failed to create directory for tmpfiles: " + tmpfileDir.getAbsolutePath());
      return false;
    }
    set_tmpfile_dir(tmpfileDir.getAbsolutePath());
    return true;
  }

  private static native void set_tmpfile_dir(String tmpfile_dir);

  /* Required methods for ContentProvider */

  @Override
  public void attachInfo(Context ctx, ProviderInfo providerInfo) {
    if (providerInfo == null) {
      throw new NullPointerException("Tmpfile ProviderInfo cannot be null.");
    }
    // So if the authorities equal the library internal ones, the developer forgot to set his applicationId
    if ("com.viliussutkus89.tmpfile.tmpfile".equals(providerInfo.authority)) {
      throw new IllegalStateException("Incorrect provider authority in manifest. Most likely due to a "
        + "missing applicationId variable in application\'s build.gradle.");
    }
    super.attachInfo(ctx, providerInfo);
  }

  @Nullable
  @Override
  public Cursor query(@NonNull Uri uri, @Nullable String[] strings, @Nullable String s, @Nullable String[] strings1, @Nullable String s1) {
    return null;
  }

  @Nullable
  @Override
  public String getType(@NonNull Uri uri) {
    return null;
  }

  @Nullable
  @Override
  public Uri insert(@NonNull Uri uri, @Nullable ContentValues contentValues) {
    return null;
  }

  @Override
  public int delete(@NonNull Uri uri, @Nullable String s, @Nullable String[] strings) {
    return 0;
  }

  @Override
  public int update(@NonNull Uri uri, @Nullable ContentValues contentValues, @Nullable String s, @Nullable String[] strings) {
    return 0;
  }
}
