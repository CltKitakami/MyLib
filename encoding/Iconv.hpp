#ifndef _ICONV_HPP_
#define _ICONV_HPP_

#include <string>


class Iconv
{
public:
	static std::string translate(std::string &src, const char *fromCharset, const char *toCharset);
	static bool translate(char *in, size_t inLength, char *out, size_t outMaxLength,
		const char *fromCharset, const char *toCharset);
};

#endif
