#include "Hexdump.hpp"
#include "common/Convert.hpp"


std::streambuf * Hexdump::redirect(std::streambuf *sb)
{
	return out.rdbuf(sb);
}


size_t Hexdump::getAddressWidth(size_t size)
{
	size_t width = 0;

	while (size > 0)
	{
		size >>= 4;
		width += 1;
	}

	return width;
}


void Hexdump::outputAddress(size_t address, size_t width)
{
	char digitals[] = "0000000000000000";
	char *p = &digitals[sizeof(size_t) * 2 - 1];

	while (address > 0)
	{
		*p-- = (char)Convert::byteToHex((uint8_t)address & 0xf);
		address >>= 4;
	}

	out << std::string(&digitals[sizeof(size_t) * 2 - width], width);
}


void Hexdump::outputHex(const char *data, size_t length, size_t chars)
{
	for (size_t i = 0; i < length; ++i)
	{
		out << (char)Convert::byteToHex((uint8_t)data[i] >> 4);
		out << (char)Convert::byteToHex((uint8_t)data[i]);
		out << ' ';
	}

	for (size_t i = length; i < chars; ++i)
		out << "   ";
}


void Hexdump::outputText(const char *data, size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		if (Convert::isInRange(data[i], '\x20', '\x7e') == true)
			out << data[i];
		else
			out << ' ';
	}
}


void Hexdump::output(const std::string &data)
{
	this->output(data.data(), data.length());
}


void Hexdump::output(const char *data, size_t length)
{
	size_t chars = 8;
	size_t width = getAddressWidth(length);
	size_t lines = (length + chars - 1) / chars;

	for (size_t i = 0; i < lines; ++i)
	{
		size_t lineStart = i * chars;
		size_t lineEnd = lineStart + chars;
		size_t lineLength = lineEnd > length ? length - lineStart : chars;
		const char *p = data + lineStart;

		outputAddress(lineStart, width);
		out << "| ";
		outputHex(p, lineLength, chars);
		out << "| ";
		outputText(p, lineLength);
		out << '\n';
	}
}
