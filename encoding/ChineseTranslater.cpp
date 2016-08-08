#include "ChineseTranslater.hpp"
#include "common/Log.hpp"


#define NULL_CHAR "\xe2\x96\xa1"

/// http://www.khngai.com/chinese/charmap/index.php

const char *ChineseTranslater::gbkToBig5Table[] =
{
#ifndef _MSC_VER
#include "gbkToBig5Table.h"
#endif
	nullptr
};

const char *ChineseTranslater::big5ToGbkTable[] =
{
#ifndef _MSC_VER
#include "big5ToGbkTable.h"
#endif
	nullptr
};

ChineseTranslater::Map ChineseTranslater::big5ToGbkMap;
ChineseTranslater::Map ChineseTranslater::gbkToBig5Map;


void ChineseTranslaterSelfTest()
{
	ChineseTranslater::generateTable();
	ChineseTranslater::generateMap();
	ChineseTranslater::translateFile("big5Table.txt", "big5ToGbkUtf8.txt", ChineseTranslater::BIG5_TO_GBK);
	ChineseTranslater::translateFile("gbkTable.txt", "gbkToBig5Utf8.txt", ChineseTranslater::GBK_TO_BIG5);
}


void ChineseTranslater::write(BufferedFile &f,
	uint8_t char1, uint8_t char2)
{
	f.write('\"');
	f.write((char)char1);
	f.write((char)char2);
	f.write("\",");
}


void ChineseTranslater::writeBlock(BufferedFile &f,
	uint8_t base, uint8_t start, uint8_t end)
{
	for (uint8_t j = start; j <= end; ++j)
	{
		if ((j & 0xf) == 0)
			f.write('\n');
		else
			f.write(' ');
		ChineseTranslater::write(f, base, j);
	}
}


void ChineseTranslater::writeNull(BufferedFile &f,
	uint8_t start, uint8_t end)
{
	for (uint8_t j = start; j <= end; ++j)
	{
		if ((j & 0xf) == 0)
			f.write("\n \"\" ,");
		else
			f.write("  \"\" ,");
	}
}


void ChineseTranslater::initialize()
{
	ChineseTranslater::generateMap();
}


void ChineseTranslater::generateTable()
{
	{
		BufferedFile f("gbkTable.txt", File::BINARY_WRITE);

		for (uint8_t i = MB_START1; i <= MB_END1; ++i)
		{
			ChineseTranslater::writeBlock(f, i, MB_START2, 0x7e);
			ChineseTranslater::writeNull(f, 0x7e + 1, 0x80 - 1);
			ChineseTranslater::writeBlock(f, i, 0x80, MB_END2);
			f.write("  \"\" ,");
		}
	}
	{
		BufferedFile f("big5Table.txt", File::BINARY_WRITE);

		for (uint8_t i = MB_START1; i <= MB_END1; ++i)
		{
			ChineseTranslater::writeBlock(f, i, MB_START2, 0x7e);
			ChineseTranslater::writeNull(f, 0x7e + 1, 0xa1 - 1);
			ChineseTranslater::writeBlock(f, i, 0xa1, MB_END2);
			f.write("  \"\" ,");
		}
	}
}


void ChineseTranslater::generateMap()
{
	char chinese[3] = { 0, 0, 0 };
	int tableIndex = 0;
	ByteString byteStr;
	Utf8String utf8Big5;
	Utf8String utf8Gbk;

	for (uint8_t i = MB_START1; i <= MB_END1; ++i)
	{
		chinese[0] = (char)i;

		for (uint8_t j = MB_START2; j <= MB_END2; ++j)
		{
			chinese[1] = (char)j;
			byteStr.assign(chinese, 2);
			utf8Big5.assign(gbkToBig5Table[tableIndex]);
			utf8Gbk.assign(big5ToGbkTable[tableIndex]);
			gbkToBig5Map[byteStr] = utf8Big5;
			big5ToGbkMap[byteStr] = utf8Gbk;
			++tableIndex;
		}

		++tableIndex;
		// prevent from overflow(0xff) by skipping uint8_t less operator
	}
}


void ChineseTranslater::translateFile(const char *from, const char *to,
	TranslationType type)
{
	BufferedFile rf(from, File::BINARY_READ);
	BufferedFile wf(to, File::BINARY_WRITE);

	if (rf.isOpened() == false)
	{
		LOG.e(Log::format("Can't open %s", from));
		return;
	}
	else if (wf.isOpened() == false)
	{
		LOG.e(Log::format("Can't open %s", to));
		return;
	}

	Map &map = ChineseTranslater::getMap(type);
	Utf8String converted;
	BufferedFile::Iterator it = rf.begin();
	BufferedFile::Iterator end = rf.end();
	char chinese[3] = { 0, 0, 0 };

	for (; it != end; ++it)
	{
		chinese[0] = (char)*it;

		if (isMultiByte1((uint8_t)chinese[0]) == true)
		{
			if (++it != end)
			{
				chinese[1] = (char)*it;

				if (isMultiByte2((uint8_t)chinese[1]) == true)
				{
					converted = map[ByteString(chinese, 2)];

					if (converted.length() > 0)
						wf.write(converted);
					else
						wf.write(NULL_CHAR);
				}
				else
				{
					wf.write(chinese[0]);
					wf.write(chinese[1]);
				}
			}
			else
			{
				wf.write(chinese[0]);
			}
		}
		else
		{
			wf.write(chinese[0]);
		}
	}
}


ChineseTranslater::Utf8String ChineseTranslater::translate(
	const char *str, uint32_t length, TranslationType type)
{
	Map &map = ChineseTranslater::getMap(type);
	const char *it = str, *end = str + length;
	char chinese[3] = { 0, 0, 0 };
	Utf8String converted, out;
	out.reserve(length);

	for (; it != end; ++it)
	{
		chinese[0] = *it;

		if (isMultiByte1((uint8_t)chinese[0]) == true)
		{
			if (++it != end)
			{
				chinese[1] = *it;

				if (isMultiByte2((uint8_t)chinese[1]) == true)
				{
					converted = map[ByteString(chinese, 2)];

					if (converted.length() > 0)
						out.append(converted);
					else
						out.append(NULL_CHAR);
				}
				else
				{
					out.push_back(chinese[0]);
					out.push_back(chinese[1]);
				}
			}
			else
			{
				out.push_back(chinese[0]);
			}
		}
		else
		{
			out.push_back(chinese[0]);
		}
	}

	return out;
}


ChineseTranslater::Utf8String ChineseTranslater::translate(
	const ByteString &str, TranslationType type)
{
	return ChineseTranslater::translate(str.data(), str.length(), type);
}
