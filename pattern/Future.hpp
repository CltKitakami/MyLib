#ifndef _FUTURE_HPP_
#define _FUTURE_HPP_

#include "pattern/ThreadPool.hpp"
#include "pattern/EventLoop.hpp"
#include "thread/ConditionVariable.hpp"


template <typename FutureDataType>
class Future
{
public:
	Future() : isReady(false) {}
	virtual ~Future() = default;

	bool getReady() const { return this->isReady; }

	void reset()  { this->isReady = false; }

	void setData(FutureDataType data)
	{
		if (this->isReady == false)
		{
			ScopedMutex<ConditionVariable> m(&this->cv);
			this->isReady = true;
			this->data = data;
			this->cv.notifyAll();
		}
	}

	FutureDataType get()
	{
		ScopedMutex<ConditionVariable> m(&this->cv);

		while (this->isReady == false)
			this->cv.wait();

		return this->data;
	}

protected:
	bool isReady;
	ConditionVariable cv;
	FutureDataType data;
};


template <typename FutureDataType, typename InputType>
void futureDataHandler(void *event);


template <typename FutureDataType, typename InputType>
class FutureHost : protected EventLoop
{
public:
	typedef FutureDataType (*Handler)(InputType);

	typedef struct Event
	{
		Event(InputType input, Handler handler = nullptr) :
			input(input), handler(handler) {}

		InputType input;
		Handler handler;
	} Event;

	typedef struct EventObject
	{
		EventObject(Future<FutureDataType> *future, const Event &event) :
			future(future), event(event) {}

		Future<FutureDataType> *future;
		FutureDataType data;
		Event event;
	} EventObject;


	FutureHost(size_t maxThreads = 8) : pool(maxThreads) {}

	virtual ~FutureHost() = default;

	void setThreadPool(ThreadPool &pool)
	{
		if (this->pool.size() != 0)
			this->pool.killAll();

		this->pool = pool;
	}

	ThreadPool & getThreadPool() { return this->pool; }
	const ThreadPool & getThreadPool() const { return this->pool; }

	virtual void submit(Future<FutureDataType> *future, const Event &event)
	{
		this->EventLoop::postEvent(new EventObject(future, event));
	}

protected:
	virtual void processEvent(void *event)
	{
		pool.service(ThreadPool::Request(futureDataHandler<FutureDataType, InputType>, event));
	}

	ThreadPool pool;
};


template <typename FutureDataType, typename InputType>
void futureDataHandler(void *event)
{
	typedef typename FutureHost<FutureDataType, InputType>::EventObject EventObject;
	EventObject *obj = reinterpret_cast<EventObject *>(event);

	if (obj->event.handler != nullptr)
		obj->future->setData(obj->event.handler(obj->event.input));

	delete obj;
}

#endif
