#include "TimerHandler.hpp"


void timerCallback(void *arg)
{
	// Thread-safe: called by timer thread
	TimerHandler::OnTimedObject *obj = (TimerHandler::OnTimedObject *)arg;
	obj->isOnTimed = true;
	obj->handler->postEvent(arg);
}


void TimerHandler::attach(Timer &timer, Timer::OnTimedEvent event, void *arg)
{
	// Thread-safe: with lock
	ScopedMutex<Mutex> sm(&this->mutex);
	objs.push_back(OnTimedObject(this, (unsigned long long int)&timer, event, arg));
	timer.setOnTimedEvent(timerCallback, &objs.back());
}


void TimerHandler::detach(Timer &timer)
{
	// Thread-safe: with lock
	ScopedMutex<Mutex> sm(&this->mutex);
	unsigned long long int timerId = (unsigned long long int)&timer;
	for (std::list<OnTimedObject>::iterator it = objs.begin(); it != objs.end(); ++it)
	{
		if ((*it).id == timerId)
		{
			timer.stop();
			timer.setOnTimedEvent(nullptr);
			if ((*it).isOnTimed.value() == false)
				objs.erase(it);
			else
				(*it).isTerminated = true;

			break;
		}
	}
}


void TimerHandler::processEvent(void *event)
{
	// Thread-safe: called by event-loop thread
	TimerHandler::OnTimedObject *obj = (TimerHandler::OnTimedObject *)event;
	obj->event(obj->arg);
	obj->isOnTimed = false;

	if (obj->isTerminated == true)
	{
		ScopedMutex<Mutex> sm(&this->mutex);
		objs.remove(*obj);
	}
}
