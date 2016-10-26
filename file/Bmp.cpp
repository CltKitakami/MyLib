#include "Bmp.hpp"
#include "common/Convert.hpp"
#include "common/Exception.hpp"


Bmp::Bmp(const char *fileName)
{
	this->open(fileName);
}


Bmp::Bmp(const char *fileName,
	const SizeU &size, uint8_t channel)
{
	this->open(fileName, size, channel);
}


Bmp::~Bmp()
{
	this->close();
}


void Bmp::open(const char *fileName)
{
	if (this->file.open(fileName, File::BINARY_READ) == true)
		this->readHeader();
	else
		throw Exception(TRACE, fileName) << " not exist";
}


void Bmp::open(const char *fileName,
	const SizeU &size, uint8_t channel)
{
	if (this->file.open(fileName, File::BINARY_WRITE) == true)
		this->createDefaultHeader(size, channel);
	else
		throw Exception(TRACE, "Can't open ") << fileName;
}


void Bmp::close()
{
	this->file.close();
}


void Bmp::read(ImageAttributes &attributes)
{
	Header &h = this->header;
	File::SizeType padding = (File::SizeType)this->getPadding();
	File::SizeType lineStep = (File::SizeType)this->getLineStep();
	char dump[3];

	if (padding != 0)
	{
		for (int row = (int)h.height - 1; row >= 0; --row)
		{
			this->file.read(attributes.getRow((uint32_t)row), lineStep);
			this->file.read(dump, padding);
		}
	}
	else
	{
		for (int row = (int)h.height - 1; row >= 0; --row)
			this->file.read(attributes.getRow((uint32_t)row), lineStep);
	}
}


void Bmp::write(ImageAttributes &attributes)
{
	Header &h = this->header;
	File::SizeType padding = (File::SizeType)this->getPadding();
	File::SizeType lineStep = (File::SizeType)this->getLineStep();
	char dump[3] = { 0, 0, 0 };

	this->writeHeader();

	if (padding != 0)
	{
		for (int row = (int)h.height - 1; row >= 0; --row)
		{
			this->file.write(attributes.getRow((uint32_t)row), lineStep);
			this->file.write(dump, padding);
		}
	}
	else
	{
		for (int row = (int)h.height - 1; row >= 0; --row)
			this->file.write(attributes.getRow((uint32_t)row), lineStep);
	}
}


void Bmp::readHeader()
{
	char buffer[HEADER_SIZE];
	Header &h = this->header;

	this->file.read(buffer, HEADER_SIZE);

	if (buffer[0] != 'B' || buffer[1] != 'M')
	{
		throw Exception(TRACE, "Read bitmap header magic number error");
	}
	else
	{
		this->fillHeader(buffer);

		if (h.dibHeaderSize != DIB_HEADER_SIZE ||
			h.colorsInColorTable != 0)
		{
			Exception ex(TRACE, "Unsupport: dibHeaderSize = ");
				ex << h.dibHeaderSize << ", colorsInColorTable = " <<
				h.colorsInColorTable;
			throw ex;
		}
	}
}


void Bmp::writeHeader()
{
	char buffer[HEADER_SIZE];
	char *p = buffer;

	Convert::splitDualBytes(header.id, &p[0]);
	Convert::splitQuadBytes(header.fileSize, &p[2]);
	Convert::splitQuadBytes(header.reserved, &p[6]);
	Convert::splitQuadBytes(header.dataOffset, &p[10]);
	Convert::splitQuadBytes(header.dibHeaderSize, &p[14]);
	Convert::splitQuadBytes(header.width, &p[18]);
	Convert::splitQuadBytes(header.height, &p[22]);
	Convert::splitDualBytes(header.planes, &p[26]);
	Convert::splitDualBytes(header.bitsPrePixel, &p[28]);
	Convert::splitQuadBytes(header.compression, &p[30]);
	Convert::splitQuadBytes(header.imageSize, &p[34]);
	Convert::splitQuadBytes(header.xPixelsPerMeter, &p[38]);
	Convert::splitQuadBytes(header.yPixelsPerMeter, &p[42]);
	Convert::splitQuadBytes(header.colorsInColorTable, &p[46]);
	Convert::splitQuadBytes(header.importantColorCount, &p[50]);
	this->file.write(buffer, HEADER_SIZE);
}


void Bmp::fillHeader(const char buffer[HEADER_SIZE])
{
	Header &h = this->header;
	const char *p = buffer;

	h.id = Convert::combineDualBytes(p[0], p[1]);
	h.fileSize = Convert::combineQuadBytes(p[2], p[3], p[4], p[5]);
	h.reserved = Convert::combineQuadBytes(p[6], p[7], p[8], p[9]);
	h.dataOffset = Convert::combineQuadBytes(p[10], p[11], p[12], p[13]);
	h.dibHeaderSize = Convert::combineQuadBytes(p[14], p[15], p[16], p[17]);
	h.width = Convert::combineQuadBytes(p[18], p[19], p[20], p[21]);
	h.height = Convert::combineQuadBytes(p[22], p[23], p[24], p[25]);
	h.planes = Convert::combineDualBytes(p[26], p[27]);
	h.bitsPrePixel = Convert::combineDualBytes(p[28], p[29]);
	h.compression = Convert::combineQuadBytes(p[30], p[31], p[32], p[33]);
	h.imageSize = Convert::combineQuadBytes(p[34], p[35], p[36], p[37]);
	h.xPixelsPerMeter = Convert::combineQuadBytes(p[38], p[39], p[40], p[41]);
	h.yPixelsPerMeter = Convert::combineQuadBytes(p[42], p[43], p[44], p[45]);
	h.colorsInColorTable = Convert::combineQuadBytes(p[46], p[47], p[48], p[49]);
	h.importantColorCount = Convert::combineQuadBytes(p[50], p[51], p[52], p[53]);
}


void Bmp::createDefaultHeader(const SizeU &size, uint8_t channel)
{
	Header &h = this->header;
	h.width = size.width;
	h.height = size.height;

	uint32_t imageSize = h.height * (h.width * channel + this->getPadding());

	h.id = Convert::combineDualBytes('B', 'M');
	h.fileSize = imageSize + HEADER_SIZE;
	h.reserved = 0;
	h.dataOffset = HEADER_SIZE;
	h.dibHeaderSize = DIB_HEADER_SIZE;
	h.planes = 1;
	h.bitsPrePixel = channel * 8;
	h.compression = 0;
	h.imageSize = imageSize;
	h.xPixelsPerMeter = 0;
	h.yPixelsPerMeter = 0;
	h.colorsInColorTable = 0;
	h.importantColorCount = 0;
}
