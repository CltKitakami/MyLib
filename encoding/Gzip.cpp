#include <zlib.h>
#include "Gzip.hpp"
#include "common/Exception.hpp"
#include "common/String.hpp"


typedef z_stream Stream;


Stream Gzip_initialize()
{
	Stream stream;
	stream.next_in = nullptr;
	stream.avail_in = 0;
	stream.total_in = 0;
	stream.next_out = nullptr;
	stream.avail_out = 0;
	stream.total_out = 0;
	stream.msg = nullptr;
	stream.state = nullptr;
	stream.zalloc = nullptr;
	stream.zfree = nullptr;
	stream.opaque = nullptr;
	stream.data_type = 0;
	stream.adler = 0;
	stream.reserved = 0;
	return stream;
}


std::string Gzip::decompress(char *in, unsigned long length)
{
	int ret;
	unsigned long totalSize = 0;
	std::string out;
	char *pOut;

	if (length * 4 > Gzip::BLOCK_SIZE)
		pOut = String::getSpace(out, 0, length * 4);
	else
		pOut = String::getSpace(out, 0, Gzip::BLOCK_SIZE);

	Stream stream = Gzip_initialize();

	if (::inflateInit2(&stream, GZIP_ENCODING + MAX_WBITS) != Z_OK)
		throw Exception(TRACE, "inflateInit failed.");

	stream.next_in = reinterpret_cast<Bytef *>(in);
	stream.avail_in = length;

	do
	{
		stream.next_out = reinterpret_cast<Bytef *>(pOut);
		stream.avail_out = Gzip::BLOCK_SIZE;
		ret = ::inflate(&stream, Z_NO_FLUSH);

		if (totalSize < stream.total_out)
		{
			totalSize = stream.total_out;
			pOut = String::getSpace(out, totalSize, Gzip::BLOCK_SIZE);
		}
	} while (ret == Z_OK);

	::inflateEnd(&stream);

	if (ret != Z_STREAM_END)	// an error occurred that was not EOF
	{
		Exception ex(TRACE, "During zlib decompression: (");
		ex << ret << ")";
		throw ex;
	}

	out.resize(totalSize);

	return out;
}


std::string Gzip::compress(char *in, unsigned long length, int level)
{
	int ret;
	unsigned long totalSize = 0;
	std::string out;
	char *pOut;

	if (length / 2 > BLOCK_SIZE)
		pOut = String::getSpace(out, 0, length / 2);
	else
		pOut = String::getSpace(out, 0, BLOCK_SIZE);

	Stream stream = Gzip_initialize();

	if (::deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
		GZIP_ENCODING + 15, level, Z_DEFAULT_STRATEGY) != Z_OK)
		throw Exception(TRACE, "deflateInit failed.");

	stream.next_in = reinterpret_cast<Bytef *>(in);
	stream.avail_in = length;

	do {
		stream.next_out = reinterpret_cast<Bytef *>(pOut);
		stream.avail_out = Gzip::BLOCK_SIZE;
		ret = ::deflate(&stream, Z_FINISH);

		if (totalSize < stream.total_out)
		{
			totalSize = stream.total_out;
			pOut = String::getSpace(out, totalSize, Gzip::BLOCK_SIZE);
		}
	} while (ret == Z_OK);

	::deflateEnd(&stream);

	if (ret != Z_STREAM_END)	// an error occurred that was not EOF
	{
		Exception ex(TRACE, "During zlib compression: (");
		ex << ret << ")";
		throw ex;
	}

	out.resize(totalSize);

	return out;
}
