#include <cstring>
#include "FilePath.hpp"


std::string FilePath::getExtension() const
{
	return FilePath::getExtension(this->path);
}


std::string FilePath::getExtension(const std::string &path)
{
	return FilePath::getExtension(path.data(), (int)path.length());
}


std::string FilePath::getExtension(const char *path)
{
	return FilePath::getExtension(path, (int)::strlen(path));
}


std::string FilePath::getExtension(const char *path, int length)
{
	const char *begin = path;
	const char *end = path + length;
	const char *dotAt = end;

	while (--dotAt >= begin)
	{
		if (FilePath::isSpliter(*dotAt) == true)
			break;
		else if (*dotAt == '.')
		{
			++dotAt;
			return std::string(dotAt, (size_t)(end - dotAt));
		}
	}

	return std::string();
}


std::string FilePath::getBaseName() const
{
	return FilePath::getBaseName(this->path);
}


std::string FilePath::getBaseName(const std::string &path)
{
	return FilePath::getBaseName(path.data(), (int)path.length());
}


std::string FilePath::getBaseName(const char *path)
{
	return FilePath::getBaseName(path, (int)::strlen(path));
}


std::string FilePath::getBaseName(const char *path, int length)
{
	const char *begin = path;
	const char *end = path + length;
	const char *lastSpliterAt = end;
	const char *dotAt = nullptr;

	while (--lastSpliterAt >= begin)
	{
		if (dotAt == nullptr && *lastSpliterAt == '.')
			dotAt = lastSpliterAt;
		else if (FilePath::isSpliter(*lastSpliterAt) == true)
			break;
	}

	if (dotAt == nullptr)
		dotAt = end;

	++lastSpliterAt;

	return std::string(lastSpliterAt, (size_t)(dotAt - lastSpliterAt));
}


std::string FilePath::getFullName() const
{
	return FilePath::getFullName(this->path);
}


std::string FilePath::getFullName(const std::string &path)
{
	return FilePath::getFullName(path.data(), (int)path.length());
}


std::string FilePath::getFullName(const char *path)
{
	return FilePath::getFullName(path, (int)::strlen(path));
}


std::string FilePath::getFullName(const char *path, int length)
{
	const char *begin = path;
	const char *end = path + length;
	const char *lastSpliterAt = end;

	while (--lastSpliterAt >= begin)
	{
		if (FilePath::isSpliter(*lastSpliterAt) == true)
			break;
	}

	++lastSpliterAt;

	return std::string(lastSpliterAt, (size_t)(end - lastSpliterAt));
}


std::string FilePath::getDirectory() const
{
	return FilePath::getDirectory(this->path);
}


std::string FilePath::getDirectory(const std::string &path)
{
	return FilePath::getDirectory(path.data(), (int)path.length());
}


std::string FilePath::getDirectory(const char *path)
{
	return FilePath::getDirectory(path, (int)::strlen(path));

}


std::string FilePath::getDirectory(const char *path, int length)
{
	const char *begin = path;
	const char *end = path + length;
	const char *lastSpliterAt = end;

	while (--lastSpliterAt >= begin)
	{
		if (FilePath::isSpliter(*lastSpliterAt) == true)
			break;
	}

	return std::string(begin, lastSpliterAt);
}


FilePath & FilePath::operator += (const std::string &path)
{
	this->path += FilePath::getSpliter();
	this->path += path;
	return *this;
}


FilePath & FilePath::operator = (const std::string &path)
{
	this->path = path; return *this;
}


FilePath & FilePath::operator = (std::string &&path)
{
	this->path.assign(std::move(path));
	return *this;
}
