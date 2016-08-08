#include "Udp.hpp"


void Udp::open()
{
	this->Socket::open(UDP);
}


bool Udp::send(Id id, const char *data, int length,
	const struct sockaddr *addr, int sockaddrLen, bool isBlocking)
{
#ifdef _WIN32
	if (::sendto(id, data, (size_t)length, 0, addr, sockaddrLen) == (int)SOCKET_ERROR)
#else
	if (::sendto(id, data, (size_t)length, (size_t)MSG_NOSIGNAL,
		addr, (socklen_t)sockaddrLen) == (int)SOCKET_ERROR)
#endif
	{
		if (isBlocking == true)
			SOCKET_THROW(SOCKET_EX_SEND);
		else
			return false;
	}

	return true;
}


bool Udp::send(Id id, const char *data,
	const struct sockaddr *addr, int sockaddrLen, bool isBlocking)
{
	return Udp::send(id, data, (int)::strlen(data), addr,
		sockaddrLen, isBlocking);
}


bool Udp::send(Id id, const std::string &data,
	const struct sockaddr *addr, int sockaddrLen, bool isBlocking)
{
	return Udp::send(id, data.data(), (int)data.length(),
		addr, sockaddrLen, isBlocking);
}


bool Udp::send(const char *data)
{
	return Udp::send(this->Socket::id, data, (int)::strlen(data),
		reinterpret_cast<const struct sockaddr *>(&this->hostAddr),
		this->sockaddrLen, this->Socket::isBlocking);
}


bool Udp::send(const char *data, int length)
{
	return Udp::send(this->Socket::id, data, length,
		reinterpret_cast<const struct sockaddr *>(&this->hostAddr),
		this->sockaddrLen, this->Socket::isBlocking);
}


bool Udp::send(const std::string &data)
{
	return Udp::send(this->Socket::id, data.data(), (int)data.length(),
		reinterpret_cast<const struct sockaddr *>(&this->hostAddr),
		this->sockaddrLen, this->Socket::isBlocking);
}


int Udp::receive(Id id, char *data, int maxLength,
	struct sockaddr *addr, int *sockaddrLen, bool isBlocking)
{
#ifdef _WIN32
	int recvBytes = ::recvfrom(id, data, maxLength, 0, addr, sockaddrLen);
#else
	int recvBytes = ::recvfrom(id, data, (size_t)maxLength, 0, addr,
		reinterpret_cast<socklen_t *>(sockaddrLen));
#endif

	if (isBlocking == true)
	{
		if (recvBytes == (int)SOCKET_ERROR)
			SOCKET_THROW(SOCKET_EX_RECEIVE);
	}

	return recvBytes;
}


int Udp::receive(char *data, int maxLength)
{
	return Udp::receive(this->Socket::id, data, maxLength,
		reinterpret_cast<struct sockaddr *>(&this->hostAddr),
		&this->sockaddrLen, this->Socket::isBlocking);
}
