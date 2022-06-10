cmake_minimum_required(VERSION 3.15)
project(tc2037-parallelism)

add_executable(main main.c)

find_package(OpenMP)

add_executable(main++ main.cpp)
target_link_libraries(main++ PUBLIC OpenMP::OpenMP_CXX)

