cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestFile TestFile.cpp)
target_link_libraries(TestFile ${CMAKE_PROJECT_NAME})
add_executable(TestJson TestJson.cpp)
target_link_libraries(TestJson ${CMAKE_PROJECT_NAME})
add_executable(TestXml TestXml.cpp)
target_link_libraries(TestXml ${CMAKE_PROJECT_NAME})
