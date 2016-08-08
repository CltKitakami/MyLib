#ifndef _BASE64_HPP_
#define _BASE64_HPP_

#include <string>


class Base64
{
	static const char encodeTable[64];
	static const char decodeTable[256];

public:
	static std::string encode(const std::string &data);
	static std::string encode(const char *data, int length);
	static std::string encode(const char *data, size_t length);

	static std::string decode(const std::string &data);
	static std::string decode(const char *data, int length);
	static std::string decode(const char *data, size_t length);
};

#endif
