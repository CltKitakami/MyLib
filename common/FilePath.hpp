#ifndef _FILEPATH_H_
#define _FILEPATH_H_

#include <string>


class FilePath
{
public:
	FilePath(const char *path) : path(path) {}
	FilePath(const std::string &path) : path(path) {}
	FilePath(FilePath &&filePath) = default;
	FilePath(std::string &&path) : path(std::move(path)) {}

	std::string toString() const { return this->path; }

	std::string getExtension() const;
	static std::string getExtension(const std::string &path);
	static std::string getExtension(const char *path);
	static std::string getExtension(const char *path, int length);

	std::string getBaseName() const;
	static std::string getBaseName(const std::string &path);
	static std::string getBaseName(const char *path);
	static std::string getBaseName(const char *path, int length);

	std::string getFullName() const;
	static std::string getFullName(const std::string &path);
	static std::string getFullName(const char *path);
	static std::string getFullName(const char *path, int length);

	std::string getDirectory() const;
	static std::string getDirectory(const std::string &path);
	static std::string getDirectory(const char *path);
	static std::string getDirectory(const char *path, int length);

	static char getSpliter()
	{
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif
	}

	static bool isSpliter(char ch)
	{ return ch == '/' || ch =='\\'; }

	FilePath & operator += (const std::string &path);

	FilePath & operator = (const std::string &path);
	FilePath & operator = (std::string &&path);
	FilePath & operator = (FilePath &&filePath) = default;

	operator std::string () const { return this->path; }

private:
	std::string path;
};

#endif
