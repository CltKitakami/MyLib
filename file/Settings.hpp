#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "common/Convert.hpp"
#include "file/IniFile.hpp"
#include "pattern/Singleton.hpp"


#ifdef __linux__
#define DEFAULT_SETTINGS_FILE_PATH "settings.conf"
#else
#define DEFAULT_SETTINGS_FILE_PATH "settings.ini"
#endif

// #define SETTINGS(member) (Singleton<Settings>::instance()->member)

#define INSTALL(var, section, key, defaultValue) \
do {\
	var.initialize(section, key, defaultValue);\
	this->add(&var);\
} while (0)


class SettingInterface
{
public:
	virtual ~SettingInterface() = default;
	virtual std::string toString() = 0;
	virtual bool load(const std::string &read) = 0;

	std::string section;
	std::string key;
};


template <typename T>
class Setting : public SettingInterface
{
public:
	void initialize(const std::string &section, const std::string &key, const T &defaultValue)
	{
		this->section = section;
		this->key = key;
		this->value = defaultValue;
	}

	operator T & () { return this->value; }

	T & operator = (const T &value)
	{
		this->value = value;
		return *this;
	}

	T value;
};


class BooleanSetting : public Setting<bool>
{
public:
	virtual std::string toString() { return this->value ? "true" : "false"; }

	virtual bool load(const std::string &read)
	{
		if (read == "true")
			this->value = true;
		else if (read == "false")
			this->value = false;
		else
			return false;
		return true;
	}

	operator bool & () { return this->value; }

	bool & operator = (const bool &value)
	{
		this->value = value;
		return *this;
	}
};


class IntSetting : public Setting<int>
{
public:
	virtual std::string toString() { return Convert::toString(this->value); }

	virtual bool load(const std::string &read)
	{
		if (read.length() == 0)
		{
			return false;
		}
		else
		{
			this->value = std::atoi(read.data());
			return true;
		}
	}

	operator int & () { return this->value; }

	int & operator = (const int &value)
	{
		this->value = value;
		return *this;
	}
};


class DoubleSetting : public Setting<double>
{
public:
	virtual std::string toString() { return Convert::toString(this->value); }

	virtual bool load(const std::string &read)
	{
		if (read.length() == 0)
			return false;
		else
		{
			this->value = std::atof(read.data());
			return true;
		}
	}

	operator double & () { return this->value; }

	double & operator = (const double &value)
	{
		this->value = value;
		return *this;
	}
};


class StringSetting : public Setting<std::string>
{
public:
	virtual std::string toString() { return this->value; }

	virtual bool load(const std::string &read)
	{
		if (read.length() == 0)
			return false;
		else
		{
			this->value = read;
			return true;
		}
	}

	operator std::string & () { return this->value; }

	std::string & operator = (const std::string &value)
	{
		this->value = value;
		return *this;
	}
};


class Settings
{
public:
	virtual ~Settings() = default;
	void load(const char *filePath = DEFAULT_SETTINGS_FILE_PATH);
	void save(const char *filePath = DEFAULT_SETTINGS_FILE_PATH);

protected:
	typedef std::vector<SettingInterface *> Container;

	virtual void initialize() = 0;

	void write(IniFile &settings);
	void add(SettingInterface *s) { this->container.push_back(s); }

	Container container;
};

#endif
