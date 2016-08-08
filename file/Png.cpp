#include "Png.hpp"
#include "common/Exception.hpp"


#ifndef PNG_USE_C_STDIO
#include <pngstruct.h>
#include <pnginfo.h>

static void _pngRead_(png_structp png_ptr, png_bytep data, png_size_t length)
{
	File *file = (File *)png_ptr->io_ptr;

	if ((png_size_t)file->read((char *)data, (File::SizeType)length) != length)
		png_error(png_ptr, "PNG read error.");
}


static void _pngWrite_(png_structp png_ptr, png_bytep data, png_size_t length)
{
	File *file = (File *)png_ptr->io_ptr;
	file->write((const char *)data, (File::SizeType)length);
}


static void _pngFlush_(png_structp png_ptr)
{
	(void)png_ptr;
}

#endif


PngFileType Png::openFile(const char *fileName, bool isWrite)
{
#ifndef PNG_USE_C_STDIO
	File::Mode mode = isWrite == true ? File::BINARY_WRITE : File::BINARY_READ;
	PngFileType file = new File(fileName, mode);
	bool isFailed = file->opened() == false;
#else
	PngFileType file = ::fopen(fileName, isWrite == true ? "wb" : "rb");
	bool isFailed = file == nullptr;
#endif

	if (isFailed == true)
	{
		Exception ex(TRACE, "Can't open ");
		ex << fileName;
		throw ex;
	}

	return file;
}


void Png::closeFile(PngFileType &file)
{
	if (file != nullptr)
	{
#ifndef PNG_USE_C_STDIO
		delete file;
#else
		::fclose(file);
#endif
		file = nullptr;
	}
}


void Png::checkMagicNumber(PngFileType file)
{
	char header[CHECK_HEADER_BYTES];

#ifndef PNG_USE_C_STDIO
	file->read(header, CHECK_HEADER_BYTES);
#else
	::fread(header, 1, CHECK_HEADER_BYTES, file);
#endif

	if (png_sig_cmp((png_bytep)header, 0, CHECK_HEADER_BYTES) != 0)
		throw Exception(TRACE, "Not a PNG file");
}


png_structp Png::createReadStruct()
{
	png_structp pngStruct = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (pngStruct == nullptr)
		throw Exception(TRACE, "png_create_read_struct failed");

	return pngStruct;
}


png_structp Png::createWriteStruct()
{
	png_structp pngStruct = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (pngStruct == nullptr)
		throw Exception(TRACE, "png_create_write_struct failed");

	return pngStruct;
}


png_infop Png::createInfoStruct(png_structp pngStruct)
{
	png_infop pngInfo = png_create_info_struct(pngStruct);

	if (pngInfo == nullptr)
		throw Exception(TRACE, "png_create_info_struct failed");

	return pngInfo;
}


Png::Info Png::readPrepare(const char *fileName)
{
	int bitDepth = 0, colorType = 0, interlaceType = 0;
	Info info;

	Png::closeFile(info.file);
	info.file = Png::openFile(fileName, false);
	Png::checkMagicNumber(info.file);

	info.pngStruct = Png::createReadStruct();
	info.pngInfo = Png::createInfoStruct(info.pngStruct);

#ifdef PNG_USE_C_STDIO
	png_init_io(info.pngStruct, info.file);
#else
	png_set_read_fn(info.pngStruct, (png_voidp)info.file, _pngRead_);
#endif

	png_set_sig_bytes(info.pngStruct, CHECK_HEADER_BYTES);
	png_read_info(info.pngStruct, info.pngInfo);

	png_uint_32 pngWidth = 0, pngHeight = 0;

	png_get_IHDR(info.pngStruct, info.pngInfo, &pngWidth, &pngHeight,
		&bitDepth, &colorType, &interlaceType, nullptr, nullptr);

	info.size.width = (uint32_t)pngWidth;
	info.size.height = (uint32_t)pngHeight;

	info.type = (Png::ColorType)colorType;

	if (info.type == Png::PALETTE)
	{
		png_set_palette_to_rgb(info.pngStruct);
		info.type = Png::RGB;
	}

	if (info.type == Png::GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(info.pngStruct);

	if (info.type == PNG_COLOR_TYPE_RGB || info.type == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(info.pngStruct);

	if (png_get_valid(info.pngStruct, info.pngInfo, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(info.pngStruct);
		info.type = Png::RGBA;
	}

	return info;
}


void Png::readToBuffer(Png::Info &info, ImageAttributes &attributes)
{
	for (uint32_t y = 0; y < info.size.height; ++y)
	{
		char *p = attributes.getRow(y);
		png_read_rows(info.pngStruct, (png_bytepp)&p, nullptr, 1);
	}
}


void Png::readFinalize(Png::Info &info)
{
	png_read_end(info.pngStruct, info.pngInfo);
	Png::closeFile(info.file);
	png_destroy_read_struct(&info.pngStruct, &info.pngInfo, (png_infopp)0);
}


Png::Info Png::writePrepare(const char *fileName,
	const SizeU &size, Png::ColorType type)
{
	const png_byte bitDepth = 8;
	Info info;

	Png::closeFile(info.file);
	info.file = Png::openFile(fileName, true);

	info.pngStruct = Png::createWriteStruct();
	info.pngInfo = Png::createInfoStruct(info.pngStruct);

#ifdef PNG_USE_C_STDIO
	png_init_io(info.pngStruct, info.file);
#else
	png_set_write_fn(info.pngStruct, (png_voidp)info.file, _pngWrite_, _pngFlush_);
#endif

	png_set_IHDR(info.pngStruct, info.pngInfo, size.width, size.height,
		bitDepth, (png_byte)type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(info.pngStruct, info.pngInfo);

	info.size = size;
	info.lineStep = size.width * (type == Png::RGBA ? 4 : 3);
	info.type = type;

	return info;
}


void Png::writeFromBuffer(Png::Info &info, ImageAttributes &attributes)
{
	for (uint32_t y = 0; y < info.size.height; ++y)
	{
		char *p = attributes.getRow(y);
		png_write_rows(info.pngStruct, (png_bytepp)&p, 1);
	}
}


void Png::writeFinalize(Png::Info &info)
{
	png_write_end(info.pngStruct, info.pngInfo);
	Png::closeFile(info.file);
	png_destroy_write_struct(&info.pngStruct, &info.pngInfo);
}


PngReader::PngReader(const char *fileName) : isOpened(false)
{
	this->open(fileName);
}


PngReader::~PngReader()
{
	this->close();
}


void PngReader::open(const char *fileName)
{
	PngReader::close();
	this->info = Png::readPrepare(fileName);
	this->isOpened = true;
}


void PngReader::close()
{
	if (this->isOpened == true)
	{
		this->isOpened = false;
		Png::readFinalize(this->info);
	}
}


void PngReader::read(ImageAttributes &attributes)
{
	Png::readToBuffer(this->info, attributes);
}


PngWriter::PngWriter(const char *fileName, const SizeU &size,
	Png::ColorType type) : isOpened(false)
{
	this->open(fileName, size, type);
}


PngWriter::~PngWriter()
{
	this->close();
}


void PngWriter::open(const char *fileName, const SizeU &size,
	Png::ColorType type)
{
	this->close();
	this->info = Png::writePrepare(fileName, size, type);
	this->isOpened = true;
}


void PngWriter::close()
{
	if (this->isOpened == true)
	{
		this->isOpened = false;
		Png::writeFinalize(this->info);
	}
}


void PngWriter::write(ImageAttributes &attributes)
{
	Png::writeFromBuffer(this->info, attributes);
}
