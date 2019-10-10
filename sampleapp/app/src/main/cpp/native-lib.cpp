#include <jni.h>
#include <string>
#include <cassert>

extern "C" JNIEXPORT void JNICALL
Java_com_viliussutkus89_tmpfile_sampleapp_MainActivity_call_1tmpfile(JNIEnv *, jobject) {
  FILE *opened_file = tmpfile();

  assert(nullptr != opened_file);

  std::string_view hello_world = "Hello World\n";
  fwrite(hello_world.cbegin(), sizeof(char), hello_world.length(), opened_file);

  fclose(opened_file);
}
