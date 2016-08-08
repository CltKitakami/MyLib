#include <cstring>
#include "Unicode.hpp"


#define CHAR_TO_UINT(c) ((uint32_t)((uint8_t)(c)))


size_t Unicode::getUtf8DecodeBytes(uint32_t byte1)
{
	if (byte1 < 0xe0u)
		return byte1 < 0x80u ? 1 : 2;
	else
	{
		if (byte1 < 0xf8u)
			return byte1 < 0xf0u ? 3 : 4;
		else
			return byte1 < 0xfcu ? 5 : 6;
	}
}


size_t Unicode::getUtf8EncodeBytes(uint32_t unit)
{
	if (unit < 0x800u)
		return unit < 0x80 ? 1 : 2;
	else
	{
		if (unit < 0x200000u)
			return unit < 0x10000u ? 3 : 4;
		else
			return unit < 0x4000000u ? 5 : 6;
	}
}


uint32_t Unicode::utf8Decode(size_t bytes, const char *in)
{
	uint32_t result = CHAR_TO_UINT(in[0]) & (0xffu >> bytes);

	for (size_t j = 1; j < bytes; ++j)
	{
		result <<= 6;
		result += CHAR_TO_UINT(in[j]) & 0x3fu;	// without check 0x10xxxxx
	}

	return result;
}


void Unicode::utf8Encode(uint32_t unit, size_t bytes, char *out)
{
	if (bytes == 1)
	{
		out[0] = (char)unit;
	}
	else
	{
		out[0] = (char)((unit >> (6 * (bytes - 1))) +
			(~(0x7fu >> (bytes - 1)) & 0xffu));

		for (size_t j = 1; j < bytes; ++j)
			out[j] = ((unit >> (6 * (bytes - j - 1))) & 0x3fu) + 0x80u;
	}
}


size_t Unicode::utf8ToUnicode(const char *in, size_t inLength,
	uint32_t *out, size_t outMaxLength)
{
	size_t outLength = 0;

	for (size_t i = 0; i < inLength && outLength < outMaxLength;)
	{
		uint32_t byte1 = CHAR_TO_UINT(in[i]);
		size_t bytes = getUtf8DecodeBytes(byte1);

		if (i + bytes > inLength)
			break;

		out[outLength++] = utf8Decode(bytes, &in[i]);;
		i += bytes;
	}

	if (outLength < outMaxLength)
		out[outLength] = 0;

	return outLength;
}


Unicode::UniString Unicode::utf8ToUnicode(const std::string &in)
{
	size_t bufferSize = in.length();
	UniString out;
	out.resize(bufferSize);

	size_t size = Unicode::utf8ToUnicode(in.data(), in.length(),
		&out[0], bufferSize);

	out.resize(size);

	return out;
}


size_t Unicode::unicodeToUtf8(const uint32_t *in, size_t inLength,
	char *out, size_t outMaxLength)
{
	size_t outLength = 0;

	for (size_t i = 0; i < inLength && outLength < outMaxLength; ++i)
	{
		uint32_t unit = in[i];
		size_t bytes = getUtf8EncodeBytes(unit);

		if (outLength + bytes > outMaxLength)
			break;

		utf8Encode(unit, bytes, &out[outLength]);
		outLength += bytes;
	}

	if (outLength < outMaxLength)
		out[outLength] = 0;

	return outLength;
}


std::string Unicode::unicodeToUtf8(const UniString &in)
{
	size_t bufferSize = in.length() * 6;
	std::string out;
	out.resize(bufferSize);

	size_t size = Unicode::unicodeToUtf8(in.data(), in.length(),
		&out[0], bufferSize);

	out.resize(size);

	return out;
}


uint32_t Unicode::utf16CombineDualBytes(char byte1, char byte2)
{
	return (CHAR_TO_UINT(byte1) << 8) + CHAR_TO_UINT(byte2);
}


uint32_t Unicode::utf16CombineDualUnits(uint32_t unit1, uint32_t unit2)
{
	// without check
	// 0xd800 <= uint1 <= 0xdbff
	// 0xdc00 <= uint2 <= 0xdfff
	return ((unit1 & 0x3ffu) << 10) + (unit2 & 0x3ffu) + 0x10000u;
}


size_t Unicode::getUtf16EncodeBytes(uint32_t unit)
{
	if (unit <= 0xdfffu)
		return unit <= 0xd7ffu ? 2 : 0;
	else
	{
		if (unit <= 0xffffu)
			return 2;
		else
			return unit <= 0x10ffffu ? 4 : 0;
	}
}


void Unicode::utf16Encode(uint32_t unit, size_t bytes, char *out, bool isBigEndian)
{
	size_t byte1 = isBigEndian == true ? 0 : 1;
	size_t byte2 = isBigEndian == true ? 1 : 0;

	if (bytes == 4)
	{
		unit -= 0x10000u;
		uint32_t unit1 = unit >> 10;
		uint32_t unit2 = unit & 0x3ffu;
		out[byte1] = (char)((unit1 >> 8) + 0xd8u);
		out[byte2] = (char)(unit1 & 0xffu);
		out[2 + byte1] = (char)((unit2 >> 8) + 0xdcu);
		out[2 + byte2] = (char)(unit2 & 0xffu);
	}
	else
	{
		out[byte1] = (char)(unit >> 8);
		out[byte2] = (char)(unit & 0xffu);
	}
}


