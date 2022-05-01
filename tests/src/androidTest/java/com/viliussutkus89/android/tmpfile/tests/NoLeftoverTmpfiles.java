package com.viliussutkus89.android.tmpfile.tests;

import static org.junit.Assert.assertEquals;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.viliussutkus89.android.tmpfile.Tmpfile;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class NoLeftoverTmpfiles {
  @BeforeClass
  public static void setUpClass() {
    Tmpfile.init(InstrumentationRegistry.getInstrumentation().getTargetContext().getCacheDir());
  }

  @Test
  public void noLeftoverTmpfiles() {
    assertEquals("Found leftover tmpfiles!", 0,
        Tmpfile.getTmpfileDir().listFiles().length);
  }
}
