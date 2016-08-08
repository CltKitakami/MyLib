#include <cstdint>
#include "Base64.hpp"
#include "common/Exception.hpp"


const char Base64::encodeTable[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

const char Base64::decodeTable[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
	0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


std::string Base64::encode(const std::string &data)
{ return Base64::encode(data.data(), data.length()); }


std::string Base64::encode(const char *data, int length)
{ return Base64::encode(data, (size_t)length); }


std::string Base64::encode(const char *data, size_t length)
{
	size_t count = length - 3;
	size_t outLength = (length + 2) / 3 * 4;
	const char *ptr = data;
	std::string out;
	out.resize(outLength);
	char *pOut = &out[0];

	while (true)
	{
		uint32_t a = (uint8_t)(ptr[0]);
		uint32_t b = (uint8_t)(ptr[1]);
		uint32_t c = (uint8_t)(ptr[2]);
		uint32_t triple = (a << 16) + (b << 8) + c;

		uint8_t z = (uint8_t)(triple & 0x3f);
		uint8_t y = (uint8_t)((triple >> 6) & 0x3f);
		uint8_t x = (uint8_t)((triple >> 12) & 0x3f);
		uint8_t w = (uint8_t)((triple >> 18) & 0x3f);

		pOut[0] = encodeTable[w];
		pOut[1] = encodeTable[x];
		pOut[2] = encodeTable[y];
		pOut[3] = encodeTable[z];

		ptr += 3;
		pOut += 4;

		if (count < 3)
			break;
		else
			count -= 3;
	}

	if (count != 0)
	{
		uint32_t remainTriple = 0;
		size_t remainCount = count;
		size_t remainEqual = 3 - count;

		for (size_t i = 0; i < remainCount; ++i)
		{
			uint32_t a = (uint8_t)(*ptr++);
			remainTriple += a << ((2 - i) * 8);
		}

		for (size_t i = 0; i <= remainCount; ++i)
		{
			*pOut++ = encodeTable[(uint8_t)
				((remainTriple >> (6 * (3 - i))) & 0x3f)];
		}

		for (size_t i = remainEqual; i > 0; --i)
			*pOut++ = '=';
	}

	return out;
}


std::string Base64::decode(const std::string &data)
{ return Base64::decode(data.data(), (int)data.length()); }


std::string Base64::decode(const char *data, int length)
{ return Base64::decode(data, (size_t)length); }


std::string Base64::decode(const char *data, size_t length)
{
	if (length % 4 != 0)
		throw Exception(TRACE, "Not base64 data.");

	size_t outLength = length / 4 * 3;
	const char *ptr = data;
	std::string out;
	out.resize(outLength);
	char *pOut = &out[0];

	for (size_t count = length; count != 0; count -= 4)
	{
		uint32_t w = (uint8_t)(ptr[0]);
		uint32_t x = (uint8_t)(ptr[1]);
		uint32_t y = (uint8_t)(ptr[2]);
		uint32_t z = (uint8_t)(ptr[3]);
		uint32_t triple = ((uint32_t)decodeTable[w] << 18) +
			((uint32_t)decodeTable[x] << 12) +
			((uint32_t)decodeTable[y] << 6) +
			(uint32_t)decodeTable[z];

		char c = (char)(triple & 0xff);
		char b = (char)((triple >> 8) & 0xff);
		char a = (char)((triple >> 16) & 0xff);

		pOut[0] = a;
		pOut[1] = b;
		pOut[2] = c;

		ptr += 4;
		pOut += 3;
	}

	if (length > 4)
	{
		if (data[length - 2] ==  '=')
			out.resize(outLength - 2);
		else if (data[length - 1] ==  '=')
			out.resize(outLength - 1);
	}

	return out;
}
