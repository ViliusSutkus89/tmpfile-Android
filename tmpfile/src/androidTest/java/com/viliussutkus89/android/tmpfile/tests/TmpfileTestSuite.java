package com.viliussutkus89.android.tmpfile.tests;

import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import java.io.File;

import static com.viliussutkus89.android.tmpfile.Tmpfile.s_subfolderInCache;

@RunWith(Suite.class)
@Suite.SuiteClasses({
  TmpfileInstrumentedTests.class,
  StandaloneEXEInstrumentedTests.class,
  NoLeftoverTmpfiles.class
})
public class TmpfileTestSuite {
  public static File getTmpfileDir() {
    File cacheDir = InstrumentationRegistry.getInstrumentation().getTargetContext().getCacheDir();
    return new File(cacheDir, s_subfolderInCache);
  }
  public static File[] getTmpfiles() {
    return getTmpfileDir().listFiles();
  }
}
