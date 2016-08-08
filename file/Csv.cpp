#include "Csv.hpp"
#include "file/File.hpp"
#include "common/String.hpp"


Csv::Csv(const char *path)
{
	this->load(path);
}


void Csv::clear()
{
	this->data.clear();
}


void Csv::load(const char *path)
{
	File file(path, File::READ);
	std::string line;

	this->clear();

	while (file.isEnd() == false)
	{
		file.readLine(line);

		if (line.length() > 0)
		{
			size_t begin = 0, commaAt = line.find(',');
			Line &v = this->putLine();

			while (commaAt != std::string::npos)
			{
				String &&part = line.substr(begin, commaAt - begin);
				v.push_back(part.trim().inner());
				begin = commaAt + 1;
				commaAt = line.find(',', begin);
			}

			if (begin != 0)
			{
				String &&part = line.substr(begin);
				v.push_back(part.trim().inner());
			}
		}
	}
}


void Csv::save(const char *path)
{
	File file(path, File::WRITE);

	for (size_t i = 0; i < this->getLines(); ++i)
	{
		const Line &v = this->getLine(i);

		for (size_t j = 0; j < v.size(); ++j)
		{
			file.write(v[j]);

			if (j < v.size() - 1)
				file.write(',');
			else
				file.write('\n');
		}
	}
}
