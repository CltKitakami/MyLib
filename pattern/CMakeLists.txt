cmake_minimum_required(VERSION 2.8.8)

project(pattern)

set(pattern_SRCS
EventLoop.cpp
Runnable.cpp
ThreadPool.cpp
)

add_library(${PROJECT_NAME} OBJECT ${pattern_SRCS})
