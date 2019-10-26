cmake_minimum_required(VERSION 3.6)

add_library(tmpfile SHARED tmpfile.cpp)

# Link against android log library
find_library(log-lib log)
target_link_libraries(tmpfile ${log-lib})
