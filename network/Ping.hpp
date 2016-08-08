#ifndef _PING_HPP_
#define _PING_HPP_

#include "network/Socket.hpp"
#include "network/Ipv4Header.hpp"
#include "network/IcmpHeader.hpp"
#include "common/Stopwatch.hpp"


class Ping : protected Socket
{
public:
	void start(const char *hostName, int times = 4, int timeout = 3000);
	void start(unsigned long ip, int times = 4, int timeout = 3000);

protected:
	typedef enum ControlMessage
	{
		ECHO_REPLY = 0,
		DEST_UNREACHABLE = 3,
		ECHO_REQUEST = 8,
		TTL_EXPIRE = 11
	} ControlMessage;

	enum BufferSize
	{
		DEFAULT_PACKET_SIZE = 32,
		PING_SEND_SIZE = DEFAULT_PACKET_SIZE + sizeof(IcmpHeader),
		PING_RECV_SIZE = PING_SEND_SIZE + sizeof(Ipv4Header),
	};

	enum IcmpHeaderIndex
	{
		TYPE = 0,
		CHECKSUM = 2,
		SEQUENCE_NUMBER = 6,
		TIME_TO_LIVE = 8,
		SOURCE_ADDRESS = 12
	};

	enum { DEFAULT_TTL = 128 };

	bool setTimeToLive();
	void packetIcmp();
	void increaseIcmpSequenceNumber();
	void send();
	virtual bool send(const char *data, int length);
	int receive();
	virtual int receive(char *data, int maxLength);
	void decode(int receiveLength);

	static uint16_t checksum16(const char *buffer, int length);

	char sendBuffer[PING_SEND_SIZE];
	char receviceBuffer[PING_RECV_SIZE];
	Stopwatch stopwatch;
};

#endif
