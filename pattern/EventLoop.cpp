#include "EventLoop.hpp"


EventLoop::EventLoop() : event(EMPTY), maxQueueSize(5000)
{
	this->start();
}


EventLoop::~EventLoop()
{
	this->terminate();
}


void EventLoop::run()
{
	while (this->Runnable::getExit() == false)
	{
		this->cv.lock();
		size_t size = this->eventQueue.size();

		while (this->event == EventLoop::EMPTY && size == 0)
		{
			this->cv.wait();
			size = this->eventQueue.size();
		}

		this->cv.unlock();

		if (this->event == EventLoop::POST || size != 0)
		{
			while (size-- != 0)
				this->processEvent(this->dequeueEvent());

			this->event = EMPTY;
		}

		// if processEvent set exit event or terminate
		if (this->event == EventLoop::EXIT)
			break;
		else
			this->event = EMPTY;
	}
}


void EventLoop::terminate()
{
	if (this->event != EventLoop::EXIT)
	{
		this->cv.lock();
		this->event = EXIT;
		this->cv.notifyOne();
		this->cv.unlock();
		this->join();
	}
}


bool EventLoop::postEvent(void *pushedEvent)
{
	if (this->eventQueue.size() < this->maxQueueSize)
	{
		bool ret = true;

		this->cv.lock();

		if (this->event != EventLoop::EXIT)
		{
			this->event = POST;
			this->eventQueue.push(pushedEvent);
		}
		else
		{
			ret = false;
		}

		this->cv.unlock();
		this->cv.notifyOne();

		return ret;
	}
	else
		return false;
}


void * EventLoop::dequeueEvent()
{
	void *obj = this->eventQueue.front();
	this->eventQueue.pop();
	return obj;
}
