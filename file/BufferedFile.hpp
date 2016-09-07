#ifndef _BUFFEREDFILE_HPP_
#define _BUFFEREDFILE_HPP_

#include "file/File.hpp"
#include "pattern/NonCopyable.hpp"


class BufferedFile : private NonCopyable
{
public:
	typedef File::Mode Mode;
	typedef File::FileType FileType;
	typedef File::SizeType SizeType;
	typedef File::PositionType PositionType;
	typedef File::OffsetType OffsetType;

	class Iterator :
		public IteratorAdaptor<std::output_iterator_tag, BufferedFile>
	{
		typedef IteratorAdaptor<std::output_iterator_tag, BufferedFile> Iter;

	public:
		Iterator(BufferedFile *p) : Iter(p), position(0) {}

		Iterator(const Iterator &b) :
			Iter(b), position(b.position) {}

		Iterator(const Iterator &b, PositionType position) :
			Iter(b), position(position) {}

		virtual ~Iterator() = default;

		int operator * () { return this->Iter::ptr->read(); }

		bool operator == (const Iterator &rhs) const
		{ return this->isEnd(rhs) == true; }

		bool operator != (const Iterator &rhs) const
		{ return this->isEnd(rhs) == false; }

	protected:
		PositionType position;

		virtual void next() { this->position += 1; }

		bool isEnd(const Iterator &rhs) const
		{
			if (this->Iter::ptr == rhs.Iter::ptr)
				return this->position == rhs.position;
			else
				return true;
		}
	};

private:
	enum { BUFFER_SIZE = 4096 };

	File file;
	char *buffer;
	SizeType bufferSize, cursor, endCursor;
	bool isWrite;

public:
	BufferedFile(SizeType bufferSize = BUFFER_SIZE) :
		buffer(new char[bufferSize]), bufferSize(bufferSize),
		cursor(0), endCursor(0), isWrite(false) {}

	BufferedFile(const char *path, Mode mode,
		SizeType bufferSize = BUFFER_SIZE) :
		buffer(new char[bufferSize]), bufferSize(bufferSize),
		cursor(0), endCursor(0), isWrite(false)
		{ this->open(path, mode); }

	BufferedFile(const std::string &path, Mode mode,
		SizeType bufferSize = BUFFER_SIZE) :
		buffer(new char[bufferSize]), bufferSize(bufferSize),
		cursor(0), endCursor(0), isWrite(false)
		{ this->open(path, mode); }

	~BufferedFile();

	bool open(const char *path, Mode mode);
	bool open(const std::string &path, Mode mode);

	void close();

	bool isOpened() const { return this->file.isOpened(); }
	bool isEnd() { return this->file.isEnd(); }

	int read();
	SizeType read(std::string &buffer);
	SizeType read(std::string &buffer, SizeType maxLength);
	SizeType read(char *buffer, SizeType maxLength);

	void write(char data);
	void write(const char *data, SizeType length);
	void write(const std::string &data);

	void writeLine(const char *data, SizeType length);
	void writeLine(const std::string &data);

	PositionType getPosition() { return this->file.getPosition(); }

	void goTo(OffsetType count) { this->file.goTo(count); }
	void goToOffset(PositionType count) { this->file.goToOffset(count); }
	void goToBegin(PositionType count = 0) { this->file.goToBegin(count); }
	void goToEnd(PositionType count = 0) { this->file.goToEnd(count); }

	PositionType getSize() { return this->file.getSize(); }

	FileType & getStream() { return this->file.getStream(); }

	Iterator begin() { return Iterator(this); }
	Iterator end() { return Iterator(this, this->getSize()); }

	BufferedFile & operator << (char data)
	{ this->write(data); return *this; }

	BufferedFile & operator << (const char *data)
	{ this->write(data, (SizeType)::strlen(data)); return *this; }

	BufferedFile & operator << (const std::string &data)
	{ this->write(data); return *this; }

	BufferedFile & operator >> (std::string &buffer)
	{ this->read(buffer); return *this; }

	operator bool () const { return this->isOpened(); }

private:
	SizeType getRemain() const { return this->endCursor - this->cursor; }
	void moveCursor(SizeType offset) { this->cursor += offset; }
	void readNextBuffer();
	void writeBuffer();
};

#endif
