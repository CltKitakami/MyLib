#include "Timer.hpp"
#include "common/Exception.hpp"


#ifdef _WIN32
VOID CALLBACK onTimedHandler(PVOID lpParameter, BOOLEAN)
{
	if (lpParameter != nullptr)
	{
		Timer * timer = (Timer *)lpParameter;
		timer->oneshotReset();
		timer->onTimed();
	}
}
#else
void onTimedHandler(union sigval value)
{
	if (value.sival_ptr != nullptr)
	{
		Timer * timer = (Timer *)value.sival_ptr;
		timer->oneshotReset();
		timer->onTimed();
	}
}
#endif


Timer::~Timer()
{
	this->stop();
}


void Timer::setInterval(int periodMillisecond, int firstTimeMillisecond)
{
	if (firstTimeMillisecond == 0)
	{
		this->interval = periodMillisecond;
		this->firstTime = periodMillisecond;
		this->isOneshot = false;
	}
	else
	{
		this->interval = periodMillisecond;
		this->firstTime = firstTimeMillisecond;
		this->isOneshot = true;
	}
}


void Timer::start()
{
	if (this->self == 0 && (this->interval >= 0 || this->isOneshot == true))
	{
#ifdef _WIN32
		CHECK_FAIL_RETURN((unsigned int)
			::CreateTimerQueueTimer(&this->self, nullptr,
				(WAITORTIMERCALLBACK)onTimedHandler,
				(PVOID)this, (DWORD)this->firstTime, (DWORD)this->interval,
				(ULONG)WT_EXECUTEINTIMERTHREAD),
		unsigned int, 0);

#else
		struct itimerspec spec = getTimerspec(this->interval, this->firstTime);
		this->createTimer();
		CHECK_SUCCESS_RETURN(::timer_settime(self, 0, &spec, 0), int, 0);
#endif
	}
}


void Timer::oneshotReset()
{
	if (this->isOneshot == true)
		this->self = 0;
}


void Timer::stop()
{
	if (this->self != 0)
	{
#ifdef _WIN32
		::DeleteTimerQueueTimer(nullptr, this->self, nullptr);
		::CloseHandle(this->self);

#else
		::timer_delete(this->self);
#endif

		this->self = 0;
	}
}


void Timer::onTimed()
{
	if (this->onTimedEvent != nullptr)
		this->onTimedEvent(this->arg);
}


#ifndef _WIN32
void Timer::createTimer()
{
	struct sigevent event;
	event.sigev_signo = 0;
	event.sigev_value.sival_ptr = this;
	event.sigev_notify = SIGEV_THREAD;
	event.sigev_notify_function = onTimedHandler;
	event.sigev_notify_attributes = 0;

	CHECK_SUCCESS_RETURN(::timer_create(CLOCK_REALTIME, &event, &this->self), int, 0);
}


struct itimerspec Timer::getTimerspec(int periodMillisecond, int firstTimeMillisecond)
{
	struct itimerspec spec;
	spec.it_interval.tv_sec = periodMillisecond / 1000;
	spec.it_interval.tv_nsec = periodMillisecond % 1000 * 1000000;
	spec.it_value.tv_sec = firstTimeMillisecond / 1000;
	spec.it_value.tv_nsec = firstTimeMillisecond % 1000 * 1000000;
	return spec;
}
#endif
