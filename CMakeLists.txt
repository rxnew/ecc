cmake_minimum_required(VERSION 2.8)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/UsageSec)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/UsageSec/include
)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/UsageSec/lib
)

add_executable(Main
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

target_link_libraries(Main
  usec
)

add_definitions("-std=c++11")
