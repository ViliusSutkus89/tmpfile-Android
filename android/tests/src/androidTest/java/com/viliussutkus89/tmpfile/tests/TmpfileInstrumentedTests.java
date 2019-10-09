package com.viliussutkus89.tmpfile.tests;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.FixMethodOrder;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;

import static org.junit.Assert.*;

import java.io.File;

import static com.viliussutkus89.tmpfile.Tmpfile.s_subfolderInCache;

/*
Sort tests by method name using @FixMethodOrder(MethodSorters.NAME_ASCENDING).
Subsequent tests may crash, because they assume things from previous tests are working correctly.
*/
@RunWith(AndroidJUnit4.class)
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TmpfileInstrumentedTests {
  static {
    System.loadLibrary("nativeUnitTests");
  }

  private File getTmpfileDir() {
    final File cacheDir = InstrumentationRegistry.getInstrumentation().getTargetContext().getCacheDir();
    return new File(cacheDir, s_subfolderInCache);
  }

  @Test
  public void a0_tmpfileDirCreated() {
    final File tmpfileDir = getTmpfileDir();
    assertTrue("Tmpfile directory does not exist!", tmpfileDir.exists());
    assertTrue("Tmpfile directory is not actually a directory!", tmpfileDir.isDirectory());
    assertTrue("Tmpfile directory is not readable!", tmpfileDir.canRead());
    assertTrue("Tmpfile directory is not writable!", tmpfileDir.canWrite());
  }


  @Test
  public void b1_openAndCloseTmpfile() {
    assertTrue("Failed to open and close tmpfile!", open_and_close_tmpfile());
  }

  @Test
  public void b2_writeToTmpfile() {
    assertTrue("Failed to write to tmpfile!", write_to_tmpfile());
  }

  @Test
  public void b3_writeAndReadbackFromTmpfile() {
    assertTrue("Failed to read from previously written tmpfile!", write_and_readback_from_tmpfile());
  }


  private native boolean open_and_close_tmpfile();

  private native boolean write_to_tmpfile();

  private native boolean write_and_readback_from_tmpfile();
}
