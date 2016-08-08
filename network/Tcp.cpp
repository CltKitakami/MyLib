#include <errno.h>
#include <cstring>

#include "Tcp.hpp"


void Tcp::open()
{
	this->Socket::open(TCP);
}


bool Tcp::listen()
{
	if (::listen(this->Socket::id, SOMAXCONN) == SOCKET_ERROR)
	{
		if (this->Socket::isBlocking == true)
			SOCKET_THROW(SOCKET_EX_LISTEN);
		else
			return false;
	}

	return true;
}


Socket::Id Tcp::accept()
{
	Socket::Id newSocket = ::accept(this->Socket::id, 0, 0);

	if (this->Socket::isBlocking == true)
	{
		if (newSocket == (Socket::Id)SOCKET_ERROR)
			SOCKET_THROW(SOCKET_EX_ACCEPT);
	}

	return newSocket;
}


bool Tcp::connect()
{
	int ret = ::connect(this->Socket::id,
		reinterpret_cast<struct sockaddr *>(&this->hostAddr),
		sizeof(struct sockaddr));

	if (ret == SOCKET_ERROR)
	{
		if (this->Socket::isBlocking == true)
			SOCKET_THROW(SOCKET_EX_CONNECT);
		else if (errno != EINPROGRESS)
			SOCKET_THROW(SOCKET_EX_CONNECT);
		else
			return false;
	}

	return true;
}


bool Tcp::send(Id id, const char *data, int length, bool isBlocking)
{
#ifdef _WIN32
	if (::send(id, data, length, 0) == (int)SOCKET_ERROR)
#else
	if (::send(id, data, (size_t)length, (size_t)MSG_NOSIGNAL) == (int)SOCKET_ERROR)
#endif
	{
		if (isBlocking == true)
			SOCKET_THROW(SOCKET_EX_SEND);
		else
			return false;
	}

	return true;
}


bool Tcp::send(Id id, const char *data, bool isBlocking)
{
	return Tcp::send(id, data, (int)::strlen(data), isBlocking);
}


bool Tcp::send(Id id, const std::string &data, bool isBlocking)
{
	return Tcp::send(id, data.data(), (int)data.length(), isBlocking);
}


bool Tcp::send(const char *data)
{
	return Tcp::send(this->Socket::id, data,
		(int)::strlen(data), this->Socket::isBlocking);
}


bool Tcp::send(const char *data, int length)
{
	return Tcp::send(this->Socket::id, data, length,
		this->Socket::isBlocking);
}


bool Tcp::send(const std::string &data)
{
	return Tcp::send(this->Socket::id, data.data(),
		(int)data.length(), this->Socket::isBlocking);
}


int Tcp::receive(Id id, char *data, int maxLength, bool isBlocking)
{
#ifdef _WIN32
	int recvBytes = ::recv(id, data, maxLength, 0);
#else
	int recvBytes = ::read(id, data, (size_t)maxLength);
#endif

	if (isBlocking == true)
	{
		if (recvBytes == (int)SOCKET_ERROR)
			SOCKET_THROW(SOCKET_EX_RECEIVE);
	}

	return recvBytes;
}


int Tcp::receive(char *data, int maxLength)
{
	return Tcp::receive(this->Socket::id, data, maxLength,
		this->Socket::isBlocking);
}
