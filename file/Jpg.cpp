#include <cstdio>
#include <cstdbool>

#include "Jpg.hpp"
#include "common/Exception.hpp"


void Jpg::DecompressInfo::open(const char *fileName)
{
	if (isCreated == false)
	{
		info.err = jpeg_std_error(&jerr.mgr);
		jerr.mgr.error_exit = Jpg::errorExit;

		if (setjmp(jerr.buffer) != 0)
		{
			release();
			return;
		}

		file = fopen(fileName, "rb");

		if (file == nullptr)
		{
			Exception ex(TRACE, "Can't open ");
			ex << fileName;
			throw ex;
		}

		isCreated = true;
		jpeg_create_decompress(&info);
		jpeg_stdio_src(&info, file);

		(void)jpeg_read_header(&info, TRUE);
		(void)jpeg_start_decompress(&info);
	}
}


void Jpg::DecompressInfo::release()
{
	if (isCreated == true)
	{
		isCreated = false;
		(void)jpeg_finish_decompress(&info);
		jpeg_destroy_decompress(&info);
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}
}


void Jpg::CompressInfo::open(const char *fileName,
	const SizeU &size, int quality)
{
	if (isCreated == false)
	{
		info.err = jpeg_std_error(&jerr.mgr);
		jerr.mgr.error_exit = Jpg::errorExit;

		if (setjmp(jerr.buffer) != 0)
		{
			release();
			return;
		}

		file = fopen(fileName, "wb");

		if (file == nullptr)
		{
			Exception ex(TRACE, "Can't open ");
			ex << fileName;
			throw ex;
		}

		isCreated = true;
		jpeg_create_compress(&info);
		jpeg_stdio_dest(&info, file);

		info.image_width = (unsigned int)size.width;
		info.image_height = (unsigned int)size.height;
		info.input_components = 3;
		info.in_color_space = JCS_RGB;

		jpeg_set_defaults(&info);
		jpeg_set_quality(&info, quality, TRUE);
		jpeg_start_compress(&info, TRUE);
	}
}


void Jpg::CompressInfo::release()
{
	if (isCreated == true)
	{
		isCreated = false;
		(void)jpeg_finish_compress(&info);
		jpeg_destroy_compress(&info);
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}
}


void Jpg::errorExit(j_common_ptr info)
{
	Error *err = (Error *)info->err;
	(*info->err->output_message) (info);
	longjmp(err->buffer, 1);
}


Jpg::DecompressInfo Jpg::readPrepare(const char *fileName)
{
	DecompressInfo info;
	info.open(fileName);
	return info;
}


void Jpg::readToBuffer(Jpg::DecompressInfo &info,
	ImageAttributes &attributes, ColorType type)
{
	struct jpeg_decompress_struct &jinfo = info.self();

#ifdef JPEG_TURBO
	jinfo.out_color_space = (J_COLOR_SPACE)type;	// change default
#else
	(void)type;
#endif

	while ((uint32_t)jinfo.output_scanline < info.getHeight())
	{
		unsigned char *p = (unsigned char *)
			attributes.getRow((uint32_t)jinfo.output_scanline);
		(void)jpeg_read_scanlines(&jinfo, &p, 1);
	}

}


void Jpg::readFinalize(Jpg::DecompressInfo &info)
{
	info.release();
}


Jpg::CompressInfo Jpg::writePrepare(const char *fileName,
	const SizeU &size, int quality)
{
	CompressInfo info;
	info.open(fileName, size, quality);
	return info;
}


void Jpg::writeFromBuffer(Jpg::CompressInfo &info, ImageAttributes &attributes)
{
	struct jpeg_compress_struct &jinfo = info.self();
	JSAMPROW row[1];

	while (jinfo.next_scanline < jinfo.image_height)
	{
		row[0] = (JSAMPROW)attributes.getRow((uint32_t)jinfo.next_scanline);
		(void)jpeg_write_scanlines(&jinfo, row, 1);
	}
}


void Jpg::writeFinalize(Jpg::CompressInfo &info)
{
	info.release();
}



JpgReader::JpgReader(const char *fileName) : isOpened(false)
{
	this->open(fileName);
}


JpgReader::~JpgReader()
{
	this->close();
}


void JpgReader::open(const char *fileName)
{
	if (isOpened == false)
	{
		info = Jpg::readPrepare(fileName);
		isOpened = true;
	}
}


void JpgReader::close()
{
	if (isOpened == true)
	{
		isOpened = false;
		Jpg::readFinalize(info);
	}
}


void JpgReader::read(ImageAttributes &attributes, Jpg::ColorType type)
{
	Jpg::readToBuffer(info, attributes, type);
}


JpgWriter::JpgWriter(const char *fileName,
	const SizeU &size, int quality) : isOpened(false)
{
	this->open(fileName, size, quality);
}


JpgWriter::~JpgWriter()
{
	this->close();
}


void JpgWriter::open(const char *fileName,
	const SizeU &size, int quality)
{
	if (isOpened == false)
	{
		info = Jpg::writePrepare(fileName, size, quality);
		isOpened = true;
	}
}


void JpgWriter::close()
{
	if (isOpened == true)
	{
		isOpened = false;
		Jpg::writeFinalize(info);
	}
}


void JpgWriter::write(ImageAttributes &attributes)
{
	Jpg::writeFromBuffer(info, attributes);
}
