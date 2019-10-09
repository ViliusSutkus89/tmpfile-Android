package com.viliussutkus89.tmpfile;

import androidx.annotation.NonNull;

import java.io.File;

public class Tmpfile {

  static {
    System.loadLibrary("tmpfile");
  }

  public static final String s_subfolderInCache = "tmpfiles";

  public class TmpfileDirNotExistsAndFailedToCreateException extends Exception {
  }

  public synchronized void setCacheDir(@NonNull File cacheDir) throws TmpfileDirNotExistsAndFailedToCreateException {
    File tmpfileDir = new File(cacheDir, s_subfolderInCache);
    if (!tmpfileDir.exists() && !tmpfileDir.mkdirs()) {
      throw new TmpfileDirNotExistsAndFailedToCreateException();
    }

    set_tmpfile_dir(tmpfileDir.getAbsolutePath());
  }

  private static native void set_tmpfile_dir(String tmpfile_dir);

}
