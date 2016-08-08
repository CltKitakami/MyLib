#include <limits>

#include "file/BufferedFile.hpp"


BufferedFile::~BufferedFile()
{
	this->close();

	if (this->buffer != nullptr)
	{
		delete [] this->buffer;
		this->buffer = nullptr;
	}
}


bool BufferedFile::open(const char *path, Mode mode)
{
	if ((mode & File::WRITE) == File::WRITE)
	{
		this->isWrite = true;
		this->endCursor = bufferSize;
	}
	else
	{
		this->isWrite = false;
		this->endCursor = 0;
	}

	return this->file.open(path, mode);
}


bool BufferedFile::open(const std::string &path, Mode mode)
{ return this->file.open(path.data(), mode); }


void BufferedFile::close()
{
	if (this->isWrite == true)
		this->writeBuffer();

	this->file.close();
}


void BufferedFile::readNextBuffer()
{
	this->endCursor = this->file.read(this->buffer, this->bufferSize);
	this->cursor = 0;
}


void BufferedFile::writeBuffer()
{
	this->file.write(this->buffer, this->cursor);
	this->cursor = 0;
}


int BufferedFile::read()
{
	if (this->getRemain() == 0)
		this->readNextBuffer();

	return this->buffer[this->cursor++];
}


BufferedFile::SizeType BufferedFile::read(std::string &buffer)
{
	SizeType fileSize = (SizeType)std::min(this->file.getSize(),
		(PositionType)std::numeric_limits<SizeType>::max());

	buffer.resize((size_t)fileSize);
	this->read(&buffer[0], fileSize);

	return fileSize;
}


BufferedFile::SizeType BufferedFile::read(
	std::string &buffer, SizeType maxLength)
{
	buffer.resize((size_t)maxLength);
	SizeType length = this->read(&buffer[0], maxLength);
	buffer.resize((size_t)length);
	return length;
}


BufferedFile::SizeType BufferedFile::read(
	char *buffer, SizeType maxLength)
{
	char *p = buffer;
	SizeType remain = this->getRemain();

	if (maxLength >= remain)
	{
		std::copy(this->buffer, this->buffer + remain, p);

		SizeType desire = maxLength - remain;
		SizeType readSize = 0;

		if (desire >= this->bufferSize)
		{
			readSize = this->file.read(p + remain, desire);
			this->readNextBuffer();
		}
		else
		{
			this->readNextBuffer();
			readSize = std::min(this->endCursor, desire);
			std::copy(this->buffer, this->buffer + readSize, p);
			this->moveCursor(readSize);
		}

		return remain + readSize;
	}
	else
	{
		std::copy(this->buffer, this->buffer + maxLength, p);
		this->moveCursor(maxLength);
		return maxLength;
	}

}


void BufferedFile::write(char data)
{
	if (this->getRemain() == 0)
		this->writeBuffer();

	this->buffer[this->cursor++] = data;
}


void BufferedFile::write(const char *data, SizeType length)
{
	const char *p = data;
	SizeType remain = this->getRemain();

	if (length > remain)
	{
		std::copy(data, data + remain, this->buffer + this->cursor);
		this->writeBuffer();

		SizeType desire = length - remain;

		if (desire >= this->bufferSize)
		{
			this->file.write(data, desire);
		}
		else
		{
			std::copy(p, p + length, this->buffer + this->cursor);
			this->moveCursor(length);
		}
	}
	else
	{
		std::copy(p, p + length, this->buffer + this->cursor);
		this->moveCursor(length);
	}
}


void BufferedFile::write(const std::string &data)
{ this->write(data.data(), (SizeType)data.length()); }


void BufferedFile::writeLine(const char *data, SizeType length)
{
	this->write(data, length);
	this->write('\n');
}


void BufferedFile::writeLine(const std::string &data)
{
	this->write(data);
	this->write('\n');
}
