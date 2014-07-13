cmake_minimum_required(VERSION 2.6)
project(retro)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

add_subdirectory(benchmarks)
add_subdirectory(tests)
