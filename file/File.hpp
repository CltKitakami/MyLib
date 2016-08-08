#ifndef _FILE_HPP_
#define _FILE_HPP_

#include <cstring>
#include <string>
#include <fstream>
#include "pattern/IteratorAdaptor.hpp"
#include "pattern/NonCopyable.hpp"


class File : private NonCopyable
{
public:
	typedef std::fstream FileType;
	typedef std::streamsize SizeType;
	typedef std::streampos PositionType;
	typedef std::streamoff OffsetType;

	typedef enum Mode
	{
		APPEND = std::ios_base::ate,
		BINARY = std::ios_base::binary,
		READ = std::ios_base::in,
		WRITE = std::ios_base::out,
		TRUNCATE = std::ios_base::trunc,
		BINARY_READ = BINARY | READ,
		BINARY_WRITE = BINARY | WRITE
	} Mode;

private:
	enum { BLOCK_SIZE = 4096 };

	FileType file;

public:
	File() = default;

	File(const char *path, Mode mode) :
		file(path, (std::ios_base::openmode)mode) {}

	File(const std::string &path, Mode mode) :
		file(path, (std::ios_base::openmode)mode) {}

	~File();

	bool open(const char *path, Mode mode);
	bool open(const std::string &path, Mode mode);

	void close();

	bool isOpened() const { return this->file.is_open(); }
	bool isEnd() { return this->file.eof(); }

	int read();
	SizeType read(std::string &buffer);
	SizeType read(std::string &buffer, SizeType maxLength);
	SizeType read(char *buffer, SizeType maxLength);

	SizeType readLine(std::string &buffer);
	SizeType readLine(char *buffer, SizeType maxLength);

	void write(char data);
	void write(const char *data, SizeType length);
	void write(const std::string &data);

	void writeLine(const char *data, SizeType length);
	void writeLine(const std::string &data);

	PositionType getPosition();

	void goTo(OffsetType count);
	void goToOffset(PositionType count);
	void goToBegin(PositionType count = 0);
	void goToEnd(PositionType count = 0);

	void copyFrom(File &from, SizeType blockSize = BLOCK_SIZE);

	PositionType getSize();

	PositionType getRemain();

	FileType & getStream() { return this->file; }

	static bool exists(const char *path);
	static bool exists(const std::string &path);

	template <typename T>
	File & operator << (T &data)
	{ this->file << data; return *this; }

	File & operator << (const char *data);
	File & operator << (const std::string &data);

	template <typename T>
	File & operator >> (T &buffer)
	{ this->file >> buffer; return *this; }

	File & operator >> (std::string &buffer);

	operator bool () const { return this->isOpened(); }
};


inline File::Mode operator ~ (File::Mode a)
{ return (File::Mode)~(int)a; }

inline File::Mode operator | (File::Mode a, File::Mode b)
{ return (File::Mode)((int)a | (int)b); }

inline File::Mode operator & (File::Mode a, File::Mode b)
{ return (File::Mode)((int)a & (int)b); }

inline File::Mode operator ^ (File::Mode a, File::Mode b)
{ return (File::Mode)((int)a ^ (int)b); }

inline File::Mode & operator |= (File::Mode &a, File::Mode b)
{ return (File::Mode &)((int &)a |= (int)b); }

inline File::Mode & operator &= (File::Mode &a, File::Mode b)
{ return (File::Mode &)((int &)a &= (int)b); }

inline File::Mode & operator ^= (File::Mode &a, File::Mode b)
{ return (File::Mode &)((int &)a ^= (int)b); }

#endif
