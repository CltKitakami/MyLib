#include "Crc.hpp"


#define CRC_TEMPLATE \
	template <typename SizeType, SizeType polynomial, \
		SizeType initial, SizeType finalXor, \
		bool reversedData, bool reversedRemainder, \
		uint32_t bitWidth>

#define CRC_TEMPLATE_PARAM \
	SizeType, polynomial, initial, \
	finalXor, reversedData, reversedRemainder, bitWidth


CRC_TEMPLATE SizeType Crc<CRC_TEMPLATE_PARAM>::table[256] = { 0 };
CRC_TEMPLATE bool Crc<CRC_TEMPLATE_PARAM>::isInitialized = false;


uint8_t crcReverse8(uint8_t x)
{
	const uint8_t mask0 = 0x55;
	const uint8_t mask1 = 0x33;

	x = (((~mask0) & x) >> 1) | ((mask0 & x) << 1);
	x = (((~mask1) & x) >> 2) | ((mask1 & x) << 2);

	return (x >> 4) | (x << 4);
}


uint16_t crcReverse16(uint16_t x)
{
	const uint16_t mask0 = 0x5555;
	const uint16_t mask1 = 0x3333;
	const uint16_t mask2 = 0x0F0F;

	x = (((~mask0) & x) >> 1) | ((mask0 & x) << 1);
	x = (((~mask1) & x) >> 2) | ((mask1 & x) << 2);
	x = (((~mask2) & x) >> 4) | ((mask2 & x) << 4);

	return (x >> 8) | (x << 8);
}


uint32_t crcReverse32(uint32_t x)
{
	const uint32_t mask0 = 0x55555555;
	const uint32_t mask1 = 0x33333333;
	const uint32_t mask2 = 0x0f0f0f0f;
	const uint32_t mask3 = 0x00ff00ff;

	x = (((~mask0) & x) >> 1) | ((mask0 & x) << 1);
	x = (((~mask1) & x) >> 2) | ((mask1 & x) << 2);
	x = (((~mask2) & x) >> 4) | ((mask2 & x) << 4);
	x = (((~mask3) & x) >> 8) | ((mask3 & x) << 8);

	return (x >> 16) | (x << 16);
}


uint64_t crcReverse64(uint64_t x)
{
	const uint64_t mask0 = 0x5555555555555555;
	const uint64_t mask1 = 0x3333333333333333;
	const uint64_t mask2 = 0x0f0f0f0f0f0f0f0f;
	const uint64_t mask3 = 0x00ff00ff00ff00ff;
	const uint64_t mask4 = 0x0000ffff0000ffff;

	x = (((~mask0) & x) >> 1) | ((mask0 & x) << 1);
	x = (((~mask1) & x) >> 2) | ((mask1 & x) << 2);
	x = (((~mask2) & x) >> 4) | ((mask2 & x) << 4);
	x = (((~mask3) & x) >> 8) | ((mask3 & x) << 8);
	x = (((~mask4) & x) >> 16) | ((mask4 & x) << 16);

	return (x >> 32) | (x << 32);
}


CRC_TEMPLATE
SizeType Crc<CRC_TEMPLATE_PARAM>::reverse(SizeType data)
{
	switch (bitWidth)
	{
		case 32:
			return (SizeType)crcReverse32((uint32_t)data);
		case 16:
			return (SizeType)crcReverse16((uint16_t)data);
		case 8:
			return (SizeType)crcReverse8((uint8_t)data);
		case 64:
			return (SizeType)crcReverse64((uint64_t)data);
		default:
		{
			SizeType reflection = 0;

			for (uint32_t bit = 0; bit < bitWidth; ++bit)
			{
				if (data & 0x01)
					reflection |= (1u << ((bitWidth - 1) - bit));

				data >>= 1;
			}

			return reflection;
		}
	}
}


CRC_TEMPLATE
uint8_t Crc<CRC_TEMPLATE_PARAM>::reverseData(uint8_t data)
{
	return reversedData == false ? data : crcReverse8(data);
}


CRC_TEMPLATE
SizeType Crc<CRC_TEMPLATE_PARAM>::reverseTable(SizeType remainder)
{
	return reversedData == false ? remainder : reverse(remainder);
}


CRC_TEMPLATE
SizeType Crc<CRC_TEMPLATE_PARAM>::reverseRemainder(SizeType remainder)
{
	return reversedRemainder == false ? remainder : reverse(remainder);
}


CRC_TEMPLATE
void Crc<CRC_TEMPLATE_PARAM>::initialize()
{
	if (isInitialized == false)
	{
		isInitialized = true;

		const SizeType maxBit = 1u << (bitWidth - 1);

		for (uint32_t dividend = 0; dividend < 256; ++dividend)
		{
			SizeType remainder = (uint32_t)reverseData((uint8_t)dividend)
				<< (bitWidth - 8);

			for (int bit = 0; bit < 8; ++bit)
			{
				if (remainder & maxBit)
					remainder = (remainder << 1) ^ polynomial;
				else
					remainder = (remainder << 1);
			}

			table[dividend] = reverseTable(remainder);
		}
	}
}


CRC_TEMPLATE
SizeType Crc<CRC_TEMPLATE_PARAM>::checksum(const char *data, size_t length)
{
	SizeType remainder = initial;

	initialize();

	if (reversedData == false)
	{
		for (size_t byte = 0; byte < length; ++byte)
		{
			uint8_t index = (uint8_t)data[byte] ^
				((uint8_t)remainder >> (bitWidth - 8));
			remainder = table[index] ^ (remainder << 8);
		}
	}
	else
	{
		for (size_t byte = 0; byte < length; ++byte)
		{
			uint8_t index = (uint8_t)data[byte] ^ (uint8_t)remainder;
			remainder = table[index] ^ (remainder >> 8);
		}
	}

	if (8 * sizeof(SizeType) > bitWidth)
		remainder &= ~0u >> (8 * sizeof(SizeType) - bitWidth);

	if (reversedRemainder == false)
		return remainder ^ finalXor;
	else
		return ~remainder ^ finalXor;
}


uint32_t crc32(const char *data, size_t length)
{
	return Crc<uint32_t, 0x04c11db7, ~0u, ~0u, true, false>::
		checksum(data, length);
}


uint16_t crc16(const char *data, size_t length)
{
	return Crc<uint16_t, 0x8005, (uint16_t)~0u, 0, true, false>::
		checksum(data, length);
}
