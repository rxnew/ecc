cmake_minimum_required(VERSION 2.8)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(usec
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/usage_sec.cpp
)
