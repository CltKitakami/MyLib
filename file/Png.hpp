#ifndef _PNG_HPP_
#define _PNG_HPP_

#include <png.h>

#include "type/ImageAttributes.hpp"

#ifndef PNG_USE_C_STDIO
#include "file/File.hpp"
typedef File * PngFileType;
#else
#include <cstdio>
typedef FILE * PngFileType;
#endif


class Png
{
public:
	typedef enum ColorType
	{
		GRAY = PNG_COLOR_TYPE_GRAY,
		GRAYA = PNG_COLOR_TYPE_GRAY_ALPHA,
		PALETTE = PNG_COLOR_TYPE_PALETTE,
		RGB = PNG_COLOR_TYPE_RGB,
		RGBA = PNG_COLOR_TYPE_RGB_ALPHA
	} ColorType;

	typedef struct Info
	{
		PngFileType file;
		png_structp pngStruct;
		png_infop pngInfo;
		SizeU size;
		uint32_t lineStep;
		ColorType type;

		Info() : file(nullptr) {}
	} Info;

	static Info readPrepare(const char *fileName);
	static void readToBuffer(Info &info, ImageAttributes &attributes);
	static void readFinalize(Info &info);

	static Info writePrepare(const char *fileName,
		const SizeU &size, ColorType type = RGBA);
	static void writeFromBuffer(Info &info, ImageAttributes &attributes);
	static void writeFinalize(Info &info);

private:
	enum { CHECK_HEADER_BYTES = 8 };

	static PngFileType openFile(const char *fileName, bool isWrite);
	static void closeFile(PngFileType &file);
	static void checkMagicNumber(PngFileType file);
	static png_structp createReadStruct();
	static png_structp createWriteStruct();
	static png_infop createInfoStruct(png_structp pngStruct);
};


class PngReader
{
public:
	PngReader() : isOpened(false) {}
	PngReader(const char *fileName);
	~PngReader();

	void open(const char *fileName);
	void close();

	void read(ImageAttributes &attributes);

	Png::ColorType getType() const { return info.type; }
	uint32_t getWidth() const { return info.size.width; }
	uint32_t getHeight() const { return info.size.height; }
	SizeU getSize() const { return info.size; }
	uint8_t getChannel() const
	{ return info.type == Png::RGBA ? 4 : 3; }

private:
	bool isOpened;
	Png::Info info;
};


class PngWriter
{
public:
	PngWriter() : isOpened(false) {}

	PngWriter(const char *fileName, const SizeU &size,
		Png::ColorType type = Png::RGBA);

	~PngWriter();

	void open(const char *fileName, const SizeU &size,
		Png::ColorType type = Png::RGBA);

	void close();

	void write(ImageAttributes &attributes);

private:
	bool isOpened;
	Png::Info info;
};

#endif
