#include "common/Log.hpp"
#include "encoding/Crc.hpp"

uint32_t crc24(const char *data, size_t length)
{
	return Crc<uint32_t, 0x864cfb, 0xb704ce, 0, false, false, 24>::
		checksum(data, length);
}

int main()
{
	LOG_VAR_HEX(crc32("123456789", 9));
	LOG_VAR_HEX(crc16("123456789", 9));
	LOG_VAR_HEX(crc24("123456789", 9));
	return 0;
}
