#include "HeaderContainer.hpp"
#include "common/ConstString.hpp"


std::string HeaderContainer::findHeader(const std::string &headers, const std::string &id, int startAt)
{
	std::size_t at = headers.find(id, (std::size_t)startAt);
	if (at != std::string::npos)
	{
		ConstString str(": ");
		std::size_t valueAt = at + id.length() + str.getLength();
		return headers.substr(valueAt, headers.find('\r', valueAt) - valueAt);
	}
	else
		return std::string();
}


void HeaderContainer::addFields(std::string &in, const std::string &id, const std::string &value)
{
	in += id;
	in += ": ";
	in += value;
	in += "\r\n";
}


void HeaderContainer::defaultFieldsToString(std::string &in)
{
	const std::string id[DEFAULT_FIELD_END] =
	{
		"Host",
		"User-Agent",
		"Accept",
		"Accept-Language",
		"Accept-Encoding",
		"Content-Type",
		"Referer",
		"Content-Length",
		"Connection"
	};

	for (int i = 0; i < DEFAULT_FIELD_END; ++i)
	{
		if (this->defaultFields[i].length() > 0)
			addFields(in, id[i], this->defaultFields[i]);
	}
}


void HeaderContainer::customFieldsToString(std::string &in)
{
	for (Map::const_iterator it = this->customFields.begin();
		it != this->customFields.end(); ++it)
	{
		addFields(in, it->first, it->second);
	}
}


std::string HeaderContainer::toString(const std::string &requestLine, const std::string &cookie)
{
	std::string str(requestLine);
	str.reserve((DEFAULT_FIELD_END + customFields.size()) * 32);

	this->defaultFieldsToString(str);

	if (cookie.length() > 0)
	{
		str += "Cookie: ";
		str += cookie;
		str += "\r\n";
	}

	this->customFieldsToString(str);

	str += "\r\n";

	return str;
}
