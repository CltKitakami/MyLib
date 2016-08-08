#include <cstring>
#include "Sha1.hpp"
#include "Base64.hpp"
#include "common/Convert.hpp"


#define SHA1_ROL(value, bits) \
	(((value) << (bits)) | (((value) & 0xffffffff) >> (32 - (bits))))
#define SHA1_BLK(i) \
	(word[i & 15] = SHA1_ROL(word[(i + 13) & 15] ^ word[(i + 8) & 15] ^ \
		word[(i + 2) & 15] ^ word[i & 15], 1))

#define SHA1_R0(v, w, x, y, z, i) z += ((w & (x ^ y)) ^ y) + word[i] + \
	0x5a827999 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R1(v, w, x, y, z, i) z += ((w & (x ^ y)) ^ y) + SHA1_BLK(i) + \
	0x5a827999 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R2(v, w, x, y, z, i) z += (w ^ x ^ y) + SHA1_BLK(i) + \
	0x6ed9eba1 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R3(v, w, x, y, z, i) z += (((w | x) & y) | (w & x)) + SHA1_BLK(i) + \
	0x8f1bbcdc + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R4(v, w, x, y, z, i) z += (w ^ x ^ y) + SHA1_BLK(i) + \
	0xca62c1d6 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);


enum CONSTANTS
{
	H0 = 0x67452301,
	H1 = 0xefcdab89,
	H2 = 0x98badcfe,
	H3 = 0x10325476,
	H4 = 0xc3d2e1f0,
};


void Sha1::reset()
{
	this->digest[0] = (uint32_t)H0;
	this->digest[1] = (uint32_t)H1;
	this->digest[2] = (uint32_t)H2;
	this->digest[3] = (uint32_t)H3;
	this->digest[4] = (uint32_t)H4;
	this->HashFunctionInterface::length = 0;
	this->HashFunctionInterface::lastBlockOffset = 0;
}


void Sha1::transform(const uint8_t *data, size_t length)
{ this->HashFunctionInterface::transform(data, length); }


void Sha1::finalize()
{ this->HashFunctionInterface::finalize(); }


