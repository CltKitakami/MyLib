cmake_minimum_required(VERSION 2.8.8)

project(test)

add_executable(TestSocket TestSocket.cpp)
target_link_libraries(TestSocket ${CMAKE_PROJECT_NAME})

if(ENABLE_OPENSSL)
add_executable(TestOpensslSocket TestOpensslSocket.cpp)
target_link_libraries(TestOpensslSocket ${CMAKE_PROJECT_NAME})
endif()
