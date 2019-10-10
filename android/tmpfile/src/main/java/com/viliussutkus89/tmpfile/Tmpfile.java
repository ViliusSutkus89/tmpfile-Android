package com.viliussutkus89.tmpfile;

import android.os.FileObserver;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class Tmpfile {

  static {
    System.loadLibrary("tmpfile");
  }

  public static final String s_subfolderInCache = "tmpfiles";
  private final List<FileObserver> m_observers = new ArrayList<>();

  public class TmpfileDirNotExistsAndFailedToCreateException extends Exception {
  }

  public synchronized void setCacheDir(@NonNull File cacheDir) throws TmpfileDirNotExistsAndFailedToCreateException {
    File tmpfileDir = new File(cacheDir, s_subfolderInCache);
    if (!tmpfileDir.exists() && !tmpfileDir.mkdirs()) {
      throw new TmpfileDirNotExistsAndFailedToCreateException();
    }

    m_observers.add(new FileCloseObserver(tmpfileDir));

    set_tmpfile_dir(tmpfileDir.getAbsolutePath());
  }

  private class FileCloseObserver extends FileObserver {
    private final File m_tmpfileFileDir;

    public FileCloseObserver(@NonNull File tmpfileDir) {
      super(tmpfileDir.getAbsolutePath(), FileObserver.CLOSE_WRITE);
      m_tmpfileFileDir = tmpfileDir;
      startWatching();
    }

    @Override
    public void onEvent(int i, @Nullable String closedFileName) {
      if (null != closedFileName) {
        on_file_closed(new File(m_tmpfileFileDir, closedFileName).getAbsolutePath());
      }
    }
  }

  private static native void set_tmpfile_dir(String tmpfile_dir);

  private static native void on_file_closed(String file_path);

}