void Sha1::Sha1::transformBlock(const uint8_t *block)
{
	uint32_t a = this->digest[0];
	uint32_t b = this->digest[1];
	uint32_t c = this->digest[2];
	uint32_t d = this->digest[3];
	uint32_t e = this->digest[4];

	uint32_t word[16];
	const uint8_t *p = block;

	for (int i = 0; i < 16; ++i)
	{
		uint8_t b3 = p[0];
		uint8_t b2 = p[1];
		uint8_t b1 = p[2];
		uint8_t b0 = p[3];
		p += 4;
		word[i] = Convert::combineQuadBytes(b0, b1, b2, b3);
	}

	SHA1_R0(a, b, c, d, e, 0);
	SHA1_R0(e, a, b, c, d, 1);
	SHA1_R0(d, e, a, b, c, 2);
	SHA1_R0(c, d, e, a, b, 3);
	SHA1_R0(b, c, d, e, a, 4);
	SHA1_R0(a, b, c, d, e, 5);
	SHA1_R0(e, a, b, c, d, 6);
	SHA1_R0(d, e, a, b, c, 7);
	SHA1_R0(c, d, e, a, b, 8);
	SHA1_R0(b, c, d, e, a, 9);
	SHA1_R0(a, b, c, d, e, 10);
	SHA1_R0(e, a, b, c, d, 11);
	SHA1_R0(d, e, a, b, c, 12);
	SHA1_R0(c, d, e, a, b, 13);
	SHA1_R0(b, c, d, e, a, 14);
	SHA1_R0(a, b, c, d, e, 15);

	SHA1_R1(e, a, b, c, d, 16);
	SHA1_R1(d, e, a, b, c, 17);
	SHA1_R1(c, d, e, a, b, 18);
	SHA1_R1(b, c, d, e, a, 19);

	SHA1_R2(a, b, c, d, e, 20);
	SHA1_R2(e, a, b, c, d, 21);
	SHA1_R2(d, e, a, b, c, 22);
	SHA1_R2(c, d, e, a, b, 23);
	SHA1_R2(b, c, d, e, a, 24);
	SHA1_R2(a, b, c, d, e, 25);
	SHA1_R2(e, a, b, c, d, 26);
	SHA1_R2(d, e, a, b, c, 27);
	SHA1_R2(c, d, e, a, b, 28);
	SHA1_R2(b, c, d, e, a, 29);
	SHA1_R2(a, b, c, d, e, 30);
	SHA1_R2(e, a, b, c, d, 31);
	SHA1_R2(d, e, a, b, c, 32);
	SHA1_R2(c, d, e, a, b, 33);
	SHA1_R2(b, c, d, e, a, 34);
	SHA1_R2(a, b, c, d, e, 35);
	SHA1_R2(e, a, b, c, d, 36);
	SHA1_R2(d, e, a, b, c, 37);
	SHA1_R2(c, d, e, a, b, 38);
	SHA1_R2(b, c, d, e, a, 39);

	SHA1_R3(a, b, c, d, e, 40);
	SHA1_R3(e, a, b, c, d, 41);
	SHA1_R3(d, e, a, b, c, 42);
	SHA1_R3(c, d, e, a, b, 43);
	SHA1_R3(b, c, d, e, a, 44);
	SHA1_R3(a, b, c, d, e, 45);
	SHA1_R3(e, a, b, c, d, 46);
	SHA1_R3(d, e, a, b, c, 47);
	SHA1_R3(c, d, e, a, b, 48);
	SHA1_R3(b, c, d, e, a, 49);
	SHA1_R3(a, b, c, d, e, 50);
	SHA1_R3(e, a, b, c, d, 51);
	SHA1_R3(d, e, a, b, c, 52);
	SHA1_R3(c, d, e, a, b, 53);
	SHA1_R3(b, c, d, e, a, 54);
	SHA1_R3(a, b, c, d, e, 55);
	SHA1_R3(e, a, b, c, d, 56);
	SHA1_R3(d, e, a, b, c, 57);
	SHA1_R3(c, d, e, a, b, 58);
	SHA1_R3(b, c, d, e, a, 59);

	SHA1_R4(a, b, c, d, e, 60);
	SHA1_R4(e, a, b, c, d, 61);
	SHA1_R4(d, e, a, b, c, 62);
	SHA1_R4(c, d, e, a, b, 63);
	SHA1_R4(b, c, d, e, a, 64);
	SHA1_R4(a, b, c, d, e, 65);
	SHA1_R4(e, a, b, c, d, 66);
	SHA1_R4(d, e, a, b, c, 67);
	SHA1_R4(c, d, e, a, b, 68);
	SHA1_R4(b, c, d, e, a, 69);
	SHA1_R4(a, b, c, d, e, 70);
	SHA1_R4(e, a, b, c, d, 71);
	SHA1_R4(d, e, a, b, c, 72);
	SHA1_R4(c, d, e, a, b, 73);
	SHA1_R4(b, c, d, e, a, 74);
	SHA1_R4(a, b, c, d, e, 75);
	SHA1_R4(e, a, b, c, d, 76);
	SHA1_R4(d, e, a, b, c, 77);
	SHA1_R4(c, d, e, a, b, 78);
	SHA1_R4(b, c, d, e, a, 79);

	this->digest[0] += a;
	this->digest[1] += b;
	this->digest[2] += c;
	this->digest[3] += d;
	this->digest[4] += e;
}


std::string Sha1::toHexString(const char *data, size_t length)
{ return toHexString(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha1::toHexString(const std::string &data)
{ return toHexString(data.data(), data.length()); }


std::string Sha1::toBase64String(const char *data, size_t length)
{ return toBase64String(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha1::toBase64String(const std::string &data)
{ return toBase64String(data.data(), data.length()); }


std::string Sha1::toHexString()
{
	std::string result;
	result.resize(40);
	char *p = &result[39];

	for (int i = 4; i >= 0; --i)
	{
		uint32_t bits = this->digest[i];

		for (int j = 0; j < 8; ++j)
		{
			*p-- = (char)Convert::byteToHex(bits & 0xf);
			bits >>= 4;
		}
	}

	return result;
}


std::string Sha1::toBase64String()
{
	std::string raw;
	raw.resize(20);
	char *p = &raw[19];

	for (int i = 4; i >= 0; --i)
	{
		uint32_t bits = this->digest[i];

		for (int j = 0; j < 4; ++j)
		{
			*p-- = (char)bits;
			bits >>= 8;
		}
	}

	return Base64::encode(raw);
}


std::string Sha1::toHexString(const uint8_t *data, size_t length)
{
	Sha1 sha1;
	sha1.transform(data, length);
	sha1.finalize();
	return sha1.toHexString();
}


std::string Sha1::toBase64String(const uint8_t *data, size_t length)
{
	Sha1 sha1;
	sha1.transform(data, length);
	sha1.finalize();
	return sha1.toBase64String();
}


void Sha1::addLengthMessage()
{
	HashFunctionInterface::addLengthMessageBigEndian(
		this->HashFunctionInterface::length,
		this->HashFunctionInterface::lastBlock);
}
