#ifndef _TIMERHANDLER_HPP_
#define _TIMERHANDLER_HPP_

#include <list>

#include "pattern/EventLoop.hpp"
#include "Timer.hpp"
#include "Atomic.hpp"
#include "Mutex.hpp"


class TimerHandler : public EventLoop
{
public:
	typedef struct OnTimedObject
	{
		OnTimedObject() = default;

		OnTimedObject(TimerHandler *handler, unsigned long long int id,
			Timer::OnTimedEvent event, void *arg = nullptr) :
			handler(handler), id(id),
			event(event), arg(arg), isTerminated(false) {}

		bool operator == (const struct OnTimedObject &obj) const
		{ return this->id == obj.id; }

		TimerHandler *handler;
		unsigned long long int id;
		Timer::OnTimedEvent event;
		void *arg;
		Atomic<bool> isOnTimed;
		bool isTerminated;
	} OnTimedObject;

	void attach(Timer &timer, Timer::OnTimedEvent event, void *arg = nullptr);
	void detach(Timer &timer);

private:
	virtual void processEvent(void *event);

	std::list<OnTimedObject> objs;
	Mutex mutex;
};

#endif
