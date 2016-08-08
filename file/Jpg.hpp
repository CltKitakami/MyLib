#ifndef _JPG_HPP_
#define _JPG_HPP_

#include <jpeglib.h>
#include <setjmp.h>
#include <cstdio>
#include <cstdlib>
#include "type/ImageAttributes.hpp"


#define JPEG_TURBO


class Jpg
{
public:
	typedef struct Error
	{
		struct jpeg_error_mgr mgr;
		jmp_buf buffer;
	} Error;

	typedef enum ColorType
	{
#ifdef JPEG_TURBO
		BGR = JCS_EXT_BGR,
		RGB = JCS_EXT_RGB,
		RGBX = JCS_EXT_RGBX,
		BGRX = JCS_EXT_BGRX,
		XBGR = JCS_EXT_XBGR,
		DEFAULT = BGR
#else
		DEFAULT
#endif
	} ColorType;

	class Info
	{
	public:
		Info() : isCreated(false), file(nullptr) {}
		virtual ~Info() = default;
		virtual void release() = 0;

	protected:
		bool isCreated;
		FILE *file;
		Error jerr;
	};

	class DecompressInfo : public Info
	{
	public:
		DecompressInfo() : Info() {}
		void open(const char *fileName);
		virtual void release();
		struct jpeg_decompress_struct & self() { return info; }

		uint32_t getWidth() const { return (uint32_t)info.output_width; }
		uint32_t getHeight() const { return (uint32_t)info.output_height; }

		SizeU getSize() const { return SizeU(getWidth(), getHeight()); }

	private:
		struct jpeg_decompress_struct info;
	};

	class CompressInfo : public Info
	{
	public:
		CompressInfo() : Info() {}
		void open(const char *fileName, const SizeU &size, int quality = 80);
		virtual void release();
		struct jpeg_compress_struct & self() { return info; }

	private:
		struct jpeg_compress_struct info;
	};

	static DecompressInfo readPrepare(const char *fileName);
	static void readToBuffer(DecompressInfo &info,
		ImageAttributes &attributes, ColorType type = DEFAULT);
	static void readFinalize(DecompressInfo &info);

	static CompressInfo writePrepare(const char *fileName,
		const SizeU &size, int quality = 80);

	static void writeFromBuffer(CompressInfo &info,
		ImageAttributes &attributes);

	static void writeFinalize(CompressInfo &info);

	static void errorExit(j_common_ptr info);
};


class JpgReader
{
public:
	JpgReader() : isOpened(false) {}
	JpgReader(const char *fileName);
	~JpgReader();

	void open(const char *fileName);
	void close();

	void read(ImageAttributes &attributes,
		Jpg::ColorType type = Jpg::DEFAULT);

	uint32_t getWidth() const { return info.getWidth(); }
	uint32_t getHeight() const { return info.getHeight(); }
	SizeU getSize() const { return info.getSize(); }

private:
	bool isOpened;
	Jpg::DecompressInfo info;
};


class JpgWriter
{
public:
	JpgWriter() : isOpened(false) {}
	JpgWriter(const char *fileName, const SizeU &size, int quality = 80);

	~JpgWriter();

	void open(const char *fileName, const SizeU &size, int quality = 80);

	void close();

	void write(ImageAttributes &attributes);

private:
	bool isOpened;
	Jpg::CompressInfo info;
};

#endif
