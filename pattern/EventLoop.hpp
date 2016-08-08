#ifndef _EVENTLOOP_HPP_
#define _EVENTLOOP_HPP_

#include <queue>

#include "thread/ConditionVariable.hpp"
#include "pattern/Runnable.hpp"


class EventLoop : protected Runnable
{
public:
	EventLoop();
	virtual ~EventLoop();

	bool postEvent(void *pushedEvent);

protected:
	typedef std::queue<void *> EventQueue;
	typedef enum Event { EMPTY, POST, EXIT } Event;

	virtual void processEvent(void *event) = 0;

	void * dequeueEvent();

	size_t getMaxQueueSize() const { return maxQueueSize; }
	void setMaxQueueSize(size_t size) { maxQueueSize = size; }

	void terminate();

	ConditionVariable cv;
	Event event;
	EventQueue eventQueue;
	size_t maxQueueSize;

private:
	void run();
};

#endif
