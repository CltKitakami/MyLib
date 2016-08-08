#ifndef _HEADERCONTAINER_HPP_
#define _HEADERCONTAINER_HPP_

#include <string>
#include <map>


class HeaderContainer
{
public:
	typedef enum DefaultField
	{
		HOST = 0,
		USER_AGENT,
		ACCEPT,
		ACCEPT_LANGUAGE,
		ACCEPT_ENCODING,
		CONTENT_TYPE,
		REFERER,
		CONTENT_LENGTH,
		CONNECTION,
		DEFAULT_FIELD_END
	} DefaultField;

	typedef std::map<std::string, std::string> Map;

	HeaderContainer() = default;
	HeaderContainer(const HeaderContainer &hc) = default;
	HeaderContainer(HeaderContainer &&hc) = default;

	HeaderContainer & operator = (const HeaderContainer &hc) = default;
	HeaderContainer & operator = (HeaderContainer &&hc) = default;

	static std::string findHeader(const std::string &headers, const std::string &id, int startAt = 0);

	std::string toString(const std::string &requestLine, const std::string &cookie);

	std::string defaultFields[DEFAULT_FIELD_END];
	Map customFields;

private:
	inline void addFields(std::string &in, const std::string &id, const std::string &value);
	void defaultFieldsToString(std::string &in);
	void customFieldsToString(std::string &in);
};

#endif
