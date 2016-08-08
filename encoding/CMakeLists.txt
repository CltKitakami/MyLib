cmake_minimum_required(VERSION 2.8.8)

project(encoding)

set(encoding_SRCS
Base64.cpp
ChineseTranslater.cpp
Crc.cpp
Unicode.cpp
Gzip.cpp
Md5.cpp
Sha1.cpp
Sha256.cpp
Sha512.cpp
)

add_library(${PROJECT_NAME} OBJECT ${encoding_SRCS})
