cmake_minimum_required(VERSION 2.6)
project(retro)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

################################
# GTest
################################
add_subdirectory(gtest-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################
macro (add_unit_test TEST_NAME)
  # Add test cpp file for each chapter
  add_executable(test-${TEST_NAME} tests/${TEST_NAME}.cpp)
  include_directories(${CMAKE_SOURCE_DIR})

  # Link test executable against gtest & gtest_main
  target_link_libraries(test-${TEST_NAME} gtest gtest_main)
  add_test(test-${TEST_NAME} test-${CHAPTER_NAME})
endmacro()

add_unit_test(queue)
