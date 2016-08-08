#ifndef _COOKIECONTAINER_HPP_
#define _COOKIECONTAINER_HPP_

#include <map>
#include <string>


class CookieContainer
{
	typedef std::map<std::string, std::string> Map;

	Map container;

public:
	CookieContainer() = default;
	CookieContainer(const CookieContainer &cc) = default;
	CookieContainer(CookieContainer &&cc) = default;

	CookieContainer & operator = (const CookieContainer &cc) = default;
	CookieContainer & operator = (CookieContainer &&cc) = default;

	inline void add(const std::string &id, const std::string &value)
	{ this->container[id] = value; }

	inline void remove(const std::string &id)
	{ this->container.erase(id); }

	void addFromCookieString(const std::string &str);

	inline void reset() { this->container.clear(); }

	std::string toString();
};

#endif
