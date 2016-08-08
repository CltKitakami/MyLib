#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <pthread.h>
#else
#error "Unknow OS"
#endif

#include "pattern/NonCopyable.hpp"


class Mutex : private NonCopyable
{
public:
#ifdef _WIN32
	Mutex()
	{
#if (_WIN32_WINNT >= 0x0600)
		::InitializeCriticalSectionEx(&this->cs, 1000, 0);
#else
		::InitializeCriticalSection(&this->cs);
#endif
	}

	~Mutex() { ::DeleteCriticalSection(&this->cs); }

	void lock() { ::EnterCriticalSection(&this->cs); }
	bool trylock() { return ::TryEnterCriticalSection(&this->cs) != 0; }
	void unlock() { ::LeaveCriticalSection(&this->cs); }

	CRITICAL_SECTION & self() { return this->cs; }

#else
	Mutex() : m(PTHREAD_MUTEX_INITIALIZER) {}
	~Mutex() { ::pthread_mutex_destroy(&this->m); }

	void lock() { ::pthread_mutex_lock(&this->m); }
	bool trylock() { return ::pthread_mutex_trylock(&this->m) == 0; }
	void unlock() { ::pthread_mutex_unlock(&this->m); }

	pthread_mutex_t & self() { return this->m; }
#endif

private:
	bool isLocked;
#ifdef _WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t m;
#endif
};


template <class T>
class ScopedMutex : private NonCopyable
{
	T *m;
	ScopedMutex() {}
public:
	ScopedMutex(T *mutex) : m(mutex) { this->m->lock(); }
	~ScopedMutex() { this->m->unlock(); }
};

#endif
