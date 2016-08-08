#include <cstdint>

#include "FileExplorer.hpp"
#include "common/Exception.hpp"


FileExplorer::Entry FileExplorer::openDirectory(const std::string &path)
{
	Entry entry;
	bool isOpened = false;

#ifdef _WIN32
	entry.ptr = ::FindFirstFile(path.data(), &entry.info);
	isOpened = entry.ptr != INVALID_HANDLE_VALUE;
#else
	entry.ptr = ::opendir(path.data());

	if (entry.ptr != nullptr)
	{
		FileExplorer::findNextFile(entry);
		isOpened = true;
	}
	else
		isOpened = false;

#endif

	if (isOpened == false)
	{
		Exception ex(TRACE, "No such directory: ");
		ex << path;
		throw ex;
	}

	return entry;
}


void FileExplorer::closeDirectory(Entry &entry)
{
#ifdef _WIN32
	::FindClose(entry.ptr);
#else
	::closedir(entry.ptr);
#endif
}


void FileExplorer::findNextFile(Entry &entry)
{
#ifdef _WIN32
	if (::FindNextFile(entry.ptr, &entry.info) == 0)
		entry.ptr = nullptr;
#else
	entry.info = ::readdir(entry.ptr);

	if (entry.info == nullptr)
		entry.ptr = nullptr;
#endif
}


bool FileExplorer::isDotOrDotDot(Entry &entry)
{
	std::string name = entry.getName();
	const char *ch = name.data();

	if (name.length() == 1 && *ch == '.' ||
		name.length() == 2 && *ch == '.' && *(ch + 1) == '.')
		return true;
	else
		return false;
}


std::vector<std::string> FileExplorer::getFilesInDirectory(const std::string &path, bool isRecursive)
{
	Entry entry;
	std::vector<std::string> fileNames;

	try
	{
#ifdef _WIN32
		entry = FileExplorer::openDirectory((path + "\\*").data());
#else
		entry = FileExplorer::openDirectory(path.data());
#endif
	}
	catch (Exception ex)
	{
		return fileNames;
	}

	do
	{
		FileExplorer::findNextFile(entry);
	} while (entry.ptr != nullptr && FileExplorer::isDotOrDotDot(entry) == true);

	while (entry.ptr != nullptr)
	{
		std::string findName = entry.getName();

#ifdef _WIN32
		bool isFile = (entry.info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
		std::string findPath;
		findPath.reserve(path.length() + findName.length() + 1);
		findPath += path;
		findPath += '/';
		findPath += findName;

		struct stat st;
		::lstat(findPath.data(), &st);

		bool isFile = S_ISDIR(st.st_mode) == 0;
#endif

		bool isFolder = isFile == false;

		if (isFile == true)
			fileNames.push_back(findName);
		else if (isRecursive == true && isFolder == true)
		{
#ifdef _WIN32
			std::vector<std::string> subFolderFiles = getFilesInDirectory(path + "/" + findName, isRecursive);
#else
			std::vector<std::string> subFolderFiles = getFilesInDirectory(findPath, isRecursive);
#endif
			fileNames.reserve(fileNames.size() + subFolderFiles.size());

			for (size_t i = 0; i < subFolderFiles.size(); ++i)
				fileNames.push_back(findName + "/" + subFolderFiles[i]);
		}

		FileExplorer::findNextFile(entry);
	}

	FileExplorer::closeDirectory(entry);

	return fileNames;
}
