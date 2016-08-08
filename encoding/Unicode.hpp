#ifndef _UNICODE_HPP_
#define _UNICODE_HPP_

#include <string>
#include <cstdint>


class Unicode
{
public:
	typedef std::basic_string<uint32_t> UniString;

	static size_t utf8ToUnicode(const char *in, size_t inLength,
		uint32_t *out, size_t outMaxLength);

	static UniString utf8ToUnicode(const std::string &in);

	static size_t unicodeToUtf8(const uint32_t *in, size_t inLength,
		char *out, size_t outMaxLength);

	static std::string unicodeToUtf8(const UniString &in);

	static size_t utf16ToUnicode(const char *in, size_t inLength,
		uint32_t *out, size_t outMaxLength, bool isBigEndian);

	static UniString utf16ToUnicode(const std::string &in, bool isBigEndian);

	static size_t unicodeToUtf16(const uint32_t *in, size_t inLength,
		char *out, size_t outMaxLength, bool isBigEndian);

	static std::string unicodeToUtf16(const UniString &in, bool isBigEndian);

	static size_t utf16ToUtf8(const char *in, size_t inLength,
		char *out, size_t outMaxLength, bool isBigEndian);

	static std::string utf16ToUtf8(const std::string &in, bool isBigEndian);

	static size_t utf8ToUtf16(const char *in, size_t inLength,
		char *out, size_t outMaxLength, bool isBigEndian);

	static std::string utf8ToUtf16(const std::string &in, bool isBigEndian);

private:
	static size_t getUtf8DecodeBytes(uint32_t byte1);
	static size_t getUtf8EncodeBytes(uint32_t unit);
	static uint32_t utf8Decode(size_t bytes, const char *in);
	static void utf8Encode(uint32_t unit, size_t bytes, char *out);
	static uint32_t utf16CombineDualBytes(char byte1, char byte2);
	static uint32_t utf16CombineDualUnits(uint32_t unit1, uint32_t unit2);
	static size_t getUtf16EncodeBytes(uint32_t unit);
	static void utf16Encode(uint32_t unit, size_t bytes, char *out, bool isBigEndian);
};

#endif
