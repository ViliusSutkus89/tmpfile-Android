/*
 * TmpfileInstrumentedTests.java
 *
 * Copyright (C) 2019 - 2021 Vilius Sutkus'89
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    Tmpfile.init(InstrumentationRegistry.getInstrumentation().getTargetContext().getCacheDir());
    System.loadLibrary("nativeUnitTests");
  }

  @Test
  public void tmpfileDirCreated() {
    final File tmpfileDir = Tmpfile.getTmpfileDir();
    assertTrue("Tmpfile directory does not exist!", tmpfileDir.exists());
    assertTrue("Tmpfile directory is not actually a directory!", tmpfileDir.isDirectory());
    assertTrue("Tmpfile directory is not readable!", tmpfileDir.canRead());
    assertTrue("Tmpfile directory is not writable!", tmpfileDir.canWrite());
  }

  @Test
  public void selfTest() {
    assertTrue("Self test failed!", Tmpfile.self_test());
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
