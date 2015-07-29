cmake_minimum_required(VERSION 2.8)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(test
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp
)

target_link_libraries(test
  usec
)

add_definitions("-std=c++11")
