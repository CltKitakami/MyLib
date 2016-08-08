#include <iostream>
#include <cstdlib>

#include "common/Log.hpp"
#include "common/String.hpp"
#include "common/Stopwatch.hpp"
#include "common/Exception.hpp"
#include "file/File.hpp"
#include "file/IniFile.hpp"


// #define TEST_FILE


int main()
{
	LOG.setFileInfo(false);

	try
	{
#ifdef TEST_FILE
		std::string path("E:\\file 1.txt");
		File wf(path, File::WRITE);
		wf.write("123\r\n");
		wf.writeLine("456");
		wf.close();

		std::string buffer;
		File rf(path, File::READ);

		if (rf.isOpened() == true)
		{
			while (true)
			{
				if (rf.readLine(buffer) > 0)
					LOGT("read: " << buffer);
				else if (rf.isEnd() == true)
					break;
			}
		}
		else
			LOGT("File not exist.");

		rf.close();
#else
		IniFile iniFile("file.ini");
		LOGT("===== insert =====");
		iniFile.insert("Section3", "Key1", "1");
		iniFile.insert("Section4", "Key2", "#");
		iniFile.insert("Section1", "Key5", "$");
		iniFile.insert("Section1", "Key3", "(");
		iniFile.insert("Section2", "Key7", "@");
		iniFile.save();
		iniFile.print();
		LOGT("===== load =====");
		iniFile.load();
		LOGT("===== Read =====");
		LOG_VAR(iniFile["section2"]["key7"]);
		LOG_VAR(iniFile["section4"]["key2"]);
		LOG_VAR(iniFile["section1"]["key5"]);
		LOG_VAR(iniFile["section1"]["key3"]);
		LOG_VAR(iniFile["section3"]["key1"]);

		IniFile::Sections::iterator findAt = iniFile.at("empty?");
		if (findAt == iniFile.end())
			LOGT("Can not find the section.");
		else
			LOG_VAR(iniFile["empty?"]["empty?"]);

		IniFile::Sections::iterator findAt1 = iniFile.at("section1");
		if (findAt1 == iniFile.end())
			LOGT("Can not find the section.");
		else
			LOG_VAR(iniFile.read("section1", "key3"));
#endif

	}
	catch (Exception ex)
	{
		LOGT(ex.getMessage());
	}
	return 0;
}
