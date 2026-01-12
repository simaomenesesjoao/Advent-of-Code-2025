set(PROJECT_NAME day10)
project(${PROJECT_NAME})

find_package(Eigen3 REQUIRED)
find_package(integer_solver REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    aoc_utils
    integer_solver::integer_solver
    Eigen3::Eigen
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -O3)