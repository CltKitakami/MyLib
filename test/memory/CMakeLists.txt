cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestObjectPool TestObjectPool.cpp)
target_link_libraries(TestObjectPool ${CMAKE_PROJECT_NAME})

add_executable(TestPointer TestPointer.cpp)
target_link_libraries(TestPointer ${CMAKE_PROJECT_NAME})
