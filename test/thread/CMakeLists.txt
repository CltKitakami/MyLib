cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestCondition TestCondition.cpp)
target_link_libraries(TestCondition ${CMAKE_PROJECT_NAME})

add_executable(TestMutex TestMutex.cpp)
target_link_libraries(TestMutex ${CMAKE_PROJECT_NAME})

add_executable(TestOpenMP TestOpenMP.cpp)
target_link_libraries(TestOpenMP ${CMAKE_PROJECT_NAME})

add_executable(TestSemaphore TestSemaphore.cpp)
target_link_libraries(TestSemaphore ${CMAKE_PROJECT_NAME})

add_executable(TestThread TestThread.cpp)
target_link_libraries(TestThread ${CMAKE_PROJECT_NAME})

add_executable(TestTimer TestTimer.cpp)
target_link_libraries(TestTimer ${CMAKE_PROJECT_NAME})
