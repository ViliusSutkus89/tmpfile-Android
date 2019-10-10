package com.viliussutkus89.tmpfile.tests;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

@RunWith(AndroidJUnit4.class)
public class NoLeftoverTmpfilesAfterTests {
  @Test
  public void noLeftoverTmpfiles() {
    assertEquals("Found Leftover tmpfiles after executing tests!", 0, TmpfileTestSuite.getTmpfiles().length);
  }
}