cmake_minimum_required(VERSION 3.0.0)
project(MPIPE VERSION 1.0.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread ")

add_subdirectory(packages/config)
add_subdirectory(packages/pipeline)

add_executable(Main main.cpp)
target_link_libraries(
    Main PRIVATE 
    Config 
    Pipeline 
)

