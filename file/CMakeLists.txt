cmake_minimum_required(VERSION 2.8.8)

project(file)

set(file_SRCS
Bmp.cpp
BufferedFile.cpp
Csv.cpp
File.cpp
IniFile.cpp
Settings.cpp
Json.cpp
json/JsonContext.cpp
json/JsonLexer.cpp
json/JsonParser.cpp
Wav.cpp
Xml.cpp
xml/XmlContext.cpp
xml/XmlLexer.cpp
xml/XmlParser.cpp
)

if(ENABLE_JPG)
	message("ENABLE_JPG: ON")
	set(file_SRCS ${file_SRCS} Jpg.cpp)
endif()

if(ENABLE_PNG)
	message("ENABLE_PNG: ON")
	set(file_SRCS ${file_SRCS} Png.cpp)
endif()

add_library(${PROJECT_NAME} OBJECT ${file_SRCS})
