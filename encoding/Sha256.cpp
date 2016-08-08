#include <cstring>
#include "Sha256.hpp"
#include "Base64.hpp"
#include "common/Convert.hpp"


#define ROTATE(value, bits) \
	(((value) << (bits)) | (((value) & 0xffffffff) >> (32 - (bits))))

#define Sigma0(x) (ROTATE((x), 30) ^ ROTATE((x), 19) ^ ROTATE((x), 10))
#define Sigma1(x) (ROTATE((x), 26) ^ ROTATE((x), 21) ^ ROTATE((x), 7))
#define sigma0(x) (ROTATE((x), 25) ^ ROTATE((x), 14) ^ ((x) >> 3))
#define sigma1(x) (ROTATE((x), 15) ^ ROTATE((x), 13) ^ ((x) >> 10))

#define Ch(x,y,z) (((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define ROUND_00_15(i, a, b, c, d, e, f, g, h) do {\
    temp += h + Sigma1(e) + Ch(e,f,g) + k256[i];\
    h = Sigma0(a) + Maj(a,b,c);\
    d += temp;\
    h += temp;\
} while (0)

#define ROUND_16_63(i, a, b, c, d, e, f, g, h) do {\
        s0 = word[(i + 1) & 0x0f];\
        s0 = sigma0(s0);\
        s1 = word[(i + 14) & 0x0f];\
        s1 = sigma1(s1);\
        temp = word[(i) & 0x0f] += s0 + s1 + word[(i + 9) & 0x0f];\
        ROUND_00_15(i, a, b, c, d, e, f, g, h);\
} while (0)


enum CONSTANTS
{
	H0 = 0x6a09e667,
	H1 = 0xbb67ae85,
	H2 = 0x3c6ef372,
	H3 = 0xa54ff53a,
	H4 = 0x510e527f,
	H5 = 0x9b05688c,
	H6 = 0x1f83d9ab,
	H7 = 0x5be0cd19
};


const uint32_t Sha256::k256[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


void Sha256::reset()
{
	this->digest[0] = (uint32_t)H0;
	this->digest[1] = (uint32_t)H1;
	this->digest[2] = (uint32_t)H2;
	this->digest[3] = (uint32_t)H3;
	this->digest[4] = (uint32_t)H4;
	this->digest[5] = (uint32_t)H5;
	this->digest[6] = (uint32_t)H6;
	this->digest[7] = (uint32_t)H7;
	this->HashFunctionInterface::length = 0;
	this->HashFunctionInterface::lastBlockOffset = 0;
}


void Sha256::transform(const uint8_t *data, size_t length)
{ this->HashFunctionInterface::transform(data, length); }


void Sha256::finalize()
{ this->HashFunctionInterface::finalize(); }


void Sha256::transformBlock(const uint8_t *block)
{
	uint32_t a = this->digest[0];
	uint32_t b = this->digest[1];
	uint32_t c = this->digest[2];
	uint32_t d = this->digest[3];
	uint32_t e = this->digest[4];
	uint32_t f = this->digest[5];
	uint32_t g = this->digest[6];
	uint32_t h = this->digest[7];

	uint32_t s0, s1, temp;
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

	temp = word[0]; ROUND_00_15(0, a, b, c, d, e, f, g, h);
	temp = word[1]; ROUND_00_15(1, h, a, b, c, d, e, f, g);
	temp = word[2]; ROUND_00_15(2, g, h, a, b, c, d, e, f);
	temp = word[3]; ROUND_00_15(3, f, g, h, a, b, c, d, e);
	temp = word[4]; ROUND_00_15(4, e, f, g, h, a, b, c, d);
	temp = word[5]; ROUND_00_15(5, d, e, f, g, h, a, b, c);
	temp = word[6]; ROUND_00_15(6, c, d, e, f, g, h, a, b);
	temp = word[7]; ROUND_00_15(7, b, c, d, e, f, g, h, a);
	temp = word[8]; ROUND_00_15(8, a, b, c, d, e, f, g, h);
	temp = word[9]; ROUND_00_15(9, h, a, b, c, d, e, f, g);
	temp = word[10]; ROUND_00_15(10, g, h, a, b, c, d, e, f);
	temp = word[11]; ROUND_00_15(11, f, g, h, a, b, c, d, e);
	temp = word[12]; ROUND_00_15(12, e, f, g, h, a, b, c, d);
	temp = word[13]; ROUND_00_15(13, d, e, f, g, h, a, b, c);
	temp = word[14]; ROUND_00_15(14, c, d, e, f, g, h, a, b);
	temp = word[15]; ROUND_00_15(15, b, c, d, e, f, g, h, a);

	for (int i = 16; i < 64; i += 8)
	{
		ROUND_16_63(i + 0, a, b, c, d, e, f, g, h);
		ROUND_16_63(i + 1, h, a, b, c, d, e, f, g);
		ROUND_16_63(i + 2, g, h, a, b, c, d, e, f);
		ROUND_16_63(i + 3, f, g, h, a, b, c, d, e);
		ROUND_16_63(i + 4, e, f, g, h, a, b, c, d);
		ROUND_16_63(i + 5, d, e, f, g, h, a, b, c);
		ROUND_16_63(i + 6, c, d, e, f, g, h, a, b);
		ROUND_16_63(i + 7, b, c, d, e, f, g, h, a);
	}

	this->digest[0] += a;
	this->digest[1] += b;
	this->digest[2] += c;
	this->digest[3] += d;
	this->digest[4] += e;
	this->digest[5] += f;
	this->digest[6] += g;
	this->digest[7] += h;
}


std::string Sha256::toHexString(const char *data, size_t length)
{ return toHexString(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha256::toHexString(const std::string &data)
{ return toHexString(data.data(), data.length()); }


std::string Sha256::toBase64String(const char *data, size_t length)
{ return toBase64String(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha256::toBase64String(const std::string &data)
{ return toBase64String(data.data(), data.length()); }


std::string Sha256::toHexString()
{
	std::string result;
	result.resize(64);
	char *p = &result[63];

	for (int i = 7; i >= 0; --i)
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


std::string Sha256::toBase64String()
{
	std::string raw;
	raw.resize(32);
	char *p = &raw[31];

	for (int i = 7; i >= 0; --i)
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


std::string Sha256::toHexString(const uint8_t *data, size_t length)
{
	Sha256 sha256;
	sha256.transform(data, length);
	sha256.finalize();
	return sha256.toHexString();
}


std::string Sha256::toBase64String(const uint8_t *data, size_t length)
{
	Sha256 sha256;
	sha256.transform(data, length);
	sha256.finalize();
	return sha256.toBase64String();
}


void Sha256::addLengthMessage()
{
	HashFunctionInterface::addLengthMessageBigEndian(
		this->HashFunctionInterface::length,
		this->HashFunctionInterface::lastBlock);
}
