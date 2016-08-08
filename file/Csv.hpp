#ifndef _CSV_HPP_
#define _CSV_HPP_

#include <vector>
#include <string>


class Csv
{
public:
	typedef std::vector<std::string> Line;
	typedef std::vector<Line> Data;

	Csv() = default;
	Csv(const char *path);

	void load(const char *path);
	void save(const char *path);
	void clear();

	size_t getLines() const { return this->data.size(); }

	const Line & getLine(size_t index) const
	{
		return this->data[index];
	}

	Line & getLine(size_t index)
	{
		return this->data[index];
	}

	Line & putLine()
	{
		this->data.push_back(Line());
		return this->data.back();
	}

	void setLine(size_t index, const Line &line) { this->data[index] = line; }
	void setLine(size_t index, Line &&line) { this->data[index] = line; }

	void setData(const Data &data) { this->data = data; }
	void setData(Data &&data) { this->data = data; }

private:
	Data data;
};


#endif
