#ifndef _CRC_HPP_
#define _CRC_HPP_

#include <cstdlib>
#include <cstdint>

template <typename SizeType = uint32_t, SizeType polynomial = 0x4c11db7u,
	SizeType initial = ~0u, SizeType finalXor = ~0u,
	bool reversedData = true, bool reversedRemainder = false,
	uint32_t bitWidth = 8 * sizeof(SizeType)>
class Crc
{
public:
	static SizeType checksum(const char *data, size_t length);

private:
	static void initialize();
	static SizeType reverse(SizeType data);
	static uint8_t reverseData(uint8_t data);
	static SizeType reverseTable(SizeType remainder);
	static SizeType reverseRemainder(SizeType remainder);

	static SizeType table[256];
	static bool isInitialized;
};


uint32_t crc32(const char *data, size_t size);
uint16_t crc16(const char *data, size_t size);

#endif
