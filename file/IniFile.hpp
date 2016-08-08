#ifndef _INIFILE_HPP_
#define _INIFILE_HPP_

#include <map>
#include <vector>
#include <functional>
#include <iostream>

#include "common/String.hpp"


class IniFile
{
public:
	struct IgnoreCase :
		public std::function<bool(std::string, std::string)>
	{
		bool operator () (const std::string &lhs, const std::string &rhs) const
		{ return String::compareIgnoreCase(lhs, rhs) < 0; }
	};

	typedef std::map<std::string, std::string, IgnoreCase> Keys;

	typedef struct Section
	{
		std::string name;
		Keys keys;

		Section() {}
		Section(const std::string &name) : name(name) {}
		Section(const std::string &name, const std::string &key, const std::string &value) :
			name(name) { keys.insert(std::make_pair(key, value)); }
	} Section;

	typedef std::vector<Section> Sections;


	IniFile() {}
	IniFile(const char *path) : path(path) {}
	IniFile(const std::string &path) : path(path) {}
	virtual ~IniFile() { this->content.clear(); }

	void load();
	void save();

	void print(std::ostream &stm = std::cout);

	Sections::iterator at(const std::string &section);

	Keys & read(const std::string &section);
	std::string read(const std::string &section, const std::string &key);

	void insert(const char *section, const char *key, const char *value);
	void insert(const std::string &section, const std::string &key, const std::string &value);

	void remove(const char *section);
	void remove(const char *section, const char *key);
	void remove(const std::string &section);
	void remove(const std::string &section, const std::string &key);

	Keys & operator [] (const std::string &section) { return this->read(section); }

	Sections::iterator begin() { return this->content.begin(); }
	Sections::iterator end() { return this->content.end(); }
	Sections::const_iterator begin() const { return this->content.begin(); }
	Sections::const_iterator end() const { return this->content.end(); }

	Sections content;
private:
	std::string path;

	void findSection(const String &str, size_t &start, size_t &end);
	bool parseSection(const String &line, String &section);
	bool parseKeyAndValue(const String &line, String &key, String &value);
};

#endif
