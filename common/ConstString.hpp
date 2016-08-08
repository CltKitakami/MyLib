#ifndef _CONSTSTRING_HPP_
#define _CONSTSTRING_HPP_

#include <cstdlib>


class ConstString
{
	const char *str;
	size_t length;

public:
	template <size_t N>
	constexpr ConstString(const char (&str)[N]) :
		str(str) , length(N - 1) {}

	const char * getCharArray() const { return this->str; }
	size_t getLength() const { return this->length; }
};

#endif
