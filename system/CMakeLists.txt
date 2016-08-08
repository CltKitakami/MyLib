cmake_minimum_required(VERSION 2.8.8)

project(system)

set(system_SRCS
FileExplorer.cpp
ProcessTimes.cpp
)

add_library(${PROJECT_NAME} OBJECT ${system_SRCS})
