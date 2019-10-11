package com.viliussutkus89.tmpfile.tests;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertTrue;

import java.io.File;

@RunWith(AndroidJUnit4.class)
public class TmpfileInstrumentedTests {
  static {
    System.loadLibrary("nativeUnitTests");
  }

  @Test
  public void tmpfileDirCreated() {
    final File tmpfileDir = TmpfileTestSuite.getTmpfileDir();
    assertTrue("Tmpfile directory does not exist!", tmpfileDir.exists());
    assertTrue("Tmpfile directory is not actually a directory!", tmpfileDir.isDirectory());
    assertTrue("Tmpfile directory is not readable!", tmpfileDir.canRead());
    assertTrue("Tmpfile directory is not writable!", tmpfileDir.canWrite());
  }

  @Test
  public void openAndCloseTmpfile() {
    assertTrue("Failed to open and close tmpfile!", open_and_close_tmpfile());
  }

  @Test
  public void openAndCloseTmpfileTwice() {
    assertTrue("Failed to open and close tmpfile once!", open_and_close_tmpfile());
    assertTrue("Failed to open and close tmpfile for the second time!", open_and_close_tmpfile());
  }

  @Test
  public void writeToTmpfile() {
    assertTrue("Failed to write to tmpfile!", write_to_tmpfile());
  }

  @Test
  public void writeAndReadbackFromTmpfile() {
    assertTrue("Failed to read from previously written tmpfile!", write_and_readback_from_tmpfile());
  }

  @Test
  @Ignore("On exit deletion of unclosed (leaked) tmpfiles not yet implemented.")
  public void unclosedTmpfileNotGarbageCollected() {
    open_and_not_close_tmpfile();
  }

  private native boolean open_and_close_tmpfile();

  private native boolean write_to_tmpfile();

  private native boolean write_and_readback_from_tmpfile();

  private native boolean open_and_not_close_tmpfile();

}
