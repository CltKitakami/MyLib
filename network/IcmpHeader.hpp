#ifndef _ICMPHEADER_HPP_
#define _ICMPHEADER_HPP_

#include <cstdint>


typedef struct IcmpHeader
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t identifier;
	uint16_t sequenceNumber;
} IcmpHeader;

#endif
