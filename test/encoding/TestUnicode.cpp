#include "common/Log.hpp"
#include "encoding/Unicode.hpp"


void printBinary(const char *in, size_t length)
{
	std::cout << std::hex;
	for (size_t i = 0; i < length; ++i)
		std::cout << "\\x" << (uint32_t)(uint8_t)in[i];
	std::cout << std::dec << std::endl;
}

void printUnicode(const uint32_t *in, size_t length)
{
	std::cout << std::hex;
	for (size_t i = 0; i < length; ++i)
		std::cout << "\\u" << in[i];
	std::cout << std::dec << std::endl;
}


int main()
{
	char buf[100];
	char buf2[100];
	uint32_t uni[100];

	char utf8[] = { "\x24\xf0\x90\x90\xb7\xf0\xa4\xad\xa2\xee\x80\x80" };
	char utf16Be[] = { "\x00\x24\xd8\x01\xdc\x37\xd8\x52\xdf\x62\xe0\x00" };
	char utf16Le[] = { "\x24\x00\x01\xd8\x37\xdc\x52\xd8\x62\xdf\x00\xe0" };

	LOG_VAR(utf8);

	{
		LOG.d("original UTF-8");
		printBinary(utf8, sizeof(utf8) - 1);

		size_t uniLen = Unicode::utf8ToUnicode(utf8, sizeof(utf8) - 1, uni, 100);
		LOG.d("to unicode");
		printUnicode(uni, uniLen);

		uniLen = Unicode::unicodeToUtf8(uni, uniLen, buf, 100);
		LOG.d("to UTF-8");
		printBinary(buf, uniLen);
		LOG.d("");
	}

	{
		LOG.d("original UTF-16BE");
		printBinary(utf16Be, sizeof(utf16Be) - 1);

		size_t uniLen = Unicode::utf16ToUnicode(utf16Be, sizeof(utf16Be) - 1, uni, 100, true);
		LOG.d("to unicode");
		printUnicode(uni, uniLen);

		uniLen = Unicode::unicodeToUtf16(uni, uniLen, buf, 100, true);
		LOG.d("to UTF-16BE");
		printBinary(buf, uniLen);
		LOG.d("");
	}

	{
		LOG.d("original UTF-16LE");
		printBinary(utf16Le, sizeof(utf16Le) - 1);

		size_t uniLen = Unicode::utf16ToUnicode(utf16Le, sizeof(utf16Le) - 1, uni, 100, false);
		LOG.d("to unicode");
		printUnicode(uni, uniLen);

		uniLen = Unicode::unicodeToUtf16(uni, uniLen, buf, 100, false);
		LOG.d("to UTF-16LE");
		printBinary(buf, uniLen);
		LOG.d("");
	}

	{
		size_t uniLen = Unicode::utf16ToUntf8(utf16Be, sizeof(utf16Be) - 1, buf, 100, true);
		LOG.d("UTF-16BE to UTF-8");
		printBinary(buf, uniLen);
		uniLen = Unicode::utf8ToUtf16(buf, uniLen, buf2, 100, true);
		LOG.d("UTF-8 to UTF-16BE");
		printBinary(buf2, uniLen);
	}

	return 0;
}
