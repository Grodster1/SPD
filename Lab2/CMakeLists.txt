cmake_minimum_required(VERSION 3.15)
project(FlowShopOptimization VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SOURCES
    main.cpp
    src/FlowShopProblem.cpp
    src/InstanceGenerator.cpp
    src/Algorithms.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE inc)