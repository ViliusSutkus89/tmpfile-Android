package com.viliussutkus89.android.tmpfile.sampleapp;

import android.app.Application;

import com.viliussutkus89.android.tmpfile.Tmpfile;

public class MyApplication extends Application {
  @Override
  public void onCreate() {
    super.onCreate();

    Tmpfile.init(getApplicationContext().getCacheDir());
  }
}
