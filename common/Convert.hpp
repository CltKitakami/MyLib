#ifndef _CONVERT_HPP_
#define _CONVERT_HPP_

#include <cstdlib>
#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>


#ifdef _MSC_VER
#define _CONV_TEMPLATE_(var, defaultType) \
	template <typename var>
#define _CONV_TEMPLATE2_(var1, defaultType1, var2, defaultType2) \
	template <typename var1, var2>
#else
#define _CONV_TEMPLATE_(var, defaultType) \
	template <typename var = defaultType>
#define _CONV_TEMPLATE2_(var1, type1, var2, type2) \
	template <typename var1 = type1, typename var2 = type2>
#endif


class Convert
{
public:
#if _MSC_VER <= 1600
	static std::string toString(int value)
	{ return std::to_string((long long)value); }

	static std::string toString(long value)
	{ return std::to_string((long long)value); }

	static std::string toString(long long value)
	{ return std::to_string(value); }

	static std::string toString(unsigned value)
	{ return std::to_string((unsigned long long)value); }

	static std::string toString(unsigned long value)
	{ return std::to_string((unsigned long long)value); }

	static std::string toString(unsigned long long value)
	{ return std::to_string((long double)value); }

	static std::string toString(float value)
	{ return std::to_string((long double)value); }

	static std::string toString(double value)
	{ return std::to_string((long double)value); }

	static std::string toString(long double value)
	{ return std::to_string((long double)value); }

#else
	template <typename T>
	static std::string toString(T value)
	{ return std::to_string(value); }
#endif

	_CONV_TEMPLATE_(T, int)
	static size_t intToString(T value, char *buffer, size_t size);

	_CONV_TEMPLATE_(T, unsigned int)
	static size_t uintToString(T value, char *buffer, size_t size);

	_CONV_TEMPLATE_(T, int)
	static std::string intToString(T value);

	_CONV_TEMPLATE_(T, unsigned int)
	static std::string uintToString(T value);

	static uint8_t inline byteToHex(uint8_t value);
	static uint8_t inline hexToByte(uint8_t value);
	static uint8_t inline getHexBase(uint8_t value);

	_CONV_TEMPLATE_(T, int)
	static std::string uintToHexString(T dec);

	_CONV_TEMPLATE_(T, int)
	static T hexStringToUint(const std::string &hex);

	_CONV_TEMPLATE_(T, int)
	static T stringToInt(const std::string &value);

	_CONV_TEMPLATE_(T, int)
	static T stringToInt(const char *value);

	_CONV_TEMPLATE_(T, unsigned int)
	static T stringToUint(const std::string &value);

	_CONV_TEMPLATE_(T, unsigned int)
	static T stringToUint(const char *value);

	static uint16_t combineDualBytes(char lsb, char msb)
	{ return combineDualBytes((uint8_t)lsb, (uint8_t)msb); }

	static uint16_t combineDualBytes(uint8_t lsb, uint8_t msb)
	{ return uint16_t(uint8_t(lsb)) + ((uint16_t(msb) & 0xff) << 8); }

	static uint32_t combineQuadBytes(char lsb, char b1, char b2, char msb)
	{ return combineQuadBytes((uint8_t)lsb, (uint8_t)b1, (uint8_t)b2, (uint8_t)msb); }

	static uint32_t combineQuadBytes(uint8_t lsb, uint8_t b1, uint8_t b2, uint8_t msb)
	{ return uint32_t(lsb) + ((uint32_t(b1) & 0xff) << 8) +
		((uint32_t(b2) & 0xff) << 16) + ((uint32_t(msb) & 0xff) << 24); }

	static void splitDualBytes(uint16_t value, char *outBytes)
	{ splitDualBytes(value, reinterpret_cast<uint8_t *>(outBytes)); }

	static void splitDualBytes(uint16_t value, uint8_t *outBytes)
	{
		outBytes[0] = uint8_t(value & 0xff);
		outBytes[1] = uint8_t((value >> 8) & 0xff);
	}

	static void splitDualReverseBytes(uint16_t value, char *outBytes)
	{ splitDualReverseBytes(value, reinterpret_cast<uint8_t *>(outBytes)); }

	static void splitDualReverseBytes(uint16_t value, uint8_t *outBytes)
	{
		outBytes[0] = uint8_t((value >> 8) & 0xff);
		outBytes[1] = uint8_t(value & 0xff);
	}

	static void splitQuadBytes(uint32_t value, char *outBytes)
	{ splitQuadBytes(value, reinterpret_cast<uint8_t *>(outBytes)); }

