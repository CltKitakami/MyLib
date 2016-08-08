cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestString TestString.cpp)
target_link_libraries(TestString ${CMAKE_PROJECT_NAME})

add_executable(TestTime TestTime.cpp)
target_link_libraries(TestTime ${CMAKE_PROJECT_NAME})

add_executable(TestBacktrace TestBacktrace.cpp)
target_link_libraries(TestBacktrace ${CMAKE_PROJECT_NAME})
