#ifndef _CHINESETRANSLATER_HPP_
#define _CHINESETRANSLATER_HPP_

#include <cstdint>
#include <string>
#include <map>
#include "file/BufferedFile.hpp"


class ChineseTranslater
{
public:
	typedef std::string ByteString;
	typedef std::string Utf8String;
	typedef std::map<ByteString, Utf8String> Map;

	typedef enum MultiByteRange
	{
		MB_START1 = 0x81,
		MB_END1 = 0xfe,
		MB_START2 = 0x40,
		MB_END2 = 0xfe
	} MultiByteRange;

	typedef enum TranslationType
	{
		BIG5_TO_GBK = 0,
		GBK_TO_BIG5 = 1
	} TranslationType;

	static void initialize();

	static void generateTable();
	static void generateMap();

	static void translateFile(const char *from, const char *to, TranslationType type = GBK_TO_BIG5);

	static Utf8String translate(const char *str, uint32_t length,
		TranslationType type = GBK_TO_BIG5);

	static Utf8String translate(const ByteString &str,
		TranslationType type = GBK_TO_BIG5);

private:
	static void write(BufferedFile &f, uint8_t char1, uint8_t char2);
	static void writeBlock(BufferedFile &f, uint8_t base, uint8_t start, uint8_t end);
	static void writeNull(BufferedFile &f, uint8_t start, uint8_t end);

	static bool isMultiByte1(uint8_t char1) { return char1 >= MB_START1; }
	static bool isMultiByte2(uint8_t char2) { return char2 >= MB_START2; }
	static Map & getMap(TranslationType type)
	{ return type == BIG5_TO_GBK ? big5ToGbkMap : gbkToBig5Map; }

	static const char *gbkToBig5Table[];
	static const char *big5ToGbkTable[];
	static Map big5ToGbkMap;
	static Map gbkToBig5Map;
};

#endif
