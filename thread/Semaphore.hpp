#ifndef _SEMAPHORE_HPP_
#define _SEMAPHORE_HPP_

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <semaphore.h>
#else
#error "Unknow OS"
#endif

#include "pattern/NonCopyable.hpp"


class Semaphore : private NonCopyable
{
public:
	Semaphore(int maxValue = 1);
	~Semaphore();

	void acquire();
	bool timedAcquire(int millisecond);
	void release();

private:
#ifdef _WIN32
	HANDLE self;
#else
	sem_t self;
#endif
};

#endif
