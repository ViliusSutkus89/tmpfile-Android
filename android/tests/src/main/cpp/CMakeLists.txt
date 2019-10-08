cmake_minimum_required(VERSION 3.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")
add_library(nativeUnitTests SHARED nativeUnitTests.cpp)

# Link against android log library
find_library(log-lib log)
target_link_libraries(nativeUnitTests ${log-lib})

STRING(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_lowercase)
# Link against libtmpfile
add_library(tmpfile SHARED IMPORTED)
set_property(TARGET tmpfile PROPERTY IMPORTED_LOCATION
  ${CMAKE_CURRENT_LIST_DIR}/../../../../tmpfile/build/intermediates/merged_native_libs/${CMAKE_BUILD_TYPE_lowercase}/out/lib/${ANDROID_ABI}/libtmpfile.so)
target_link_libraries(nativeUnitTests tmpfile)
