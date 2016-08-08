#ifndef _MSC_VER
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <string>

#include "Stopwatch.hpp"


#ifdef _MSC_VER
static int gettimeofday(struct timeval *tv, struct timezone *)
{
	FILETIME ft;
	::GetSystemTimeAsFileTime(&ft);

	long long ms = ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    ms /= 10;  // microseconds

    tv->tv_sec = (long)(ms / 1000000UL);
    tv->tv_usec = (long)(ms % 1000000UL);

    return 0;
}
#endif


void Stopwatch::start()
{
	(void)::gettimeofday(&this->begin, 0);
}


void Stopwatch::stop()
{
	(void)::gettimeofday(&this->end, 0);
}


uint32_t Stopwatch::getMillis()
{
	struct timeval t = getElapsedTime();
	return (uint32_t)(t.tv_sec * 1e3 + t.tv_usec / 1e3);
}


uint32_t Stopwatch::getMicros()
{
	struct timeval t = getElapsedTime();
	return (uint32_t)(t.tv_sec * 1e6 + t.tv_usec);
}


struct timeval Stopwatch::getElapsedTime()
{
	struct timeval t;
	t.tv_sec = this->end.tv_sec - this->begin.tv_sec;
	t.tv_usec = this->end.tv_usec - this->begin.tv_usec;
	return t;
}
