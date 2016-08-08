#ifndef _IPV4HEADER_HPP_
#define _IPV4HEADER_HPP_

#include <cstdint>


typedef struct Ipv4Header
{
	uint8_t version : 4;
	uint8_t internetHeaderLength : 4;
	uint8_t differentiatedServicesCodePoint : 6;
	uint8_t explicitCongestionNotification : 2;
	uint16_t totalLength;
	uint16_t identification;
	uint16_t flags : 3;
	uint16_t fragmentOffset : 13;
	uint8_t timeToLive;
	uint8_t protocol;
	uint16_t headerChecksum;
	uint32_t sourceIpAddress;
	uint32_t destinationIpAddress;
} Ipv4Header;

#endif
