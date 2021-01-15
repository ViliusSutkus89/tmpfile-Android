package com.viliussutkus89.android.tmpfile.tests;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

@RunWith(Suite.class)
@Suite.SuiteClasses({
  TmpfileInstrumentedTests.class,
  StandaloneEXEInstrumentedTests.class,
  NoLeftoverTmpfiles.class
})
public class TmpfileTestSuite {
}
