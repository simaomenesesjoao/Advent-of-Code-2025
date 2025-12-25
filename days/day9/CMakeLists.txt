set(PROJECT_NAME day9)
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
    aoc_utils)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -O3)