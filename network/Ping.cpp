#ifdef _WIN32
#include <ws2tcpip.h>
#elif defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "Ping.hpp"
#include "network/SocketIdSet.hpp"
#include "common/Log.hpp"
#include "common/Convert.hpp"
#include "thread/Thread.hpp"


#define CHAR_TO_UINT32(c) ((uint32_t)(uint8_t)(c))
#define CHAR_TO_UINT16(c) ((uint16_t)(uint8_t)(c))


void Ping::start(const char *hostName, int times, int timeout)
{
	this->start(Socket::getHostByName(hostName), times, timeout);
}


void Ping::start(unsigned long ip, int times, int timeout)
{
	SocketIdSet set(1);
	this->Socket::open(Socket::ICMP);
	this->Socket::setAddress(ip);
	this->packetIcmp();
	set.read.add(this->Socket::id);

	if (this->setTimeToLive() == false)
		return;

	for (int i = 0; i < times; ++i)
	{
		if (i != 0)
		{
			Thread::sleep(1000);
			this->Socket::setBlocking(true);
			this->increaseIcmpSequenceNumber();
			set.read.restore();
		}

		this->stopwatch.start();
		this->send();
		this->Socket::setBlocking(false);

		bool isTimeout = false;
		try
		{
			while (set.timedSelect(timeout) == 0)
			{
				if (set.read.available(this->Socket::id) == true)
					break;
			}
		}
		catch (Exception &)
		{
			isTimeout = true;
			LOGP("Timeout");
		}

		if (isTimeout == false)
		{
			this->stopwatch.stop();
			this->decode(this->receive());
		}
	}
}


uint16_t Ping::checksum16(const char *buffer, int length)
{
    unsigned long sum = 0;
	const uint16_t *p = reinterpret_cast<const uint16_t *>(buffer);

    while (length > 1)
	{
		sum += (unsigned long)*p++;
		length -= 2;
    }

    if (length > 0)
		sum += (unsigned long)*(reinterpret_cast<const uint8_t *>(p));

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}


bool Ping::setTimeToLive()
{
	int ttl = DEFAULT_TTL;
	if (setsockopt(this->Socket::id, IPPROTO_IP, IP_TTL, (const char*)&ttl,
		(socklen_t)sizeof(ttl)) == SOCKET_ERROR)
	{
		LOG.e(TRACE, "TTL setsockopt failed.");
		return false;
	}
	else
		return true;
}


void Ping::packetIcmp()
{
	uint16_t identifier = 256;//(uint16_t)GetCurrentProcessId();
	uint16_t sequenceNumber = 0xffff;
	char *p = this->sendBuffer;

	*p = ECHO_REQUEST; p += 1;
	*p = 0; p += 1;
	*p = 0; p += 1;
	*p = 0; p += 1;
	Convert::splitDualBytes(identifier, p); p += 2;
	Convert::splitDualBytes(sequenceNumber, p); p += 2;

	int i = 0;
	while (i < DEFAULT_PACKET_SIZE - 2)
	{
		for (int c = (int)'a';
			c <= (int)'w' && i < DEFAULT_PACKET_SIZE - 2; ++i, ++c, ++p)
		{
			*p = (char)c;
		}
	}

	*p = 'h', *(++p) = 'i';

	uint16_t checksum = checksum16(this->sendBuffer, PING_SEND_SIZE);
	Convert::splitDualBytes(checksum, &this->sendBuffer[CHECKSUM]);
}


void Ping::increaseIcmpSequenceNumber()
{
	char *p = this->sendBuffer;
	p[SEQUENCE_NUMBER] += 1;
	p[CHECKSUM] -= 1;

	if (p[SEQUENCE_NUMBER] == '\x00')
	{
		p[SEQUENCE_NUMBER + 1] += 1;
		if (p[SEQUENCE_NUMBER + 1] == '\x00')
			p[CHECKSUM] += 1;
		else
		{
			if (p[CHECKSUM] == '\xff')
				p[CHECKSUM + 1] -= 1;
		}
	}
	else
	{
		if (p[CHECKSUM] == '\xff')
			p[CHECKSUM + 1] -= 1;
	}
}


void Ping::send()
{
	(void)this->send(this->sendBuffer, PING_SEND_SIZE);
}


bool Ping::send(const char *data, int length)
{
	int ret = ::sendto(this->Socket::id, data, (size_t)length, 0,
		reinterpret_cast<const struct sockaddr *>(&this->Socket::hostAddr),
		(socklen_t)sizeof(struct sockaddr));

	if (ret == (int)SOCKET_ERROR)
		SOCKET_THROW(SOCKET_EX_SEND);

	return true;
}


int Ping::receive()
{
	char *p = this->receviceBuffer;
	return this->receive(p, PING_RECV_SIZE);
}


int Ping::receive(char *data, int maxLength)
{
	int sockaddrLen = (int)sizeof(struct sockaddr);
#ifdef _WIN32
	int recvBytes = ::recvfrom(this->Socket::id, data, maxLength, 0,
		reinterpret_cast<struct sockaddr *>(&this->hostAddr), &sockaddrLen);
#else
	int recvBytes = ::recvfrom(this->Socket::id, data, (size_t)maxLength, 0,
		reinterpret_cast<struct sockaddr *>(&this->hostAddr),
		reinterpret_cast<socklen_t *>(&sockaddrLen));
#endif
	if (recvBytes == (int)SOCKET_ERROR)
		SOCKET_THROW(SOCKET_EX_RECEIVE);

	return recvBytes;
}


void Ping::decode(int receiveLength)
{
	char *ipv4 = this->receviceBuffer;
	int headerLength = ((uint8_t)ipv4[0] & 0xf) * 4;

	char *icmp = &this->receviceBuffer[headerLength];
	ControlMessage type = (ControlMessage)(uint8_t)icmp[TYPE];
	// uint16_t identifier = Convert::combineDualBytes(icmp[4], icmp[5]);
	uint16_t sequenceNumber = Convert::combineDualBytes(icmp[SEQUENCE_NUMBER], icmp[SEQUENCE_NUMBER + 1]);
	uint8_t timeToLive = (uint8_t)ipv4[TIME_TO_LIVE];

	if (receiveLength < headerLength + (int)sizeof(IcmpHeader))
	{
		LOGP("Too few bytes from " <<
			Socket::ipv4AddressToString(
			this->Socket::hostAddr.sin_addr.s_addr));
	}
	else if (type != ECHO_REPLY && type != TTL_EXPIRE)
	{
		if (type == DEST_UNREACHABLE)
			LOGP("Destination unreachable.");
		else
			LOGP("TODO: Recevied ICMP packet type = " << (int)type);
	}
	else
	{
		if (CHAR_TO_UINT32(timeToLive) == 64)
			timeToLive = 1;	// TTL came back 64, so ping was probably to a host on the LAN

		float elapsed = (float)this->stopwatch.getMicros() / 1000;
		LOGP(receiveLength << " bytes from " <<
			Socket::ipv4AddressToString(&ipv4[SOURCE_ADDRESS]) <<
			": icmp_seq=" << (uint32_t)sequenceNumber <<
			" ttl=" << (uint32_t)(uint8_t)timeToLive <<
			" time=" << elapsed << " ms");
	}
}
