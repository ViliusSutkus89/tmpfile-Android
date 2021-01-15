package com.viliussutkus89.android.tmpfile.tests;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.viliussutkus89.android.tmpfile.Tmpfile;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;

import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class TmpfileInstrumentedTests {
  @BeforeClass
  public static void setUpClass() {
    Tmpfile.init(InstrumentationRegistry.getInstrumentation().getTargetContext());
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
  public void writeAndReadbackLotsOfData() {
    int oneMeg = 1024 * 1024;
    assertTrue("Failed to write and readback one megabyte!", write_and_readback_lots_of_data(oneMeg));

    int sixtyFourMegs = 64 * oneMeg;
    assertTrue("Failed to write and read back 64 megabytes!", write_and_readback_lots_of_data(sixtyFourMegs));
  }

  @Test
  public void openAndLeakTmpfile() {
    assertTrue("Failed to open and not close (leak) tmpfile!", open_and_not_close_tmpfile());
  }

  private native boolean open_and_close_tmpfile();

  private native boolean write_to_tmpfile();

  private native boolean write_and_readback_from_tmpfile();

  private native boolean write_and_readback_lots_of_data(int bytes);

  private native boolean open_and_not_close_tmpfile();

}
