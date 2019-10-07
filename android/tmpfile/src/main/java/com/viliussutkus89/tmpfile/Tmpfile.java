package com.viliussutkus89.tmpfile;

public class Tmpfile {
  static {
    System.loadLibrary("tmpfile");
  }

  public static native void set_tmpfile_dir(String tmpfile_dir);

  public static native void cleanup_tmpfiles();

}
