add_subdirectory(hayai)

set(hayai_SOURCE_DIR hayai/src)

include_directories(${hayai_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

macro (add_benchmark TEST_NAME)
  # Add benchmark cpp file
  add_executable(run_${TEST_NAME} ${TEST_NAME}.cpp)
  include_directories(${CMAKE_SOURCE_DIR})

  # Link benchmark executable against hayai_main
  target_link_libraries(run_${TEST_NAME} hayai_main)
  add_test(run_${TEST_NAME} run_${CHAPTER_NAME})
endmacro()

add_benchmark(queue)
add_benchmark(ordered_list)
