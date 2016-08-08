#include "Settings.hpp"
#include "common/Log.hpp"


void Settings::write(IniFile &settings)
{
	for (Container::iterator it = container.begin();
		it != container.end(); ++it)
	{
		SettingInterface *s = *it;
		settings.insert(s->section, s->key, s->toString());
	}
}


void Settings::load(const char *filePath)
{
	IniFile settings(filePath);

	this->initialize();

	try
	{
		settings.load();
	}
	catch (...)
	{
		LOG.e(TRACE, "Load settings.ini failed.");
		return;
	}

	for (Container::iterator it = container.begin();
		it != container.end(); ++it)
	{
		SettingInterface *s = *it;

		if (s->load(settings.read(s->section, s->key)) == false)
		{
			LOG.e(TRACE, Log::format("Load [%s][%s] failed.",
				s->section.data(), s->key.data()));
		}
	}
}


void Settings::save(const char *filePath)
{
	IniFile settings(filePath);
	this->write(settings);
	settings.save();
}

