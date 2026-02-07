cmake_minimum_required(VERSION 3.20)
project(cpp_thread_pool LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_basic tests/test_basic.cpp)
add_library(tp src/thread_pool.cpp)
target_include_directories(tp PUBLIC include)

target_link_libraries(test_basic PRIVATE tp)

add_executable(test_future tests/test_future.cpp)
target_link_libraries(test_future PRIVATE tp)

add_executable(test_shutdown tests/test_shutdown.cpp)
target_link_libraries(test_shutdown PRIVATE tp)

add_executable(example_hello examples/hello.cpp)
target_link_libraries(example_hello PRIVATE tp)

add_executable(example_parallel_sum examples/parallel_sum.cpp)
target_link_libraries(example_parallel_sum PRIVATE tp)

add_executable(bench_throughput bench/bench_throughput.cpp)
target_link_libraries(bench_throughput PRIVATE tp)

add_executable(bench_latency bench/bench_latency.cpp)
target_link_libraries(bench_latency PRIVATE tp)
