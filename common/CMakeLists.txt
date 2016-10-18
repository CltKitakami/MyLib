cmake_minimum_required(VERSION 2.8.8)

project(common)

set(common_SRCS
Backtrace.cpp
Date.cpp
Exception.cpp
FilePath.cpp
Hexdump.cpp
Log.cpp
MtRandom.cpp
Stopwatch.cpp
String.cpp
)

add_library(${PROJECT_NAME} OBJECT ${common_SRCS})
