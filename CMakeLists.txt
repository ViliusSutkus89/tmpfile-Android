cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")
project(tmpfile
  VERSION 0.9
  DESCRIPTION "tmpfile function overload for broken implementations."
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANRD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(tmpfile src/tmpfile.cpp)

