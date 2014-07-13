add_subdirectory(hayai)

set(hayai_SOURCE_DIR hayai/src)

include_directories(${hayai_SOURCE_DIR})

macro (add_benchmark TEST_NAME)
  # Add benchmark cpp file
  add_executable(run-${TEST_NAME} ${TEST_NAME}.cpp)
  include_directories(${CMAKE_SOURCE_DIR})

  # Link benchmark executable against hayai_main
  target_link_libraries(run-${TEST_NAME} hayai_main)
  add_test(run-${TEST_NAME} run-${CHAPTER_NAME})
endmacro()

add_benchmark(queue)

