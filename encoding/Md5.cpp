#include <cstring>
#include "Md5.hpp"
#include "common/Convert.hpp"


enum S1 { S11 = 7, S12 = 12, S13 = 17, S14 = 22 };
enum S2 { S21 = 5, S22 =  9, S23 = 14, S24 = 20 };
enum S3 { S31 = 4, S32 = 11, S33 = 16, S34 = 23 };
enum S4 { S41 = 6, S42 = 10, S43 = 15, S44 = 21 };

enum K1
{
	K11 = 0xd76aa478, K12 = 0xe8c7b756, K13 = 0x242070db, K14 = 0xc1bdceee,
	K15 = 0xf57c0faf, K16 = 0x4787c62a, K17 = 0xa8304613, K18 = 0xfd469501,
	K19 = 0x698098d8, K110 = 0x8b44f7af, K111 = 0xffff5bb1, K112 = 0x895cd7be,
	K113 = 0x6b901122, K114 = 0xfd987193, K115 = 0xa679438e, K116 = 0x49b40821
};

enum K2
{
	K21 = 0xf61e2562, K22 = 0xc040b340, K23 = 0x265e5a51, K24 = 0xe9b6c7aa,
	K25 = 0xd62f105d, K26 = 0x02441453, K27 = 0xd8a1e681, K28 = 0xe7d3fbc8,
	K29 = 0x21e1cde6, K210 = 0xc33707d6, K211 = 0xf4d50d87, K212 = 0x455a14ed,
	K213 = 0xa9e3e905, K214 = 0xfcefa3f8, K215 = 0x676f02d9, K216 = 0x8d2a4c8a,
};

enum K3
{
	K31 = 0xfffa3942, K32 = 0x8771f681, K33 = 0x6d9d6122, K34 = 0xfde5380c,
	K35 = 0xa4beea44, K36 = 0x4bdecfa9, K37 = 0xf6bb4b60, K38 = 0xbebfbc70,
	K39 = 0x289b7ec6, K310 = 0xeaa127fa, K311 = 0xd4ef3085, K312 = 0x04881d05,
	K313 = 0xd9d4d039, K314 = 0xe6db99e5, K315 = 0x1fa27cf8, K316 = 0xc4ac5665
};

enum K4
{
	K41 = 0xf4292244, K42 = 0x432aff97, K43 = 0xab9423a7, K44 = 0xfc93a039,
	K45 = 0x655b59c3, K46 = 0x8f0ccc92, K47 = 0xffeff47d, K48 = 0x85845dd1,
	K49 = 0x6fa87e4f, K410 = 0xfe2ce6e0, K411 = 0xa3014314, K412 = 0x4e0811a1,
	K413 = 0xf7537e82, K414 = 0xbd3af235, K415 = 0x2ad7d2bb, K416 = 0xeb86d391
};


void Md5::reset()
{
	this->a0 = 0x67452301;
	this->b0 = 0xefcdab89;
	this->c0 = 0x98badcfe;
	this->d0 = 0x10325476;
	this->HashFunctionInterface::length = 0;
	this->HashFunctionInterface::lastBlockOffset = 0;
}


void Md5::transform(const uint8_t *data, size_t length)
{ this->HashFunctionInterface::transform(data, length); }


void Md5::finalize()
{ this->HashFunctionInterface::finalize(); }


std::string Md5::toString(const uint8_t *data, size_t length)
{
	Md5 md5;
	md5.transform(data, length);
	md5.finalize();
	return md5.toString();
}


std::string Md5::toString(const char *data, size_t length)
{ return toString(reinterpret_cast<const uint8_t *>(data), length); }


std::string Md5::toString(const std::string &data)
{ return toString(data.data(), data.length()); }


std::string Md5::toString()
{
	uint8_t buffer[16];
	std::string result;
	result.resize(32);
	char *out = &result[0];

	Convert::splitQuadBytes(a0, &buffer[0]);
	Convert::splitQuadBytes(b0, &buffer[4]);
	Convert::splitQuadBytes(c0, &buffer[8]);
	Convert::splitQuadBytes(d0, &buffer[12]);

	for (size_t i = 0; i < 16; ++i)
	{
		uint8_t low = buffer[i] & 0xf;
		uint8_t high = buffer[i] >> 4;
		out[0] = (char)Convert::byteToHex(high);
		out[1] = (char)Convert::byteToHex(low);
		out += 2;
	}

	return result;
}


