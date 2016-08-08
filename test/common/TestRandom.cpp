#include <cstdint>
#include <random>
#include "common/Log.hpp"
#include "common/Random.hpp"
#include "common/MtRandom.hpp"
#include "common/Convert.hpp"


int main(int argc, char *argv[])
{
	Random::initialize();
	MtRandom mt19937;
	// std::mt19937 mtRand((uint32_t)Random::next());

	int n = 10;

	if (argc == 2)
		n = Convert::stringToInt(argv[1]);


	for (int i = 0; i < n; ++i)
	{
		// LOG.d(mtRand());
		LOG.d(mt19937.next());
	}

	return 0;
}
