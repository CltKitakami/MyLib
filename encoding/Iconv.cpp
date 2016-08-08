#include <cstring>
#include <iconv.h>
#include "Iconv.hpp"


std::string Iconv::translate(std::string &in, const char *fromCharset, const char *toCharset)
{
	std::string out;
	size_t dstLen = in.length() * 2;
	size_t inLen = in.length();
	out.resize(dstLen);
	char *pIn = &in[0];
	char *pOut = &out[0];	// must after out.resize(dstLen);

	iconv_t conv = iconv_open(toCharset, fromCharset);
	int ret = (int)iconv(conv, &pIn, &inLen, &pOut, &dstLen);
	iconv_close(conv);

	if (ret == -1)
	{
		perror("iconv");
		return std::string();
	}
	else
	{
		out.resize(strlen(out.data()));
		return out;
	}
}


bool Iconv::translate(char *in, size_t inLength, char *out, size_t outMaxLength,
		const char *fromCharset, const char *toCharset)
{
	iconv_t conv = iconv_open(toCharset, fromCharset);
	int ret = (int)iconv(conv, &in, &inLength, &out, &outMaxLength);
	iconv_close(conv);
	return ret == -1 ? false : true;
}


/*
#include <windows.h>

typedef enum CodePage
{
#ifdef _WIN32
	CP_SYSTEM = CP_ACP,
#else
	CP_SYSTEM = 950,
#endif
	CP_BIG5 = 950,
	CP_GBK = 936,
	CP_SHIFT_JIF = 932,
} CodePage;

std::string Encoding::utf8ToMultiByte(const std::string &str, CodePage codePage)
{
#ifdef _WIN32

	// UTF-8 to Unicode
	int uniLength = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], str.length(), NULL, 0);
	std::string uniStr;
	uniStr.resize(uniLength);
	wchar_t *wstr = reinterpret_cast<wchar_t *>(&uniStr[0]);
	(void)::MultiByteToWideChar(CP_UTF8, 0, &str[0], str.length(), wstr, uniLength);

	// Unicode to codePage
	const wchar_t *ptr = reinterpret_cast<const wchar_t *>(uniStr.data());
	int length = ::WideCharToMultiByte(codePage, 0, ptr, uniStr.length(), NULL, 0, NULL, 0);
	std::string out;
	out.resize(length);
	(void)::WideCharToMultiByte(codePage, 0, ptr, uniStr.length(), &out[0], length, NULL, 0);

	return out;
#else
	(void)codePage;
	return str;
#endif
}
*/
