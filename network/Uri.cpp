#include <cstring>
#include <cctype>

#include "Uri.hpp"
#include "common/Convert.hpp"


Uri::Uri(const std::string &uri) : type(UNKNOWN)
{
	this->set(uri);
}


Uri & Uri::operator = (const std::string &uri)
{
	this->set(uri); return *this;
}


void Uri::set(const std::string &uri)
{
	if (uri.length() > 0)
	{
		int urlEnd = this->parseUrl(uri);
		this->parseUrn(uri, urlEnd);
	}
}


size_t Uri::parseScheme(const std::string &uri)
{
	const char *p = uri.data();
	size_t startAt = 0;

	if (p[0] == 'h' && p[1] == 't' && p[2] == 't' && p[3] == 'p')
	{
		if (p[4] == ':' && p[5] == '/' && p[6] == '/')
		{
			startAt = 7;
			this->setType(HTTP);
			this->setPort(80);
		}
		else if (p[4] == 's' && p[5] == ':' && p[6] == '/' && p[7] == '/')
		{
			startAt = 8;
			this->setType(HTTPS);
			this->setPort(443);
		}
	}
	else
	{
		this->setType(HTTP);
		this->setPort(80);
	}

	return startAt;
}


int Uri::parseUrl(const std::string &uri)
{
	size_t startAt = this->parseScheme(uri);

	size_t endAt = uri.find('/', startAt);

	if (endAt == std::string::npos)
		endAt = uri.length();

	this->hostName = std::string(uri, startAt, endAt - startAt);

	return (int)endAt;
}


void Uri::parseUrn(const std::string &uri, int urlEnd)
{
	if ((size_t)urlEnd != uri.length())
		this->hostPath = std::string(uri, (size_t)urlEnd);
	else
		this->hostPath = '/';
}


std::string Uri::encode(const std::string &str)
{
	std::string result;
	result.reserve(str.length() * 2);

	for (std::string::const_iterator it = str.begin(),
		end = str.end(); it != end; ++it)
	{
		char ch = *it;

		if (std::isalnum((unsigned)ch) || ch == '-' ||
			ch == '_' || ch == '.' || ch == '~')
		{
			result += ch;
		}
		else
		{
			result += '%';
			result += (char)Convert::byteToHex((uint8_t)((uint32_t)ch >> 4));
			result += (char)Convert::byteToHex((uint8_t)((uint32_t)ch & 0xf));
		}
	}

	return result;
}


std::string Uri::decode(const std::string &str)
{
	int encodeLength = 0;
	uint8_t high = 0, low = 0;
	std::string result;
	result.reserve(str.length());

	for (std::string::const_iterator it = str.begin(),
		end = str.end(); it != end; ++it)
	{
		char ch = *it;

		if (encodeLength == 0)
		{
			if (ch == '%')
				encodeLength = 2;
			else
				result += ch;
		}
		else
		{
			if (--encodeLength == 1)
				high = Convert::hexToByte((uint8_t)ch);
			else
			{
				low = Convert::hexToByte((uint8_t)ch);
				result += (char)((high << 4) + low);
			}
		}
	}

	return result;
}
