#ifndef _CONDITION_HPP_
#define _CONDITION_HPP_

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <pthread.h>
#else
#error "Unknow OS"
#endif

#include "Mutex.hpp"
#include "pattern/NonCopyable.hpp"


// #define HAS_CONDITION_VARIABLE

#if defined(_WIN32) && !defined(HAS_CONDITION_VARIABLE)
#define USE_FAKE_CONDITION_VARIABLE
class ConditionQueue;
#endif


class Condition : private NonCopyable
{
public:
	Condition();
	~Condition();

	void wait(Mutex &mutex);
	bool timedWait(Mutex &mutex, int millisecond);
	void notifyOne();
	void notifyAll();

private:
#if defined(HAS_CONDITION_VARIABLE)
	CONDITION_VARIABLE cond;
#elif defined(USE_FAKE_CONDITION_VARIABLE)
	ConditionQueue *cq;
#else
	pthread_cond_t cond;
#endif
};

#endif
