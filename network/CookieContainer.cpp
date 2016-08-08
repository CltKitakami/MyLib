#include "CookieContainer.hpp"


void CookieContainer::addFromCookieString(const std::string &str)
{
	std::size_t len = str.length();
	std::size_t idStart = *str.data() != ' ' ? 0 : 1;
	std::size_t equalAt, valueEnd;

	do
	{
		equalAt = str.find('=', idStart);
		valueEnd = str.find("; ", equalAt, 2);

		if (valueEnd == std::string::npos)
			valueEnd = len;

		if (equalAt > idStart && valueEnd > equalAt)
		{
			this->container[std::string(str, idStart, equalAt - idStart)] =
				std::string(str, equalAt + 1, valueEnd - equalAt - 1);
		}

		idStart = valueEnd + 2;
	} while (valueEnd != len);

	this->container.erase("expires");
	this->container.erase("path");
}


std::string CookieContainer::toString()
{
	std::size_t size = this->container.size();
	if (size == 0) return "";

	const std::string split = "; ";
	const std::string equal = "=";
	std::string str;
	str.reserve(size * 20);

	Map::const_iterator begin = this->container.begin();
	Map::const_iterator end = this->container.end();

	str += begin->first;
	str += equal;
	str += begin->second;

	while (++begin != end)
	{
		str += split;
		str += begin->first;
		str += equal;
		str += begin->second;
	}

	return str;
}
