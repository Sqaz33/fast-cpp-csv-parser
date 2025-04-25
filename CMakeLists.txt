cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(tst)

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

add_library(CSV INTERFACE)
target_include_directories(CSV INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# модульные тесты
add_executable(tst "${CMAKE_CURRENT_SOURCE_DIR}/test-src/unit-tests.cpp")
target_link_libraries(tst PRIVATE CSV)
target_link_libraries(tst PRIVATE GTest::gtest)

# бенчмарки
add_executable(line_bench "${CMAKE_CURRENT_SOURCE_DIR}/test-src/line-reader-benchmark.cpp")
target_link_libraries(line_bench PRIVATE CSV)
target_link_libraries(line_bench PRIVATE benchmark::benchmark benchmark::benchmark_main)

add_executable(csv_bench "${CMAKE_CURRENT_SOURCE_DIR}/test-src/csv_bench.cpp")
target_link_libraries(csv_bench PRIVATE CSV)
target_link_libraries(csv_bench PRIVATE benchmark::benchmark benchmark::benchmark_main)


if (UNIX) 
    target_compile_options(tst PRIVATE -Wall)
endif()

enable_testing()
add_test(NAME csv_test COMMAND tst)
