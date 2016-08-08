cmake_minimum_required(VERSION 2.8.8)

project(network)

set(network_SRCS
CookieContainer.cpp
HeaderContainer.cpp
Http.cpp
Ping.cpp
Socket.cpp
SocketIdSet.cpp
Tcp.cpp
Udp.cpp
Uri.cpp
)

if(ENABLE_OPENSSL)
	message("ENABLE_OPENSSL: ON")
	add_definitions(-DENABLE_OPENSSL)
	set(network_SRCS ${network_SRCS} Openssl.cpp)
endif()

add_library(${PROJECT_NAME} OBJECT ${network_SRCS})
