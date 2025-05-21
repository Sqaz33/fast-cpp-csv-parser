cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(bench)

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

add_library(CSV INTERFACE)
target_include_directories(CSV INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# бенчмарки
add_executable(line_first_bench "${CMAKE_CURRENT_SOURCE_DIR}/test-src/line-reader-first-bench.cpp")
target_link_libraries(line_first_bench PRIVATE CSV)
target_link_libraries(line_first_bench PRIVATE benchmark::benchmark benchmark::benchmark_main)

add_executable(line_second_bench "${CMAKE_CURRENT_SOURCE_DIR}/test-src/line-reader-second-bench.cpp")
target_link_libraries(line_second_bench PRIVATE CSV)
target_link_libraries(line_second_bench PRIVATE benchmark::benchmark benchmark::benchmark_main)

