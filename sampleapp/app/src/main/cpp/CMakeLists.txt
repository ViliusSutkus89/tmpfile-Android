cmake_minimum_required(VERSION 3.4.1)
project(tmpfile-sampleapp)

add_library(native-lib SHARED native-lib.cpp)

find_package(tmpfile REQUIRED CONFIG)
target_link_libraries(native-lib tmpfile::tmpfile)

