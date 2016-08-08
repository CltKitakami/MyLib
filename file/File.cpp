#include <limits>
#include <algorithm>

#include "File.hpp"


File::~File()
{
	this->close();
}


bool File::open(const char *path, File::Mode mode)
{
	this->file.open(path, (std::ios_base::openmode)mode);
	return this->isOpened();
}


bool File::open(const std::string &path, File::Mode mode)
{
	this->file.open(path, (std::ios_base::openmode)mode);
	return this->isOpened();
}


void File::close()
{
	this->file.close();
}


void File::write(char data)
{
	this->file.put(data);
}


int File::read()
{
	return this->file.get();
}


File::SizeType File::read(std::string &buffer)
{
	SizeType fileSize = (SizeType)std::min(this->getRemain(),
		(PositionType)std::numeric_limits<SizeType>::max());

	buffer.resize((size_t)fileSize);
	this->read(&buffer[0], fileSize);

	return fileSize;
}


File::SizeType File::read(std::string &buffer, SizeType maxLength)
{
	buffer.resize((size_t)maxLength);
	SizeType length = this->read(&buffer[0], maxLength);
	buffer.resize((size_t)length);
	return length;
}


File::SizeType File::read(char *buffer, SizeType maxLength)
{
	this->file.read(buffer, maxLength);
	return this->file.gcount();
}


File::SizeType File::readLine(std::string &buffer)
{
	SizeType offset = 0;
	buffer.resize((size_t)BLOCK_SIZE);
	char *p = &buffer[0];

	while (true)
	{
		SizeType length = this->readLine(p, BLOCK_SIZE + 1);

		if (length != BLOCK_SIZE)
		{
			offset += length;
			break;
		}
		else
		{
			offset += BLOCK_SIZE;
			buffer.resize((size_t)(offset + length));
			p = &buffer[(size_t)offset];
		}
	}

	buffer.resize((size_t)offset);

	return offset;
}


File::SizeType File::readLine(char *buffer, SizeType maxLength)
{
	this->file.getline(buffer, maxLength);

	SizeType length = this->file.gcount();

	if (length >= 1 && buffer[(size_t)length - 1] == '\0')
	{
		if (length >= 2 && buffer[(size_t)length - 2] == '\r')
			length -= 2;
		else
			length -= 1;

		buffer[(size_t)length] = '\0';
	}

	return length;
}


void File::write(const char *data, SizeType length)
{
	this->file.write(data, length);
}


void File::write(const std::string &data)
{
	this->write(data.data(), (SizeType)data.length());
}


void File::writeLine(const char *data, SizeType length)
{
	this->write(data, length);
	this->write('\n');
}


void File::writeLine(const std::string &data)
{
	this->write(data);
	this->write('\n');
}


File::PositionType File::getPosition()
{
	return this->file.tellp();
}


void File::goTo(OffsetType count)
{
	this->file.seekp(count);
}


void File::goToOffset(PositionType count)
{
	this->file.seekp(count, std::ios_base::cur);
}


void File::goToBegin(PositionType count)
{
	this->file.seekp(count, std::ios_base::beg);
}


void File::goToEnd(PositionType count)
{
	this->file.seekp(count, std::ios_base::end);
}


void File::copyFrom(File &from, SizeType blockSize)
{
	char *buffer = new char[blockSize];

	while (true)
	{
		SizeType readSize = from.read(buffer, blockSize);
		this->write(buffer, readSize);

		if (readSize != blockSize)
			break;
	}

	delete buffer;
}


File::PositionType File::getSize()
{
	PositionType current = this->getPosition();
	this->goToEnd();
	PositionType size = this->getPosition();
	this->goToBegin(current);
	return size;
}


File::PositionType File::getRemain()
{
	PositionType current = this->getPosition();
	this->goToEnd();
	PositionType size = this->getPosition();
	this->goToBegin(current);
	return size - current;
}


bool File::exists(const char *path)
{
	return File(path, File::READ).isOpened() == true;
}


bool File::exists(const std::string &path)
{
	return File::exists(path.data());
}


File & File::operator << (const char *data)
{
	this->write(data, (SizeType)::strlen(data));
	return *this;
}

File & File::operator << (const std::string &data)
{
	this->write(data);
	return *this;
}


File & File::operator >> (std::string &buffer)
{
	this->read(buffer);
	return *this;
}
