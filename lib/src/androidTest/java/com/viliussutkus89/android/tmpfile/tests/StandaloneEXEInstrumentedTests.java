package com.viliussutkus89.android.tmpfile.tests;

import android.content.Context;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.io.IOException;
import java.util.Map;

import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class StandaloneEXEInstrumentedTests {

  private static int CASE_open_and_close_tmpfile = 1;
  private static int CASE_write_to_tmpfile = 2;
  private static int CASE_write_and_readback_from_tmpfile = 3;
  private static int CASE_write_and_readback_one_Meg = 4;
  private static int CASE_write_and_readback_sixty_four_Megs = 5;
  private static int CASE_open_and_not_close_tmpfile = 6;

  private boolean runEXE(Integer argument){
    Context ctx = InstrumentationRegistry.getInstrumentation().getTargetContext();
    String nativeLibraryDir = ctx.getApplicationInfo().nativeLibraryDir;

    ProcessBuilder pb = new ProcessBuilder(
      new File(nativeLibraryDir, "libTests.exe.so").getAbsolutePath(),
      argument.toString()
    );

    Map<String, String> env = pb.environment();
    env.put("LD_LIBRARY_PATH", nativeLibraryDir);
    env.put("TMPDIR", TmpfileTestSuite.getTmpfileDir().getAbsolutePath());

    Process p;
    try {
      p = pb.start();
    } catch (IOException e) {
      e.printStackTrace();
      return false;
    }

    while (true) {
      try {
        return 0 == p.waitFor();
      } catch (InterruptedException ignored) {
      }
    }
  }

  @Test
  public void openAndCloseTmpfile() {
    assertTrue("Failed to open and close tmpfile!", runEXE(CASE_open_and_close_tmpfile));
  }

  @Test
  public void openAndCloseTmpfileTwice() {
    assertTrue("Failed to open and close tmpfile once!", runEXE(CASE_open_and_close_tmpfile));
    assertTrue("Failed to open and close tmpfile for the second time!", runEXE(CASE_open_and_close_tmpfile));
  }

  @Test
  public void writeToTmpfile() {
    assertTrue("Failed to write to tmpfile!", runEXE(CASE_write_to_tmpfile));
  }

  @Test
  public void writeAndReadbackFromTmpfile() {
    assertTrue("Failed to read from previously written tmpfile!", runEXE(CASE_write_and_readback_from_tmpfile));
  }

  @Test
  public void writeAndReadbackLotsOfData() {
    int oneMeg = 1024 * 1024;
    assertTrue("Failed to write and readback one megabyte!", runEXE(CASE_write_and_readback_one_Meg));

    int sixtyFourMegs = 64 * oneMeg;
    assertTrue("Failed to write and read back 64 megabytes!", runEXE(CASE_write_and_readback_sixty_four_Megs));
  }

  @Test
  public void openAndLeakTmpfile() {
    assertTrue("Failed to open and not close (leak) tmpfile!", runEXE(CASE_open_and_not_close_tmpfile));
  }

}
