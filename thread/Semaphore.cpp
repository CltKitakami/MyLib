#include "Semaphore.hpp"

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#include "common/Exception.hpp"


Semaphore::Semaphore(int maxValue)
{
	if (maxValue <= 0)
		maxValue = 1;

#ifdef _WIN32
	this->self = ::CreateSemaphore(nullptr, maxValue, maxValue, nullptr);
#else
	::sem_init(&this->self, 0, (unsigned int)maxValue);
#endif
}


Semaphore::~Semaphore()
{
#ifdef _WIN32
	::CloseHandle(&this->self);
#else
	::sem_destroy(&this->self);
#endif
}


void Semaphore::acquire()
{
#ifdef _WIN32
	::WaitForSingleObject(this->self, INFINITE);
#else
	::sem_wait(&this->self);
#endif
}


bool Semaphore::timedAcquire(int millisecond)
{
#ifdef _WIN32
	return ::WaitForSingleObject(this->self, (unsigned int)millisecond) == WAIT_OBJECT_0;
#else
	struct timeval now;
	struct timespec ts;

	::gettimeofday(&now, nullptr);

	ldiv_t result = std::div(now.tv_usec + millisecond * 1000, 1e6);
	long us = result.rem;
	long s = result.quot + now.tv_sec;

	ts.tv_sec = s;
	ts.tv_nsec = us * 1000;

	return ::sem_timedwait(&this->self, &ts) == 0;
#endif
}


void Semaphore::release()
{
#ifdef _WIN32
	::ReleaseSemaphore(this->self, 1, nullptr);
#else
	::sem_post(&this->self);
#endif
}
