#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <cstdint>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#elif defined(__linux__)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#error "Unknow OS"
#endif

#include "SocketException.hpp"


#ifndef _WIN32
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif
#ifndef SOMAXCONN
#define SOMAXCONN (128)
#endif
#endif


#ifdef _WIN32
#define SOCKET_THROW(msg)\
do\
{\
	SocketException se(TRACE, "Error: ");\
	se << ::WSAGetLastError() << ' ' << msg;\
	throw se;\
} while (0)
#else
#define SOCKET_THROW(msg)\
do\
{\
	SocketException se(TRACE, "Error: ");\
	se << msg;\
	throw se;\
} while (0)
#endif


#ifdef _WIN32
inline bool WsaStartup()
{
	WSADATA wsa;
	return ::WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
}

inline void WsaCleanup() { ::WSACleanup(); }
#endif

class Wsa
{
public:
#ifdef _WIN32
	Wsa() { ::WsaStartup(); }
	~Wsa() { ::WsaCleanup(); }
#endif
};


class Socket
{
public:
	typedef enum AddressFamily
	{
		IPV4 = AF_INET,
		IPV6 = AF_INET6
	} AddressFamily;

	typedef enum Type
	{
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM,
		RAW = SOCK_RAW
	} Type;

	typedef enum Protocol
	{
		ICMP = IPPROTO_ICMP,
		IGMP = IPPROTO_IGMP,
		RAW_SOCKET = IPPROTO_RAW,
		RAW_TCP = IPPROTO_TCP,
		RAW_UDP = IPPROTO_UDP
	} Protocol;

#ifndef _WIN32
	typedef int SOCKET;
#endif
	typedef SOCKET Id;

	Socket() : id((Id)INVALID_SOCKET), isBlocking(false) {}
	Socket(Id id) : isBlocking(false) { this->setId(id); }
	virtual ~Socket() { this->close(); }

	void open(Socket::AddressFamily addressFamily, Socket::Type type, Protocol protocol);
	void open(Type type);
	void open(Protocol protocol);

	void close();

	void setBlocking(bool isBlocking);
	bool getBlocking() { return this->isBlocking; }

	void setAddress(const char *ip);
	void setAddress(unsigned long ip)
	{ this->hostAddr.sin_addr.s_addr = ip; }
	void setPort(uint16_t port)
	{ this->hostAddr.sin_port = (unsigned short)htons(port); }

	bool bind();

	virtual bool send(const char *data, int length) = 0;
	virtual int receive(char *data, int maxLength) = 0;

	Id getId() const { return this->id; }
	void setId(Id id);

	static unsigned long getHostByName(const char *hostName);
	static std::string ipv4AddressToString(const char *bytes4);
	static std::string ipv4AddressToString(unsigned long ip);

#ifndef _WIN32
	static void spliceAll(int from, int to, size_t length);
#endif
	static void receiveFile(Id remoteId, const char *filePath, size_t length);
	static void sendFile(const char *filePath, Id remoteId);

	operator bool () { return this->id != (Socket::Id)INVALID_SOCKET; }

protected:
	void initializeInternetAddress(Socket::AddressFamily addressFamily);

	Id id;
	bool isBlocking;
	struct sockaddr_in hostAddr;
};

#endif
