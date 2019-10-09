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
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public final class TmpfileInitProvider extends ContentProvider {
  private static final String s_TAG = "Tmpfile";
  private final Tmpfile m_tmpfile = new Tmpfile();

  @Override
  public boolean onCreate() {
    try {
      m_tmpfile.setCacheDir(getContext().getCacheDir());
      return true;
    } catch (NullPointerException e) {
      Log.e(s_TAG, "Failed to get Context in TmpfileInitProvider::onCreate()!");
    } catch (Tmpfile.TmpfileDirNotExistsAndFailedToCreateException e) {
      Log.e(s_TAG, "Failed to create directory for tmpfiles!");
    }
    return false;
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
