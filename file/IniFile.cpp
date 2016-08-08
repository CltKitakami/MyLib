#include "IniFile.hpp"
#include "common/Exception.hpp"
#include "common/String.hpp"
#include "file/File.hpp"


void IniFile::load()
{
	String buffer, section, key, value;
	File file(this->path, File::READ);

	if (file.isOpened() == false)
	{
		file.close();
		throw Exception("IniFile: Load: The file hasn't been isOpened");
	}

	while (file.isEnd() == false)
	{
		if (file.readLine(buffer.inner()) <= 0)
			continue;

		size_t commentCharAt = buffer.indexOf(';');

		if (commentCharAt != ~0u)
			String::substring(buffer, 0, commentCharAt);

		String::trim(buffer);

		if (buffer.getLength() <= 2)
			continue;

		if (parseSection(buffer, section) == false &&
			// If it is not at a section
			parseKeyAndValue(buffer, key, value) == true)
			// and can find a key and vlaue
		{
			this->insert(section.inner(), key.inner(), value.inner());
		}
	}
}


void IniFile::save()
{
	File file(this->path, File::WRITE);

	if (file.isOpened() == true)
	{
		for (Sections::iterator it = this->content.begin();
			it != this->content.end(); ++it)
		{
			Keys &keys = it->keys;
			file.write('[');
			file.write(it->name);
			file.write("]\n", 2);
			for (Keys::iterator j = keys.begin(); j != keys.end(); ++j)
			{
				file.write(j->first);
				file.write('=');
				file.write(j->second);
				file.write('\n');
			}
		}
	}
	else
		throw Exception("IniFile: Save: The file hasn't been isOpened");
}


void IniFile::print(std::ostream &stm)
{
	for (Sections::iterator it = this->content.begin();
		it != this->content.end(); ++it)
	{
		Keys &keys = it->keys;
		stm << '[' << it->name << ']' << std::endl;
		for (Keys::iterator j = keys.begin(); j != keys.end(); ++j)
			stm << j->first << '=' << j->second << std::endl;
	}
}


IniFile::Sections::iterator IniFile::at(const std::string &section)
{
	for (Sections::iterator it = this->content.begin();
		 it != this->content.end(); it++)
	{
		if (String::equalsIgnoreCase(
			it->name.data(), section.data()) == true)
			return it;
	}

	return this->content.end();
}


IniFile::Keys & IniFile::read(const std::string &section)
{
	Sections::iterator findAt = this->at(section);

	if (findAt == this->content.end())
		throw Exception(TRACE, "Can not find: " + section);

	return (*findAt).keys;
}


std::string IniFile::read(const std::string &section, const std::string &key)
{ return (*this)[section][key]; }


void IniFile::insert(const char *section,
	const char *key, const char *value)
{
	this->insert(std::string(section),
		std::string(key), std::string(value));
}


void IniFile::insert(const std::string &section,
	const std::string &key, const std::string &value)
{
	Sections::iterator findAt = this->at(section);

	if (findAt == this->content.end())
		this->content.push_back(Section(section, key, value));
	else
		findAt->keys[key] = value;
}


void IniFile::remove(const char *section)
{ this->remove(std::string(section)); }


void IniFile::remove(const char *section, const char *key)
{ this->remove(std::string(section), std::string(key)); }


void IniFile::remove(const std::string &section)
{
	Sections::iterator findAt = this->at(section);

	if (findAt == this->content.end())
		this->content.erase(findAt);
}


void IniFile::remove(const std::string &section, const std::string &key)
{ (*this)[section].erase(key); }


void IniFile::findSection(const String &str, size_t &start, size_t &end)
{
	std::string::const_iterator itStart = str.begin();
	std::string::const_iterator itEnd = str.end();
	std::string::const_iterator it, itPre, itPost;
	it = itPre = itPost = itStart;

	while (it != itEnd)
	{
		if ((char)*it == '[')
		{
			itPre = ++it;
			break;
		}
		else if ((char)*it == '=')
			return;

		++it;
	}

	if (it != itEnd)
	{
		it = itEnd;
		while (--it != itPre)
		{
			if ((char)*it == ']')
			{
				itPost = it;
				break;
			}
		}
	}

	start = (size_t)(itPre - itStart);
	end = (size_t)(itPost - itStart);
}


bool IniFile::parseSection(const String &line, String &section)
{
	size_t sectionStartAt = 0, sectionEndAt = 0;

	this->findSection(line, sectionStartAt, sectionEndAt);

	if (sectionStartAt < sectionEndAt)
	{
		section = line.substring(sectionStartAt,
			sectionEndAt - sectionStartAt);
		String::trim(section);
		return true;
	}
	else
		return false;
}


bool IniFile::parseKeyAndValue(const String &line,
	String &key, String &value)
{
	size_t equalAt = line.indexOf('=');

	if (equalAt != ~0u)
	{
		key = line.substring((size_t)0, equalAt);
		value = line.substring(equalAt + 1);

		String::trim(key);
		String::trim(value);

		return true;
	}
	else
		return false;
}
