#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#elif defined(__linux__)
#include <pthread.h>
#include <unistd.h>
#else
#error "Unknow OS"
#endif

#include "pattern/NonCopyable.hpp"


class Thread : private NonCopyable
{
public:
#ifdef _WIN32
#ifndef _MSC_VER
	typedef unsigned int __attribute__((__stdcall__))(*Routine)(void *arg);
#else
	typedef unsigned int (__stdcall *Routine)(void *arg);
#endif
#else
	typedef void *(*Routine)(void *arg);
#endif

#ifdef _WIN32
	enum Priority
	{
		BACKGROUND_BEGIN = 0x00010000,
		BACKGROUND_END = 0x00020000,
		ABOVE_NORMAL = 1,
		BELOW_NORMAL = -1,
		HIGHEST = 2,
		IDLE = -15,
		LOWEST = -2,
		NORMAL = 0,
		TIME_CRITICAL = 15
	};
#endif

	enum State { UNSTART, RUNNING, STOPPED };

	Thread(Routine routine = nullptr, void *arg = nullptr);
	virtual ~Thread();

	int start();
	int join();
	int stop();

	static void exit(unsigned int code);

	void setRoutine(Routine routine, void *arg = nullptr);

	void setBackground(bool value)
	{ this->isBackground = value; }	// Warning. Short thread shouldn't set background!
	bool getBackground() const { return this->isBackground; }

#ifdef _WIN32
	void setPriority(Thread::Priority value);
#else
	void setPriority(int value);
#endif

	Thread::State getState() const { return this->state; };

#ifdef _WIN32
	int resume();
	int suspend();

	HANDLE self() { return this->handle; }

	static uint32_t getId() { return ::GetCurrentThreadId(); }
#else
	pthread_t self() { return this->tid; }

	static pthread_t getId() { return ::pthread_self(); }
#endif

	static void sleep(int milliSeconds);

private:
#ifdef _WIN32
	HANDLE beginThreadEx();

	static void endThreadEx(unsigned int code = 0) { ::_endthreadex(code); }

	HANDLE handle;
#else
	pthread_t tid;
#endif
	Routine routine;
	void *arg;
	bool isBackground;
	Thread::State state;
};

#endif
