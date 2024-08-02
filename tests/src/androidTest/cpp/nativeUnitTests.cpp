/*
 * nativeUnitTests.cpp
 *
 * Copyright (C) 2020, 2021 Vilius Sutkus'89
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

#include <cerrno>
#include <cstdio>
#include <memory>
#include <random>
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
Java_com_viliussutkus89_android_tmpfile_tests_TmpfileInstrumentedTests_open_1and_1close_1tmpfile(
    JNIEnv *, jobject) {
  TmpFileAutoClose openedFile;

  if (nullptr == openedFile.fh) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  if (!openedFile.close()) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
                        "Failed to close previously opened tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_android_tmpfile_tests_TmpfileInstrumentedTests_write_1to_1tmpfile(JNIEnv *,
                                                                                          jobject) {
  TmpFileAutoClose openedFile;

  size_t written = 0;
  while (written < sampleData.length()) {
    int writtenThisChunk = fwrite(&sampleData.at(written), sizeof(sampleData.at(0)),
                                  sampleData.length() - written, openedFile.fh);
    if (-1 == writtenThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fwrite failed!");
      return JNI_FALSE;
    }
    written += writtenThisChunk;
  }

  return JNI_TRUE;
}

size_t write_in_chunks(FILE *fh, const char *data, size_t length) {
  size_t written = 0;
  while (written < length) {
    size_t writtenThisChunk = fwrite(&data[written], sizeof(char), length - written, fh);
    if (0 == writtenThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fwrite failed!");
      break;
    }
    written += writtenThisChunk;
  }
  return written;
}

size_t read_in_chunks(FILE *fh, char *buffer, size_t expectedLen) {
  size_t haveRead = 0;
  while (haveRead < expectedLen) {
    size_t leftToRead = expectedLen - haveRead;
    int haveReadThisChunk = fread(&buffer[haveRead], sizeof(char), leftToRead, fh);
    if (0 == haveReadThisChunk) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "fread failed!");
      break;
    }
    haveRead += haveReadThisChunk;
  }
  return haveRead;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_android_tmpfile_tests_TmpfileInstrumentedTests_write_1and_1readback_1from_1tmpfile(
    JNIEnv *, jobject) {
  TmpFileAutoClose openedFile;
  if (sampleData.length() !=
      write_in_chunks(openedFile.fh, sampleData.cbegin(), sampleData.length())) {
    return JNI_FALSE;
  }

  if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "fseek failed! errno: %d", errno);
    return JNI_FALSE;
  }

  std::unique_ptr<char[]> readBuffer(new char[sampleData.length()]);

  if (sampleData.length() != read_in_chunks(openedFile.fh, readBuffer.get(), sampleData.length())) {
    return JNI_FALSE;
  }

  if (0 != memcmp(sampleData.cbegin(), readBuffer.get(), sampleData.length())) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
                        "Data read from tmpfile does not match previously written sample data!");
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_android_tmpfile_tests_TmpfileInstrumentedTests_write_1and_1readback_1lots_1of_1data(
    JNIEnv *, jobject, jint size) {
  TmpFileAutoClose openedFile;

  size_t length = (size_t) size;

  std::unique_ptr<char[]> randomData(new char[length]);
  std::unique_ptr<char[]> readBuffer(new char[length]);

  std::default_random_engine randomGenerator;
  std::uniform_int_distribution<uint8_t> distribution(0, std::numeric_limits<uint8_t>::max());

  for (size_t i = 0; i < length; i++) {
    randomData[i] = distribution(randomGenerator);
  }

  if (length != write_in_chunks(openedFile.fh, randomData.get(), length)) {
    return JNI_FALSE;
  }

  if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "fseek failed! errno: %d", errno);
    return JNI_FALSE;
  }

  if (length != read_in_chunks(openedFile.fh, readBuffer.get(), length)) {
    return JNI_FALSE;
  }

  if (0 != memcmp(randomData.get(), readBuffer.get(), length)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
                        "Data read from tmpfile does not match previously written sample data!");
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_viliussutkus89_android_tmpfile_tests_TmpfileInstrumentedTests_open_1and_1not_1close_1tmpfile(
    JNIEnv *, jobject) {
  FILE *fh = tmpfile();

  if (nullptr == fh) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open tmpfile. errno: %d", errno);
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, __attribute__((unused)) void *unused) {
  __android_log_print(ANDROID_LOG_DEBUG, TAG, "nativeUnitTests::JNI_OnLoad\n");

  JNIEnv *env;
  if (JNI_OK != vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6)) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

}
