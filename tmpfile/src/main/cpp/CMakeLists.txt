cmake_minimum_required(VERSION 3.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")

project("tmpfile-Android")

add_library(tmpfile SHARED tmpfile.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(nativeUnitTests SHARED nativeUnitTests.cpp)

    find_library(log-lib log)
    target_link_libraries(nativeUnitTests ${log-lib} tmpfile)

    add_executable(libTests.exe libTests.exe.cpp)
    target_link_libraries(libTests.exe tmpfile)
    set_target_properties(libTests.exe PROPERTIES OUTPUT_NAME libTests.exe.so)
endif()
