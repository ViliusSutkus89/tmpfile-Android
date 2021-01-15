cmake_minimum_required(VERSION 3.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")

project("tmpfile-Android-tests")

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")

find_package(tmpfile REQUIRED CONFIG)
find_library(log-lib log)

add_library(nativeUnitTests SHARED nativeUnitTests.cpp)

target_link_libraries(nativeUnitTests ${log-lib} tmpfile::tmpfile)

add_executable(libTests.exe libTests.exe.cpp)
target_link_libraries(libTests.exe tmpfile::tmpfile)
set_target_properties(libTests.exe PROPERTIES OUTPUT_NAME libTests.exe.so)
