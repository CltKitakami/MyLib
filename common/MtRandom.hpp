#ifndef _MTRANDOM_HPP_
#define _MTRANDOM_HPP_

#include <cstdint>


class MtRandom
{
public:
	MtRandom() { this->initialize(); }

	uint32_t next();
	uint32_t next(uint32_t start, uint32_t end);

private:
	uint32_t mt[624], mtIndex = 0;

	void initialize();
	void generate();
	uint32_t extract();
};

#endif
