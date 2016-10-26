#ifndef _BMP_HPP_
#define _BMP_HPP_

#include "type/ImageAttributes.hpp"
#include "file/File.hpp"


class Bmp
{
public:
	enum { HEADER_SIZE = 54, DIB_HEADER_SIZE = 40 };

	typedef struct Header
	{
		uint16_t id;
		uint32_t fileSize;
		uint32_t reserved;
		uint32_t dataOffset;
		uint32_t dibHeaderSize;
		uint32_t width;
		uint32_t height;
		uint16_t planes;
		uint16_t bitsPrePixel;
		uint32_t compression;
		uint32_t imageSize;
		uint32_t xPixelsPerMeter;
		uint32_t yPixelsPerMeter;
		uint32_t colorsInColorTable;
		uint32_t importantColorCount;
	} Header;

	Bmp() = default;
	Bmp(const char *fileName);
	Bmp(const char *fileName, const SizeU &size, uint8_t channel = 3);

	~Bmp();

	void open(const char *fileName);
	void open(const char *fileName, const SizeU &size, uint8_t channel = 3);

	void close();

	void read(ImageAttributes &attributes);
	void write(ImageAttributes &attributes);

	uint32_t getWidth() const { return header.width; }
	uint32_t getHeight() const { return header.height; }

	SizeU getSize() const { return SizeU(getWidth(), getHeight()); }

	uint8_t getChannel() const { return (uint8_t)(header.bitsPrePixel / 8); }

private:
	uint32_t getPadding() const { return header.width % 4; }
	uint32_t getLineStep() const { return header.width * (uint32_t)getChannel(); }
	void readHeader();
	void writeHeader();
	void fillHeader(const char buffer[HEADER_SIZE]);
	void createDefaultHeader(const SizeU &size, uint8_t channel);

	Header header;
	File file;
};

#endif
