#include <cstring>
#include "Sha512.hpp"
#include "Base64.hpp"
#include "common/Convert.hpp"


#define ROTATE(value, bits) \
	(((value) >> (bits)) | (((value) & 0xffffffffffffffff) << (64 - (bits))))

#define Sigma0(x) (ROTATE((x), 28) ^ ROTATE((x), 34) ^ ROTATE((x), 39))
#define Sigma1(x) (ROTATE((x), 14) ^ ROTATE((x), 18) ^ ROTATE((x), 41))
#define sigma0(x) (ROTATE((x), 1)  ^ ROTATE((x), 8)  ^ ((x) >> 7))
#define sigma1(x) (ROTATE((x), 19) ^ ROTATE((x), 61) ^ ((x) >> 6))

#define Ch(x,y,z) (((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define ROUND_00_15(i, a, b, c, d, e, f, g, h) do {\
        temp += h + Sigma1(e) + Ch(e, f, g) + k512[i];\
        h = Sigma0(a) + Maj(a, b, c);\
        d += temp;\
        h += temp;\
} while (0)

#define ROUND_16_80(i, j, a, b, c, d, e, f, g, h) do {\
        s0 = word[(j + 1) & 0x0f];\
        s0 = sigma0(s0);\
        s1 = word[(j + 14) & 0x0f];\
        s1 = sigma1(s1);\
        temp = word[(j) & 0x0f] += s0 + s1 + word[(j + 9) & 0x0f];\
        ROUND_00_15(i + j, a, b, c, d, e, f, g, h);\
} while (0)


#define H0 0x6a09e667f3bcc908
#define H1 0xbb67ae8584caa73b
#define H2 0x3c6ef372fe94f82b
#define H3 0xa54ff53a5f1d36f1
#define H4 0x510e527fade682d1
#define H5 0x9b05688c2b3e6c1f
#define H6 0x1f83d9abfb41bd6b
#define H7 0x5be0cd19137e2179


const uint64_t Sha512::k512[80] =
{
    0x428a2f98d728ae22, 0x7137449123ef65cd,
    0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019,
    0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe,
    0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
    0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
    0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
    0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210,
    0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926,
    0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8,
    0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001,
    0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910,
    0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
    0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
    0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60,
    0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9,
    0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207,
    0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6,
    0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493,
    0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
    0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};


void Sha512::reset()
{
	this->digest[0] = H0;
	this->digest[1] = H1;
	this->digest[2] = H2;
	this->digest[3] = H3;
	this->digest[4] = H4;
	this->digest[5] = H5;
	this->digest[6] = H6;
	this->digest[7] = H7;
	this->lengthHigh = 0;
	this->HashFunctionInterface::length = 0;
	this->HashFunctionInterface::lastBlockOffset = 0;
}


void Sha512::transform(const uint8_t *data, size_t length)
{
	uint64_t lengthLow = this->HashFunctionInterface::length;

	this->HashFunctionInterface::transform(data, length);

	if (this->HashFunctionInterface::length < lengthLow)	// overflow
		this->lengthHigh += 1;
}


void Sha512::finalize()
{ this->HashFunctionInterface::finalize(); }


