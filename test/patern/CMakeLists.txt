cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestDelegate TestDelegate.cpp)
target_link_libraries(TestDelegate ${CMAKE_PROJECT_NAME})

add_executable(TestRunable TestRunable.cpp)
target_link_libraries(TestRunable ${CMAKE_PROJECT_NAME})

add_executable(TestSubject TestSubject.cpp)
target_link_libraries(TestSubject ${CMAKE_PROJECT_NAME})
