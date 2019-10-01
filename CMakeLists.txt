cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)")
project(tmpfile
  VERSION 0.9
  DESCRIPTION "tmpfile function overload for broken implementations."
  LANGUAGES CXX)

include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANRD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(tmpfile src/tmpfile.cpp include/tmpfile.h)

generate_export_header(tmpfile)
set_target_properties(tmpfile PROPERTIES PUBLIC_HEADER include/tmpfile.h)

target_include_directories(tmpfile PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_include_directories(tmpfile INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# Android builds debug and release binaries for each of the four ABIs
# Should be installed in separate folders, so that they will not overwrite each other.
if (ANDROID)
  STRING(APPEND CMAKE_INSTALL_PREFIX "/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}")
endif()

install(TARGETS tmpfile
  EXPORT tmpfileTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include)

# Generate /lib/cmake/tmpfileConfig.cmake
write_basic_package_version_file(
  tmpfileConfigVersion.cmake
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(EXPORT tmpfileTargets
  FILE tmpfileConfig.cmake
  DESTINATION lib/cmake/tmpfile)