void Sha512::transformBlock(const uint8_t *block)
{
	uint64_t a = this->digest[0];
	uint64_t b = this->digest[1];
	uint64_t c = this->digest[2];
	uint64_t d = this->digest[3];
	uint64_t e = this->digest[4];
	uint64_t f = this->digest[5];
	uint64_t g = this->digest[6];
	uint64_t h = this->digest[7];

	uint64_t s0, s1, temp;
	uint64_t word[16];
	const uint8_t *p = block;

	for (int i = 0; i < 16; ++i)
	{
		uint8_t b7 = p[0];
		uint8_t b6 = p[1];
		uint8_t b5 = p[2];
		uint8_t b4 = p[3];
		uint8_t b3 = p[4];
		uint8_t b2 = p[5];
		uint8_t b1 = p[6];
		uint8_t b0 = p[7];
		uint32_t low = Convert::combineQuadBytes(b0, b1, b2, b3);
		uint32_t high = Convert::combineQuadBytes(b4, b5, b6, b7);
		p += 8;
		word[i] = ((uint64_t)high << 32) + (uint64_t)low;
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

	for (int i = 16; i < 80; i += 16)
	{
		ROUND_16_80(i, 0, a, b, c, d, e, f, g, h);
		ROUND_16_80(i, 1, h, a, b, c, d, e, f, g);
		ROUND_16_80(i, 2, g, h, a, b, c, d, e, f);
		ROUND_16_80(i, 3, f, g, h, a, b, c, d, e);
		ROUND_16_80(i, 4, e, f, g, h, a, b, c, d);
		ROUND_16_80(i, 5, d, e, f, g, h, a, b, c);
		ROUND_16_80(i, 6, c, d, e, f, g, h, a, b);
		ROUND_16_80(i, 7, b, c, d, e, f, g, h, a);
		ROUND_16_80(i, 8, a, b, c, d, e, f, g, h);
		ROUND_16_80(i, 9, h, a, b, c, d, e, f, g);
		ROUND_16_80(i, 10, g, h, a, b, c, d, e, f);
		ROUND_16_80(i, 11, f, g, h, a, b, c, d, e);
		ROUND_16_80(i, 12, e, f, g, h, a, b, c, d);
		ROUND_16_80(i, 13, d, e, f, g, h, a, b, c);
		ROUND_16_80(i, 14, c, d, e, f, g, h, a, b);
		ROUND_16_80(i, 15, b, c, d, e, f, g, h, a);
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


std::string Sha512::toHexString(const char *data, size_t length)
{ return toHexString(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha512::toHexString(const std::string &data)
{ return toHexString(data.data(), data.length()); }


std::string Sha512::toBase64String(const char *data, size_t length)
{ return toBase64String(reinterpret_cast<const uint8_t *>(data), length); }


std::string Sha512::toBase64String(const std::string &data)
{ return toBase64String(data.data(), data.length()); }


std::string Sha512::toHexString()
{
	std::string result;
	result.resize(128);
	char *p = &result[127];

	for (int i = 7; i >= 0; --i)
	{
		uint64_t bits = this->digest[i];

		for (int j = 0; j < 16; ++j)
		{
			*p-- = (char)Convert::byteToHex(bits & 0xf);
			bits >>= 4;
		}
	}

	return result;
}


std::string Sha512::toBase64String()
{
	std::string raw;
	raw.resize(64);
	char *p = &raw[63];

	for (int i = 7; i >= 0; --i)
	{
		uint64_t bits = this->digest[i];

		for (int j = 0; j < 8; ++j)
		{
			*p-- = (char)bits;
			bits >>= 8;
		}
	}

	return Base64::encode(raw);
}


std::string Sha512::toHexString(const uint8_t *data, size_t length)
{
	Sha512 sha512;
	sha512.transform(data, length);
	sha512.finalize();
	return sha512.toHexString();
}


std::string Sha512::toBase64String(const uint8_t *data, size_t length)
{
	Sha512 sha512;
	sha512.transform(data, length);
	sha512.finalize();
	return sha512.toBase64String();
}


void Sha512::addLengthMessage()
{
	uint64_t bitsLow = this->HashFunctionInterface::length << 3;
	uint64_t bitsHigh = (this->lengthHigh << 3) +
		((this->HashFunctionInterface::length >> (64 - 3)) & 0x7);
	uint8_t *p = this->HashFunctionInterface::lastBlock + BLOCK_LENGTH - 1;

	for (int i = LENGTH_MESSAGE / 2; i > 0; --i)
	{
		*p-- = (uint8_t)bitsLow;
		bitsLow >>= 8;
	}

	for (int i = LENGTH_MESSAGE / 2; i > 0; --i)
	{
		*p-- = (uint8_t)bitsHigh;
		bitsHigh >>= 8;
	}
}
