#ifndef _FILEEXPLORER_HPP_
#define _FILEEXPLORER_HPP_

#include <string>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <cstring>
#include <windows.h>
#include <tchar.h>
#include <direct.h>
#elif defined(__linux__)
#include <sys/types.h>
#include <dirent.h>
#else
#error "Unknow OS"
#endif


class FileExplorer
{
public:
	static std::vector<std::string> getFilesInDirectory(const std::string &path, bool isRecursive = false);

	static void move(const std::string &oldPath, const std::string &newPath)
	{ move(oldPath.data(), newPath.data()); }
	static void remove(const std::string &path) { remove(path.data()); }
	static void mkdir(const std::string &path) { mkdir(path.data()); }
	static bool isFile(const std::string &path) { return isFile(path.data()); }
	static bool isDir(const std::string &path) { return isDir(path.data()); }
	static bool exists(const std::string &path) { return exists(path.data()); }

#ifdef _WIN32
	static void move(const char *oldPath, const char *newPath) { ::MoveFile(oldPath, newPath); }
	static void remove(const char *path) { ::DeleteFile(path); }
	static void mkdir(const char *path) { ::_mkdir(path); }
	static bool isFile(const char *path)
	{
		DWORD attr = ::GetFileAttributes(path);
		return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}
	static bool isDir(const char *path)
	{
		DWORD attr = ::GetFileAttributes(path);
		return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	static bool exists(const char *path)
	{ return ::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES; }
#else
	static void move(const char *oldPath, const char *newPath) { ::rename(oldPath, newPath); }
	static void remove(const char *path) { ::remove(path); }
	static void mkdir(const char *path) { ::mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH); }
	static bool exists(const char *path) { struct stat s; return ::stat(path, &s) == 0; }
	static bool isFile(const char *path) { struct stat s; ::stat(path, &s); return S_ISREG(s.st_mode) != 0; }
	static bool isDir(const char *path) { struct stat s; ::stat(path, &s); return S_ISDIR(s.st_mode) != 0; }
#endif

private:
	typedef struct Entry
	{
#ifdef _WIN32
		Entry() : ptr(nullptr) {}

		std::string getName() { return std::string(info.cFileName); }

		HANDLE ptr;
		WIN32_FIND_DATA info;
#else
		Entry() : ptr(nullptr), info(nullptr) {}

		std::string  getName() { return std::string(info->d_name); }

		DIR *ptr;
		struct dirent *info;
#endif
	} Entry;


	static Entry openDirectory(const std::string &path);
	static void closeDirectory(Entry &entry);
	static void findNextFile(Entry &entry);
	static bool isDotOrDotDot(Entry &entry);
};

#endif
