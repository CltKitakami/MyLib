#include <fcntl.h>

#include "Socket.hpp"

#ifdef _WIN32
#include <mswsock.h>
#include "Tcp.hpp"
#include "file/File.hpp"
#else
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#endif


#define CHAR_TO_UINT32(c) ((uint32_t)(uint8_t)(c))


void Socket::open(Socket::AddressFamily addressFamily, Socket::Type type, Protocol protocol)
{
	this->initializeInternetAddress(addressFamily);

	if (this->id != (Id)INVALID_SOCKET)
		this->close();

	this->id = ::socket((int)addressFamily, (int)type, (int)protocol);

	if (this->id == (Id)INVALID_SOCKET)
		SOCKET_THROW(SOCKET_EX_SOCKET);
}


void Socket::open(Socket::Type type)
{
	this->open(IPV4, type, (Protocol)0);
}


void Socket::open(Socket::Protocol protocol)
{
	this->open(IPV4, RAW, protocol);
}


void Socket::close()
{
#ifdef _WIN32
	::closesocket(this->id);
#else
	::close(this->id);
#endif
	this->id = (Id)INVALID_SOCKET;
}


void Socket::setBlocking(bool isBlocking)
{
	this->isBlocking = isBlocking;
#ifdef _WIN32
	unsigned long mode = isBlocking == false ? 1 : 0;
	int ret = ::ioctlsocket(this->id, FIONBIO, &mode);
#else
	int ret = ~0;
	int flags = ::fcntl(id, F_GETFL, 0);
	if (flags > 0)
	{
		flags = isBlocking == false ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
		ret = ::fcntl(id, F_SETFL, flags);
	}
#endif
	if (ret != 0)
		SOCKET_THROW(SOCKET_EX_BLOCKING);
}


void Socket::setAddress(const char *ip)
{
	unsigned long dstAddr = inet_addr(ip);

	if (dstAddr == INADDR_NONE)
		dstAddr = Socket::getHostByName(ip);

	this->setAddress(dstAddr);
}


bool Socket::bind()
{
	int ret = ::bind(this->id,
		reinterpret_cast<struct sockaddr *>(&this->hostAddr),
		sizeof(struct sockaddr));

	if (ret != 0)
	{
		if (this->isBlocking == true)
			SOCKET_THROW(SOCKET_EX_BIND);
		else
			return false;
	}

	return true;
}


void Socket::setId(Id id)
{
	if (this->id != (Id)INVALID_SOCKET)
		this->close();

	this->id = id;
}


unsigned long Socket::getHostByName(const char *hostName)
{
	struct hostent *remoteHost = ::gethostbyname(hostName);

	if (remoteHost != nullptr)
	{
		struct in_addr **addrList =
			reinterpret_cast<struct in_addr **>(remoteHost->h_addr_list);
		return addrList[0]->s_addr;
	}
	else
	{
		SOCKET_THROW(SOCKET_EX_GET_HOST_NAME << hostName);
		return 0;
	}
}


std::string Socket::ipv4AddressToString(const char *bytes4)
{
	std::string str;
	str.reserve(3 * 4 + 3);	// xxx.xxx.xxx.xxx

	for (int i = 0; i < 4; ++i)
	{
		uint32_t digit2 = CHAR_TO_UINT32(bytes4[i]) / 100;
		uint32_t remain2 = CHAR_TO_UINT32(bytes4[i]) % 100;
		uint32_t digit1 = remain2 / 10;
		uint32_t remain1 = remain2 % 10;
		uint32_t digit0 = remain1;
		if (digit2 != 0) str += (char)(digit2 + (int)'0');
		if (digit1 != 0) str += (char)(digit1 + (int)'0');
		str += (char)(digit0 + (int)'0');
		if (i != 3) str += '.';
	}

	return str;
}


std::string Socket::ipv4AddressToString(unsigned long ip)
{
	char parts[4] =
	{
		(char)(ip & 0xff),
		(char)((ip >> 8) & 0xff),
		(char)((ip >> 16) & 0xff),
		(char)((ip >> 24) & 0xff)
	};

	return ipv4AddressToString(parts);
}


void Socket::initializeInternetAddress(Socket::AddressFamily addressFamily)
{
#ifdef _MSC_VER
	this->hostAddr.sin_family = (ADDRESS_FAMILY)addressFamily;
#else
	this->hostAddr.sin_family = addressFamily;
#endif
	this->hostAddr.sin_port = 0;
	this->hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
}


#ifndef _WIN32
void Socket::spliceAll(int from, int to, size_t length)
{
	int pipefd[2];
	size_t remain = length;

	::pipe(pipefd);

	while (remain > 0)
	{
		size_t blockSize = remain > 4096 ? 4096 : remain;
		int process = ::splice(from, 0, pipefd[1], 0, blockSize,
			SPLICE_F_MORE | SPLICE_F_MOVE);

		if (process == -1)
			throw Exception(TRACE, "splice error.");

		::splice(pipefd[0], 0, to, 0, blockSize, SPLICE_F_MORE | SPLICE_F_MOVE);
		remain -= (size_t)process;
	}

	::close(pipefd[0]);
	::close(pipefd[1]);
}
#endif


void Socket::receiveFile(Socket::Id remoteId, const char *filePath, size_t length)
{
#ifdef _WIN32
	size_t remain = length;
	const int blockSize = 4096;
	char buffer[blockSize];
	File file(filePath, File::BINARY_WRITE);

	while (remain > 0)
	{
		int receSize = Tcp::receive(remoteId, buffer, blockSize);
		file.write(buffer, receSize);
		remain -= (size_t)receSize;
	}
#else
	int fileId = ::open(filePath, O_CREAT | O_RDWR | O_TRUNC, 0777);

	if (fileId < 0)
		SOCKET_THROW("Can not open " << filePath);

	spliceAll(remoteId, fileId, length);
	::close(fileId);
#endif
}


void Socket::sendFile(const char *filePath, Socket::Id remoteId)
{
#ifdef _WIN32
	HANDLE hFile = ::CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, 0,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		SOCKET_THROW("Can not open " << filePath);

	const LONGLONG maxSize = (LONGLONG)((2u << 30) - 1u);
	LARGE_INTEGER fileSize, totalBytes;
	fileSize.QuadPart = 0;
	totalBytes.QuadPart = 0;
	::GetFileSizeEx(hFile, &fileSize);
	LONGLONG remainBytes = fileSize.QuadPart;

	while (remainBytes > 0)
	{
		LONGLONG process = remainBytes > maxSize ? maxSize : remainBytes;

		if (::TransmitFile(remoteId, hFile, (DWORD)process, 0,
			nullptr, nullptr, 0) == FALSE)
			SOCKET_THROW("send file error");

		remainBytes -= process;
		totalBytes.QuadPart += process;
		::SetFilePointerEx(hFile, totalBytes, nullptr, FILE_BEGIN);
	}

	::CloseHandle(hFile);
#else
	int fileId = ::open(filePath, O_RDONLY, 0777);

	if (fileId < 0)
		SOCKET_THROW("Can not open " << filePath);

	struct stat fileStat;
	::fstat(fileId, &fileStat);
	spliceAll(fileId, remoteId, (size_t)fileStat.st_size);
	::close(fileId);
#endif
}
