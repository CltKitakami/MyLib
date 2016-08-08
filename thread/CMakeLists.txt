cmake_minimum_required(VERSION 2.8.8)

project(thread)

set(thread_SRCS
Condition.cpp
Semaphore.cpp
Thread.cpp
Timer.cpp
TimerHandler.cpp
)

add_library(${PROJECT_NAME} OBJECT ${thread_SRCS})
