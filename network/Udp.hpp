#ifndef _UDP_HPP_
#define _UDP_HPP_

#include <cstring>
#include <string>

#include "Socket.hpp"


class Udp : public Socket
{
	int sockaddrLen;
public:
	Udp() : sockaddrLen((int)sizeof(struct sockaddr)) {}
	Udp(Socket::Id id) : Socket(id) {}

	void open();

	static bool send(Id id, const char *data,
		const struct sockaddr *addr, int sockaddrLen, bool isBlocking = false);

	static bool send(Id id, const std::string &data,
		const struct sockaddr *addr, int sockaddrLen, bool isBlocking = false);

	static bool send(Id id, const char *data, int length,
		const struct sockaddr *addr, int sockaddrLen, bool isBlocking = false);

	bool send(const char *data);
	virtual bool send(const char *data, int length);
	bool send(const std::string &data);

	static int receive(Id id, char *data, int maxLength,
		struct sockaddr *addr, int *sockaddrLen, bool isBlocking = false);

	virtual int receive(char *data, int maxLength);
};

#endif
