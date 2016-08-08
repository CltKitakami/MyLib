#ifndef _STOPWATCH_HPP_
#define _STOPWATCH_HPP_

#include <cstdint>

#ifndef _MSC_VER
#include <sys/time.h>
#else
#include <windows.h>
#endif


class Stopwatch
{
public:
	void start();
	void stop();
	uint32_t getMillis();
	uint32_t getMicros();

private:
    struct timeval begin, end;

    struct timeval getElapsedTime();
};

#endif
