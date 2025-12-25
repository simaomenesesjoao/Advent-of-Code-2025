cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME aoc2025)

project($PROJECT_NAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(utils)
add_subdirectory(days)
