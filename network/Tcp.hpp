#ifndef _TCP_HPP_
#define _TCP_HPP_

#include <string>

#include "Socket.hpp"


class Tcp : public Socket
{
public:
	Tcp() : Socket() {}
	Tcp(Socket::Id id) : Socket(id) {}

	void open();
	bool listen();
	Socket::Id accept();
	bool connect();

	static bool send(Id id, const char *data, bool isBlocking = false);
	static bool send(Id id, const char *data, int length, bool isBlocking = false);
	static bool send(Id id, const std::string &data, bool isBlocking = false);

	bool send(const char *data);
	virtual bool send(const char *data, int length);
	bool send(const std::string &data);

	static int receive(Id id, char *data, int maxLength, bool isBlocking = false);

	virtual int receive(char *data, int maxLength);
};

#endif