size_t Unicode::utf16ToUnicode(const char *in, size_t inLength,
	uint32_t *out, size_t outMaxLength, bool isBigEndian)
{
	size_t outLength = 0;
	size_t byte1 = isBigEndian == true ? 0 : 1;
	size_t byte2 = isBigEndian == true ? 1 : 0;

	for (size_t i = 0; i < inLength - 1 && outLength < outMaxLength; i += 2)
	{
		uint32_t unit1 = utf16CombineDualBytes(in[i + byte1], in[i + byte2]);

		if (unit1 >= 0xd800u && unit1 <= 0xdfffu)
		{
			if (i + 4 <= inLength)
			{
				i += 2;
				uint32_t unit2 = utf16CombineDualBytes(in[i + byte1], in[i + byte2]);
				out[outLength++] = utf16CombineDualUnits(unit1, unit2);
			}
			else
			{
				break;
			}
		}
		else
		{
			out[outLength++] = unit1;
		}
	}

	if (outLength + 1 < outMaxLength)
		out[outLength] = out[outLength + 1] = 0;

	return outLength;
}


Unicode::UniString Unicode::utf16ToUnicode(const std::string &in, bool isBigEndian)
{
	size_t bufferSize = in.length();
	UniString out;
	out.resize(bufferSize);

	size_t size = Unicode::utf16ToUnicode(in.data(), in.length(),
		&out[0], bufferSize, isBigEndian);

	out.resize(size);

	return out;
}


size_t Unicode::unicodeToUtf16(const uint32_t *in, size_t inLength,
	char *out, size_t outMaxLength, bool isBigEndian)
{
	size_t outLength = 0;

	for (size_t i = 0; i < inLength && outLength < outMaxLength; ++i)
	{
		uint32_t bytes = getUtf16EncodeBytes(in[i]);

		if (outLength + bytes > outMaxLength || bytes == 0)
			break;

		utf16Encode(in[i], bytes, &out[outLength], isBigEndian);
		outLength += bytes;
	}

	if (outLength + 1 < outMaxLength)
		out[outLength] = out[outLength + 1] = 0;

	return outLength;
}


std::string Unicode::unicodeToUtf16(const UniString &in, bool isBigEndian)
{
	size_t bufferSize = in.length();
	std::string out;
	out.resize(bufferSize);

	size_t size = Unicode::unicodeToUtf16(in.data(), in.length(),
		&out[0], bufferSize, isBigEndian);

	out.resize(size);

	return out;
}


size_t Unicode::utf16ToUtf8(const char *in, size_t inLength,
	char *out, size_t outMaxLength, bool isBigEndian)
{
	size_t outLength = 0;
	size_t byte1 = isBigEndian == true ? 0 : 1;
	size_t byte2 = isBigEndian == true ? 1 : 0;

	for (size_t i = 0; i < inLength - 1 && outLength < outMaxLength; i += 2)
	{
		uint32_t unit = 0;
		uint32_t unit1 = utf16CombineDualBytes(in[i + byte1], in[i + byte2]);

		if (unit1 >= 0xd800u && unit1 <= 0xdfffu)
		{
			if (i + 4 <= inLength)
			{
				i += 2;
				uint32_t unit2 = utf16CombineDualBytes(in[i + byte1], in[i + byte2]);
				unit = utf16CombineDualUnits(unit1, unit2);
			}
			else
			{
				break;
			}
		}
		else
		{
			unit = unit1;
		}

		size_t bytes = getUtf8EncodeBytes(unit);

		if (outLength + bytes > outMaxLength)
			break;

		utf8Encode(unit, bytes, &out[outLength]);
		outLength += bytes;
	}

	if (outLength < outMaxLength)
		out[outLength] = 0;

	return outLength;
}


std::string Unicode::utf16ToUtf8(const std::string &in, bool isBigEndian)
{
	size_t bufferSize = in.length() * 4;
	std::string out;
	out.resize(bufferSize);

	size_t size = Unicode::utf16ToUtf8(in.data(), in.length(),
		&out[0], bufferSize, isBigEndian);

	out.resize(size);

	return out;
}


size_t Unicode::utf8ToUtf16(const char *in, size_t inLength,
	char *out, size_t outMaxLength, bool isBigEndian)
{
	size_t outLength = 0;

	for (size_t i = 0; i < inLength && outLength < outMaxLength;)
	{
		uint32_t byte1 = CHAR_TO_UINT(in[i]);
		size_t utf8Bytes = getUtf8DecodeBytes(byte1);

		if (i + utf8Bytes > inLength)
			break;

		uint32_t unit = utf8Decode(utf8Bytes, &in[i]);;
		i += utf8Bytes;

		uint32_t utf16Bytes = getUtf16EncodeBytes(unit);

		if (outLength + utf16Bytes > outMaxLength || utf16Bytes == 0)
			break;

		utf16Encode(unit, utf16Bytes, &out[outLength], isBigEndian);
		outLength += utf16Bytes;
	}

	if (outLength + 1 < outMaxLength)
		out[outLength] = out[outLength + 1] = 0;

	return outLength;
}


std::string Unicode::utf8ToUtf16(const std::string &in, bool isBigEndian)
{
	size_t bufferSize = in.length() * 6;
	std::string out;
	out.resize(bufferSize);

	size_t size = Unicode::utf8ToUtf16(in.data(), in.length(),
		&out[0], bufferSize, isBigEndian);

	out.resize(size);

	return out;
}