void Md5::transformBlock(const uint8_t *block)
{
	uint32_t a = a0, b = b0, c = c0, d = d0, m[16];
	const uint8_t *p = block;

	for (int i = 0; i < 16; ++i)
	{
		uint8_t b0 = p[0];
		uint8_t b1 = p[1];
		uint8_t b2 = p[2];
		uint8_t b3 = p[3];
		p += 4;
		m[i] = Convert::combineQuadBytes(b0, b1, b2, b3);
	}

	ff(a, b, c, d, m[ 0], S11, (uint32_t)K11);
	ff(d, a, b, c, m[ 1], S12, (uint32_t)K12);
	ff(c, d, a, b, m[ 2], S13, (uint32_t)K13);
	ff(b, c, d, a, m[ 3], S14, (uint32_t)K14);
	ff(a, b, c, d, m[ 4], S11, (uint32_t)K15);
	ff(d, a, b, c, m[ 5], S12, (uint32_t)K16);
	ff(c, d, a, b, m[ 6], S13, (uint32_t)K17);
	ff(b, c, d, a, m[ 7], S14, (uint32_t)K18);
	ff(a, b, c, d, m[ 8], S11, (uint32_t)K19);
	ff(d, a, b, c, m[ 9], S12, (uint32_t)K110);
	ff(c, d, a, b, m[10], S13, (uint32_t)K111);
	ff(b, c, d, a, m[11], S14, (uint32_t)K112);
	ff(a, b, c, d, m[12], S11, (uint32_t)K113);
	ff(d, a, b, c, m[13], S12, (uint32_t)K114);
	ff(c, d, a, b, m[14], S13, (uint32_t)K115);
	ff(b, c, d, a, m[15], S14, (uint32_t)K116);

	gg(a, b, c, d, m[ 1], S21, (uint32_t)K21);
	gg(d, a, b, c, m[ 6], S22, (uint32_t)K22);
	gg(c, d, a, b, m[11], S23, (uint32_t)K23);
	gg(b, c, d, a, m[ 0], S24, (uint32_t)K24);
	gg(a, b, c, d, m[ 5], S21, (uint32_t)K25);
	gg(d, a, b, c, m[10], S22, (uint32_t)K26);
	gg(c, d, a, b, m[15], S23, (uint32_t)K27);
	gg(b, c, d, a, m[ 4], S24, (uint32_t)K28);
	gg(a, b, c, d, m[ 9], S21, (uint32_t)K29);
	gg(d, a, b, c, m[14], S22, (uint32_t)K210);
	gg(c, d, a, b, m[ 3], S23, (uint32_t)K211);
	gg(b, c, d, a, m[ 8], S24, (uint32_t)K212);
	gg(a, b, c, d, m[13], S21, (uint32_t)K213);
	gg(d, a, b, c, m[ 2], S22, (uint32_t)K214);
	gg(c, d, a, b, m[ 7], S23, (uint32_t)K215);
	gg(b, c, d, a, m[12], S24, (uint32_t)K216);

	hh(a, b, c, d, m[ 5], S31, (uint32_t)K31);
	hh(d, a, b, c, m[ 8], S32, (uint32_t)K32);
	hh(c, d, a, b, m[11], S33, (uint32_t)K33);
	hh(b, c, d, a, m[14], S34, (uint32_t)K34);
	hh(a, b, c, d, m[ 1], S31, (uint32_t)K35);
	hh(d, a, b, c, m[ 4], S32, (uint32_t)K36);
	hh(c, d, a, b, m[ 7], S33, (uint32_t)K37);
	hh(b, c, d, a, m[10], S34, (uint32_t)K38);
	hh(a, b, c, d, m[13], S31, (uint32_t)K39);
	hh(d, a, b, c, m[ 0], S32, (uint32_t)K310);
	hh(c, d, a, b, m[ 3], S33, (uint32_t)K311);
	hh(b, c, d, a, m[ 6], S34, (uint32_t)K312);
	hh(a, b, c, d, m[ 9], S31, (uint32_t)K313);
	hh(d, a, b, c, m[12], S32, (uint32_t)K314);
	hh(c, d, a, b, m[15], S33, (uint32_t)K315);
	hh(b, c, d, a, m[ 2], S34, (uint32_t)K316);

	ii(a, b, c, d, m[ 0], S41, (uint32_t)K41);
	ii(d, a, b, c, m[ 7], S42, (uint32_t)K42);
	ii(c, d, a, b, m[14], S43, (uint32_t)K43);
	ii(b, c, d, a, m[ 5], S44, (uint32_t)K44);
	ii(a, b, c, d, m[12], S41, (uint32_t)K45);
	ii(d, a, b, c, m[ 3], S42, (uint32_t)K46);
	ii(c, d, a, b, m[10], S43, (uint32_t)K47);
	ii(b, c, d, a, m[ 1], S44, (uint32_t)K48);
	ii(a, b, c, d, m[ 8], S41, (uint32_t)K49);
	ii(d, a, b, c, m[15], S42, (uint32_t)K410);
	ii(c, d, a, b, m[ 6], S43, (uint32_t)K411);
	ii(b, c, d, a, m[13], S44, (uint32_t)K412);
	ii(a, b, c, d, m[ 4], S41, (uint32_t)K413);
	ii(d, a, b, c, m[11], S42, (uint32_t)K414);
	ii(c, d, a, b, m[ 2], S43, (uint32_t)K415);
	ii(b, c, d, a, m[ 9], S44, (uint32_t)K416);

	a0 += a; b0 += b; c0 += c; d0 += d;
}


void Md5::addLengthMessage()
{
	HashFunctionInterface::addLengthMessageLittleEndian(
		this->HashFunctionInterface::length,
		this->HashFunctionInterface::lastBlock);
}