	static void splitQuadBytes(uint32_t value, uint8_t *outBytes)
	{
		outBytes[0] = uint8_t(value & 0xff);
		outBytes[1] = uint8_t((value >> 8) & 0xff);
		outBytes[2] = uint8_t((value >> 16) & 0xff);
		outBytes[3] = uint8_t((value >> 24) & 0xff);
	}

	static void splitQuadReverseBytes(uint32_t value, char *outBytes)
	{ splitQuadReverseBytes(value, reinterpret_cast<uint8_t *>(outBytes)); }

	static void splitQuadReverseBytes(uint32_t value, uint8_t *outBytes)
	{
		outBytes[0] = uint8_t((value >> 24) & 0xff);
		outBytes[1] = uint8_t((value >> 16) & 0xff);
		outBytes[2] = uint8_t((value >> 8) & 0xff);
		outBytes[3] = uint8_t(value & 0xff);
	}

	_CONV_TEMPLATE2_(T1, char, T2, uint32_t)
	static bool isInRange(T1 value, T1 from, T1 to)
	{
		return (T2)(value - from) <= (T2)(to - from);
	}

	static bool isHexChar(char ch)
	{
		return Convert::isInRange(ch, '0', '9') == true
			|| Convert::isInRange(ch, 'A', 'Z') == true
			|| Convert::isInRange(ch, 'a', 'z') == true;
	}
};


template <typename T>
size_t Convert::intToString(T value, char *buffer, size_t size)
{
	bool isNegative = false;

	if (value < 0)
	{
		isNegative = true;
		value = -value;
	}

	size_t index = uintToString(value, buffer, size);

	if (isNegative) buffer[--index] = '-';

	return index;
}


template <typename T>
size_t Convert::uintToString(T value, char *buffer, size_t size)
{
	size_t index = size;

	do
	{
		buffer[--index] = (char)((value % 10) + '0');
		value /= 10;
	} while (value != 0);

	return index;
}


template <typename T>
std::string Convert::intToString(T value)
{
	const size_t size = (sizeof(T) * 8 + 1) / 3;
	char buffer[size + 1];
	size_t index = intToString(value, buffer, size);
	return std::string(&buffer[index], size - index);
}


template <typename T>
std::string Convert::uintToString(T value)
{
	const size_t size = (sizeof(T) * 8 + 1) / 3;
	char buffer[size + 1];
	size_t index = uintToString(value, buffer, size);
	return std::string(&buffer[index], size - index);
}


uint8_t Convert::byteToHex(uint8_t value)
{
	return (uint8_t)"0123456789ABCDEF"[(uint32_t)value & 0xf];
}


uint8_t Convert::hexToByte(uint8_t value)
{
	return (uint8_t)((uint32_t)value - getHexBase(value));
}


uint8_t Convert::getHexBase(uint8_t value)
{
	if (isInRange((char)value, 'A', 'F') == true)
		return (uint32_t)'A' - 10u;
	else if (isInRange((char)value, 'a', 'f') == true)
		return (uint32_t)'a' - 10u;
	else if (isInRange((char)value, '0', '9') == true)
		return (uint32_t)'0';
	else
		return 0;
}


template <typename T>
std::string Convert::uintToHexString(T dec)
{
	std::string result;
	uint32_t shift = sizeof(T) * 2 * 4 - 4;

	for (int i = (int)sizeof(T) * 2; i > 0; --i)
	{
		result += (char)byteToHex((uint8_t)((dec >> shift) & 0xf));
		shift -= 4;
	}

	return result;
}


template <typename T>
T Convert::hexStringToUint(const std::string &hex)
{
	T result = 0;

	for (std::string::const_iterator it = hex.begin(),
		max = hex.begin() + sizeof(T) * 2,
		end = max > hex.end() ? hex.end() : max;
		it != end; ++it)
	{
		uint8_t base = getHexBase((uint8_t)(*it));

		if (base == 0)
		{
			break;
		}
		else
		{
			result <<= 4;
			result += (T)((uint8_t)(*it) - base);
		}
	}

	return result;
}


template <typename T>
T Convert::stringToUint(const std::string &value)
{ return stringToUint<T>(value.data()); }


template <typename T>
T Convert::stringToUint(const char *value)
{
	const char *p = value;
	T result = 0;

	while (isInRange(*p, '0', '9') == true)
	{
		result *= 10;
		result += (T)(uint8_t)(*p - '0');
		p += 1;
	}

	return result;
}


template <typename T>
T Convert::stringToInt(const std::string &value)
{ return stringToInt<T>(value.data()); }


template <typename T>
T Convert::stringToInt(const char *value)
{
	const char *p = *value == '-' ? value + 1 : value;
	T result = stringToUint<T>(p);
	return *value != '-' ? result : -result;
}

#endif
