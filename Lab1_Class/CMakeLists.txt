cmake_minimum_required(VERSION 3.16)
project(Scheduling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    scheduling 
    main.cpp
    Algorithms.cpp
    Problem.cpp
)

add_executable(
    benchmark
    benchmark.cpp
    Algorithms.cpp
    Problem.cpp
)

add_executable(benchmark100
    benchmark100.cpp
    Problem.cpp
    Algorithms.cpp
)
