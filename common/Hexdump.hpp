#ifndef _HEXDUMP_HPP_
#define _HEXDUMP_HPP_

#include <iostream>
#include <string>


class Hexdump
{
public:
	Hexdump() : out(std::cout.rdbuf()) {}

	std::streambuf * redirect(std::streambuf *sb);

	void output(const char *data, size_t length);
	void output(const std::string &data);

private:
	size_t getAddressWidth(size_t size);
	void outputAddress(size_t address, size_t width);
	void outputHex(const char *data, size_t length, size_t chars);
	void outputText(const char *data, size_t length);

	std::ostream out;
};

#endif
