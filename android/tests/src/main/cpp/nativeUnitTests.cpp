#include <cerrno>
#include <cstdio>
#include <memory>
#include <string_view>

#include <jni.h>
#include <android/log.h>

const char * TAG = "TmpfileTests";
const std::string_view sampleData = "Hello World";

struct TmpFileAutoClose {
  FILE * fh;

  TmpFileAutoClose() {
    fh = tmpfile();
  }

  bool close() {
    bool retVal = true;
    if (nullptr != fh) {
      retVal = (0 == fclose(fh));
    }
    fh = nullptr;
    return retVal;
  }

  ~TmpFileAutoClose() {
    (void) close();
  }
};

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_tmpfile_tests_TmpfileInstrumentedTests_open_1and_1close_1tmpfile(JNIEnv*, jobject) {
  TmpFileAutoClose openedFile;

  if (nullptr == openedFile.fh) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  if (!openedFile.close()) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to close previously opened tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_tmpfile_tests_TmpfileInstrumentedTests_write_1to_1tmpfile(JNIEnv*, jobject) {
  TmpFileAutoClose openedFile;

  size_t written = 0;
  while (written < sampleData.length()) {
    int writtenThisChunk = fwrite(&sampleData.at(written), sizeof(sampleData.at(0)), sampleData.length() - written, openedFile.fh);
    if (-1 == writtenThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fwrite failed!");
      return JNI_FALSE;
    }
    written += writtenThisChunk;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_tmpfile_tests_TmpfileInstrumentedTests_write_1and_1readback_1from_1tmpfile(JNIEnv*, jobject) {
  TmpFileAutoClose openedFile;

  size_t written = 0;
  while (written < sampleData.length()) {
    int writtenThisChunk = fwrite(&sampleData.at(written), sizeof(sampleData.at(0)), sampleData.length() - written, openedFile.fh);
    if (-1 == writtenThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fwrite failed!");
      return JNI_FALSE;
    }
    written += writtenThisChunk;
  }

  if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "fseek failed! errno: %d", errno);
    return JNI_FALSE;
  }

  char readBuffer[sampleData.length() + 1];
  size_t haveRead = 0;
  while (haveRead < sampleData.length()) {
    int haveReadThisChunk = fread(&readBuffer[haveRead], sizeof(sampleData[0]), sampleData.length() - haveRead, openedFile.fh);
    if (-1 == haveReadThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fread failed!");
      return JNI_FALSE;
    }
    haveRead += haveReadThisChunk;
  }
  readBuffer[sampleData.length()] = '\0';

  if (0 != sampleData.compare(readBuffer)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Data read from tmpfile does not match previously written sample data!");
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_tmpfile_tests_TmpfileInstrumentedTests_open_1and_1not_1close_1tmpfile(JNIEnv*, jobject) {
  FILE * fh = tmpfile();

  if (nullptr == fh) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

}
