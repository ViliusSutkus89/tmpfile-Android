cmake_minimum_required(VERSION 3.6.0)
project(tmpfile
  VERSION 0.9
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANRD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Do NOT export symbols unless explicitly asked for
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(tmpfile src/tmpfile.cpp)

