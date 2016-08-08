cmake_minimum_required(VERSION 2.8.8)

project(MyLib)


option(VERBOSE "VERBOSE" OFF)
option(BUILD_TEST_APPS "BUILD_TEST_APPS" ON)
option(ENABLE_JPG "ENABLE_JPG" ON)
option(ENABLE_PNG "ENABLE_PNG" ON)
option(ENABLE_OPENSSL "ENABLE_OPENSSL" ON)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release;Debug")
SET(BUILD_PLATFORM "x86" CACHE STRING "BUILD_PLATFORM")
set_property(CACHE BUILD_PLATFORM PROPERTY STRINGS "x86;x64;arm")
set_property(CACHE EXT_INCS PROPERTY STRINGS "")
set_property(CACHE EXT_LIBS PROPERTY STRINGS "")


if(VERBOSE)
	set(CMAKE_VERBOSE_MAKEFILE ON)
else()
	set(CMAKE_VERBOSE_MAKEFILE OFF)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message("Build type: Debug")
else()
	message("Build type: Release")
	set(CMAKE_BUILD_TYPE "Release")
	add_definitions(-DNDEBUG)
endif()

if("${BUILD_PLATFORM}" STREQUAL "")
	set(BUILD_PLATFORM "x86")
endif()
message("Build platform: ${BUILD_PLATFORM}")


set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")


# Compiler flags
if("${BUILD_PLATFORM}" STREQUAL "arm")
	add_definitions(-DPLATFORM_ARM)
elseif("${BUILD_PLATFORM}" STREQUAL "x64")
	add_definitions(-DPLATFORM_X64)
else()
	add_definitions(-DPLATFORM_X86)
endif()

if(ENABLE_PNG)
add_definitions(-DPNG_USE_C_STDIO)
endif()


if(WIN32)
	add_definitions(-DPTW32_BUILD_INLINED -DPTW32_STATIC_LIB)
	set(SYSTEM_LIBS wsock32)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(SYSTEM_LIBS ${SYSTEM_LIBS} bfd liberty imagehlp intl)
	endif()
else()
	set(SYSTEM_LIBS pthread rt ncurses)	# apt-get install libncurses5-dev
endif()

if(CMAKE_COMPILER_IS_GNUCXX)

	set(WARNINGS "-Wall -Wextra -Wunreachable-code -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wswitch-default -Wunused-parameter -Wreturn-type -Wsign-conversion -Wsign-promo -Werror=non-virtual-dtor")	#  -Wconversion -Wsign-conversion
	set(NOWARNINGS "-Wno-parentheses -Wno-unused-function -Wno-unused-label -Wno-unused-result")

	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
	set(CMAKE_CXX_FLAGS_RELEASE "-Ofast") # -flto

	set(CMAKE_CXX_FLAGS "-pipe ${WARNINGS} ${NOWARNINGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fopenmp")

	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		set(CMAKE_EXE_LINKER_FLAGS "-s")
	endif()

elseif(MSVC)

	message("M$")

	set(WARNINGS "/W4 /WX- /wd4127 /wd4481 /wd4512")
	set(CMAKE_CXX_FLAGS "${WARNINGS} /EHsc")

endif()


# Include path
include_directories("${CMAKE_SOURCE_DIR}")
include_directories(SYSTEM ${EXT_INCS})

# External libraries
if(NOT WIN32)
	find_package(ZLIB)	# apt-get install zlib1g zlib1g-dev
	if (ZLIB_FOUND)
		message("zlib: yes")
		message("ZLIB_INCLUDE_DIRS = ${ZLIB_INCLUDE_DIRS}")
		message("ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")
		include_directories(SYSTEM ${ZLIB_INCLUDE_DIRS})
		set(EXT_LIBS ${EXT_LIBS} ${ZLIB_LIBRARIES})
	else()
		message("zlib: no")
	endif()
endif()


# Subdirectories
# add_subdirectory(audio)
add_subdirectory(common)
add_subdirectory(encoding)
add_subdirectory(file)
add_subdirectory(network)
add_subdirectory(pattern)
add_subdirectory(system)
add_subdirectory(thread)


# Link libraries
add_library(${PROJECT_NAME} STATIC
	# $<TARGET_OBJECTS:audio>
	$<TARGET_OBJECTS:common>
	$<TARGET_OBJECTS:encoding>
	$<TARGET_OBJECTS:file>
	$<TARGET_OBJECTS:network>
	$<TARGET_OBJECTS:pattern>
	$<TARGET_OBJECTS:system>
	$<TARGET_OBJECTS:thread>
)
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS} ${SYSTEM_LIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


# Build test executables
if(BUILD_TEST_APPS)
	add_subdirectory(test)
endif()
