cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestProcessTimes TestProcessTimes.cpp)
target_link_libraries(TestProcessTimes ${CMAKE_PROJECT_NAME})
