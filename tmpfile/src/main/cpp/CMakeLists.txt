cmake_minimum_required(VERSION 3.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")

project("tmpfile-Android")

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")

add_library(tmpfile SHARED
    tmpfile.c
    strcat_path_array.h strcat_path_array.c
    )

find_library(log-lib log)
target_link_libraries(tmpfile ${log-lib})
