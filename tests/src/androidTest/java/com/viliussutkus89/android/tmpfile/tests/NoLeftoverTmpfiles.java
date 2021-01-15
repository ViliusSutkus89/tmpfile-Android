package com.viliussutkus89.android.tmpfile.tests;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.viliussutkus89.android.tmpfile.Tmpfile;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

@RunWith(AndroidJUnit4.class)
public class NoLeftoverTmpfiles {
  @Test
  public void noLeftoverTmpfiles() {
    assertEquals("Found leftover tmpfiles!", 0,
        Tmpfile.getTmpfileDir().listFiles().length);
  }
}
