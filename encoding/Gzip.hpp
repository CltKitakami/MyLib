#ifndef _GZIP_HPP_
#define _GZIP_HPP_

#include <string>


// Warning can't work for large than 1.9GB stream,
// because there use std::string to be a buffer.
class Gzip
{
	enum { GZIP_ENCODING = 16 };
	enum { BLOCK_SIZE = 4096 };

public:
	static std::string decompress(char *in, unsigned long length);
	static std::string compress(char *in, unsigned long length, int level = 9);
};


#endif
