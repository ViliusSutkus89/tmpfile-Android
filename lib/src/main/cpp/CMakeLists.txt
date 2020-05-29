cmake_minimum_required(VERSION 3.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")

add_library(tmpfile SHARED tmpfile.cpp)

find_library(log-lib log)
target_link_libraries(tmpfile ${log-lib})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(nativeUnitTests SHARED nativeUnitTests.cpp)
    target_link_libraries(nativeUnitTests ${log-lib} tmpfile)
endif()
