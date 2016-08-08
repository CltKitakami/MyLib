#ifndef _MD5_HPP_
#define _MD5_HPP_

#include "HashFunctionInterface.hpp"


class Md5 : public HashFunctionInterface<64, 8>
{
public:
	Md5() { this->reset(); }

	virtual void reset() override;

	virtual void transform(const uint8_t *data, size_t length) override;

	virtual void finalize() override;

	virtual std::string toString() override;

	static std::string toString(const uint8_t *data, size_t length);
	static std::string toString(const char *data, size_t length);
	static std::string toString(const std::string &data);

private:
	virtual void transformBlock(const uint8_t *block) override;
	virtual void addLengthMessage() override;

	static inline uint32_t leftRotate(uint32_t x, uint32_t n)
	{ return (x << n) | (x >> (32 - n)); }

	static inline uint32_t f(uint32_t x, uint32_t y, uint32_t z)
	{ return x & y | ~x & z; }

	static inline uint32_t g(uint32_t x, uint32_t y, uint32_t z)
	{ return x & z | y & ~z; }

	static inline uint32_t h(uint32_t x, uint32_t y, uint32_t z)
	{ return x ^ y ^ z; }

	static inline uint32_t i(uint32_t x, uint32_t y, uint32_t z)
	{ return y ^ (x | ~z); }

	static inline void ff(uint32_t &a, uint32_t b, uint32_t c,
		uint32_t d, uint32_t m, uint32_t s, uint32_t k)
	{ a = leftRotate(a+ f(b,c,d) + m + k, s) + b; }

	static inline void gg(uint32_t &a, uint32_t b, uint32_t c,
		uint32_t d, uint32_t m, uint32_t s, uint32_t k)
	{ a = leftRotate(a + g(b,c,d) + m + k, s) + b; }

	static inline void hh(uint32_t &a, uint32_t b, uint32_t c,
		uint32_t d, uint32_t m, uint32_t s, uint32_t k)
	{ a = leftRotate(a + h(b,c,d) + m + k, s) + b; }

	static inline void ii(uint32_t &a, uint32_t b, uint32_t c,
		uint32_t d, uint32_t m, uint32_t s, uint32_t k)
	{ a = leftRotate(a + i(b,c,d) + m + k, s) + b; }

	uint32_t a0, b0, c0, d0;
};

#endif
