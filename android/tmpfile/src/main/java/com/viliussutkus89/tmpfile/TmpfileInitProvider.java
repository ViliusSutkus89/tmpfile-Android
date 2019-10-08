/*
Library initializer based on example published by André Tietz at
https://medium.com/@andretietz/auto-initialize-your-android-library-2349daf06920
 */

package com.viliussutkus89.tmpfile;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.ProviderInfo;
import android.database.Cursor;
import android.net.Uri;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;

public final class TmpfileInitProvider extends ContentProvider {
  static {
    // Call static{} block of Tmpfile.
    new Tmpfile();
  }

  public static void set_tmpfile_dir(Context ctx) {
    final File cacheDir = new File(ctx.getCacheDir(), "tmpfile");
    if (!cacheDir.exists()) {
      cacheDir.mkdirs();
    }
    Tmpfile.set_tmpfile_dir(cacheDir.getAbsolutePath());
  }

  @Override
  public boolean onCreate() {
    set_tmpfile_dir(getContext());
    return true;
  }

  @Override
  public void attachInfo(Context ctx, ProviderInfo providerInfo) {
    if (providerInfo == null) {
      throw new NullPointerException("TmpfileInitProvider ProviderInfo cannot be null.");
    }
    // So if the authorities equal the library internal ones, the developer forgot to set his applicationId
    if ("com.viliussutkus89.tmpfile.tmpfileinitprovider".equals(providerInfo.authority)) {
      throw new IllegalStateException("Incorrect provider authority in manifest. Most likely due to a "
        + "missing applicationId variable in application\'s build.gradle.");
    }
    super.attachInfo(ctx, providerInfo);
  }

  @Nullable
  @Override
  public Cursor query(@NonNull Uri uri, @Nullable String[] projection, @Nullable String selection, @Nullable String[] selectionArgs, @Nullable String sortOrder) {
    return null;
  }

  @Nullable
  @Override
  public String getType(@NonNull Uri uri) {
    return null;
  }

  @Nullable
  @Override
  public Uri insert(@NonNull Uri uri, @Nullable ContentValues values) {
    return null;
  }

  @Override
  public int delete(@NonNull Uri uri, @Nullable String selection, @Nullable String[] selectionArgs) {
    return 0;
  }

  @Override
  public int update(@NonNull Uri uri, @Nullable ContentValues values, @Nullable String selection, @Nullable String[] selectionArgs) {
    return 0;
  }
}
