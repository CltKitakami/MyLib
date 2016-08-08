#include <ctime>

#include "MtRandom.hpp"


void MtRandom::initialize()
{
	this->mt[0] = (uint32_t)std::time(0);

	for (uint32_t i = 1; i < 623; ++i)
	{
		this->mt[i] = 0x6c078965 *
			(this->mt[i - 1] ^ (this->mt[i - 1] >> 30)) + i;
	}
}


void MtRandom::generate()
{
	for (uint32_t i = 0; i < 623; ++i)
	{
		uint32_t y = (this->mt[i] & 0x80000000) +
			(this->mt[(i + 1) % 624] & 0x7fffffff);

		this->mt[i] = this->mt[(i + 397) % 624] ^ (y >> 1);

		if ((y & 0x1) != 0)
			this->mt[i] ^= 0x9908b0df;
	}
}

uint32_t MtRandom::extract()
{
	if (this->mtIndex == 0)
		this->generate();

	uint32_t y = this->mt[this->mtIndex];
	y ^= y >> 11;
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= y >> 18;
	this->mtIndex = (this->mtIndex + 1) % 624;

	return y;
}


uint32_t MtRandom::next()
{
	return this->extract();
}


uint32_t MtRandom::next(uint32_t start, uint32_t end)
{
	return this->next() % end + start;
}
