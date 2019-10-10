cmake_minimum_required(VERSION 3.4.1)
add_library(native-lib SHARED native-lib.cpp)

# #############################
# HOWTO link against tmpfile: #
# #############################

# Create imported CMake target
add_library(tmpfile SHARED IMPORTED)
set_property(TARGET tmpfile PROPERTY IMPORTED_LOCATION
  # libtmpfile.so will be extracted in this folder by
  # task extractLibtmpfileSoForLinkingInCMake in sampleapp/app/build.gradle
  ${CMAKE_CURRENT_LIST_DIR}/../../../build/tmpfile/jni/${ANDROID_ABI}/libtmpfile.so
)

target_link_libraries(native-lib tmpfile)

