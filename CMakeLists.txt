cmake_minimum_required(VERSION 3.6.0)
project(tmpfile
  VERSION 0.9
  LANGUAGES CXX)

# Do NOT export symbols unless explicitly asked for
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(tmpfile src/tmpfile.cpp)

