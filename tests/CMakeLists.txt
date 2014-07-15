add_subdirectory(gtest-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

macro (add_unit_test TEST_NAME)
  # Add test cpp file
  add_executable(test_${TEST_NAME} ${TEST_NAME}.cpp)
  include_directories(${CMAKE_SOURCE_DIR})

  # Link test executable against gtest & gtest_main
  target_link_libraries(test_${TEST_NAME} gtest gtest_main)
  add_test(test-${TEST_NAME} test-${CHAPTER_NAME})
endmacro()

add_unit_test(queue)
