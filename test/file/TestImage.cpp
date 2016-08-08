#define PNG_USE_C_STDIO
#include "file/Png.hpp"
#include "file/Png.cpp"
#include "file/Jpg.hpp"
#include "file/Jpg.cpp"
#include "file/Bmp.hpp"
#include "file/Bmp.cpp"

int main()
{
	{
		const size_t channel = 4;
		const SizeU size(100, 100);
		const size_t bufSize = size.getArea() * channel;
		char buf[bufSize];

		for (size_t i = 0; i < bufSize; i += channel)
		{
			buf[i + 0] = '\xff';
			buf[i + 1] = '\x00';
			buf[i + 2] = '\xff';
			buf[i + 3] = '\xff';
		}

		ImageAttributes attributes;
		attributes.assign(buf, channel, size.width * channel);
		attributes.setSize(size);
		PngWriter p("test.png", size);
		p.write(attributes);
	}
	{
		const size_t channel = 3;
		const SizeU size(100, 100);
		const size_t bufSize = size.getArea() * channel;
		char buf[bufSize];

		for (size_t i = 0; i < bufSize; i += channel)
		{
			buf[i + 0] = '\xff';
			buf[i + 1] = '\x00';
			buf[i + 2] = '\xff';
		}

		ImageAttributes attributes;
		attributes.assign(buf, channel, size.width * channel);
		attributes.setSize(size);
		JpgWriter j("test.jpg", SizeU(100, 100));
		j.write(attributes);
	}
	{
		const size_t channel = 3;
		const SizeU size(100, 100);
		const size_t bufSize = size.getArea() * channel;
		char buf[bufSize];

		for (size_t i = 0; i < bufSize; i += channel)
		{
			buf[i + 0] = '\xff';
			buf[i + 1] = '\x00';
			buf[i + 2] = '\xff';
		}

		ImageAttributes attributes;
		attributes.assign(buf, channel, size.width * channel);
		attributes.setSize(size);
		Bmp b("test.bmp", SizeU(100, 100));
		b.write(attributes);
	}

	return 0;
}
