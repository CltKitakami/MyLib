#ifndef _RANDOM_HPP_
#define _RANDOM_HPP_

#include <cstdlib>
#include <ctime>


class Random
{
public:
	static void initialize()
	{ std::srand((unsigned int)std::time(0)); }

	static int next() { return std::rand(); }
	static int next(int start, int end)
	{ return std::rand() % end + start; }
};

#endif
