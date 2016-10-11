#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <ctime>
#include <cstdlib>
#include <csignal>
#else
#error "Unknow OS"
#endif

#include "pattern/NonCopyable.hpp"


class Timer : NonCopyable
{
public:
	typedef void (*OnTimedEvent)(void *arg);

	Timer(OnTimedEvent onTimedEvent = nullptr, void *arg = nullptr) :
		self(0), interval(0), firstTime(0), isOneshot(false),
		onTimedEvent(onTimedEvent), arg(arg) {}

	virtual ~Timer();

	int getInterval() const { return this->interval; }
	void setInterval(int periodMillisecond, int firstTimeMillisecond = 0);

	void oneshotReset();

	virtual void start();
	virtual void stop();

	virtual void onTimed();

	void setOnTimedEvent(OnTimedEvent onTimedEvent, void *arg = nullptr)
	{ this->onTimedEvent = onTimedEvent; this->arg = arg; }

private:
#ifdef _WIN32
	HANDLE self;
#else
	void createTimer();
	struct itimerspec getTimerspec(int periodMillisecond, int firstTimeMillisecond);

	timer_t self;
#endif

	int interval;
	int firstTime;
	bool isOneshot;
	OnTimedEvent onTimedEvent;
	void *arg;
};

#endif
