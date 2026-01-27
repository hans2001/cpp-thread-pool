cmake_minimum_required(VERSION 3.20)
project(cpp_thread_pool LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_basic tests/test_basic.cpp)
target_include_directories(test_basic PRIVATE include)
