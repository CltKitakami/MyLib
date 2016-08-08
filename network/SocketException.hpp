#ifndef _SOCKETEXCEPTION_HPP_
#define _SOCKETEXCEPTION_HPP_

#include "common/Exception.hpp"


#define SOCKET_EX "SocketEception"

#define SOCKET_EX_WSADATA "WSADATA error code"
#define SOCKET_EX_SOCKET "Could not create socket"
#define SOCKET_EX_BLOCKING "Could not change blocking mode"
#define SOCKET_EX_ADDRESS_INVALID "Address is invalid"
#define SOCKET_EX_BIND "Bind error"
#define SOCKET_EX_LISTEN "Listen error"
#define SOCKET_EX_ACCEPT "Accept error"
#define SOCKET_EX_CONNECT "Connect error"
#define SOCKET_EX_SEND "Send error"
#define SOCKET_EX_RECEIVE "Receive error"
#define SOCKET_EX_GET_HOST_NAME "Get host by name failed: "
#define SOCKET_EX_SELECT "Select error"


class SocketException : public Exception
{
public:
	SocketException(const char *msg)
	{ this->add(SOCKET_EX, msg); }

	SocketException(const Trace &trace, const char *msg)
	{ this->addTrace(trace); this->add(SOCKET_EX, msg); }

	SocketException(const std::string &msg)
	{ this->add(SOCKET_EX, msg); }

	SocketException(const Trace &trace, const std::string &msg)
	{ this->addTrace(trace); this->add(SOCKET_EX, msg); }
};

#endif
