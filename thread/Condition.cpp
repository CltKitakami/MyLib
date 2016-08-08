#ifdef __linux__
#include <sys/time.h>
#endif

#include "Condition.hpp"
#include "common/Exception.hpp"
#include "common/Log.hpp"


#ifdef USE_FAKE_CONDITION_VARIABLE
#include <list>

typedef struct ConditionEvent
{
	ConditionEvent() : wokenUp(false)
	{
		this->event = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	~ConditionEvent() { ::CloseHandle(this->event); }

	bool wokenUp;
	HANDLE event;
} ConditionEvent;


class ConditionQueue
{
public:
	typedef std::list<ConditionEvent *> EventList;

	Mutex mutex;
	EventList list;
	EventList freeList;

	ConditionEvent * prepare()
	{
		mutex.lock();
		ConditionEvent *ce = nullptr;

		if (freeList.empty() == true)
			ce = new ConditionEvent();
		else
		{
			ce = freeList.front();
			freeList.pop_front();
			ce->wokenUp = false;
		}

		list.push_back(ce);
		mutex.unlock();

		return ce;
	}

	bool wait(ConditionEvent *ce)
	{
		return ::WaitForSingleObject(ce->event, INFINITE) == WAIT_OBJECT_0;
	}

	bool wait(ConditionEvent *ce, int millisecond)
	{
		return ::WaitForSingleObject(ce->event, (unsigned int)millisecond) == WAIT_OBJECT_0;
	}

	void post(ConditionEvent *ce)
	{
		mutex.lock();
		list.remove(ce);
		::ResetEvent(ce->event);
		freeList.push_back(ce);
		mutex.unlock();
	}
};
#endif


Condition::Condition()
{
#ifdef HAS_CONDITION_VARIABLE
	::InitializeConditionVariable(&this->cond);
#elif defined(USE_FAKE_CONDITION_VARIABLE)
	cq = new ConditionQueue();
#else
	int ret = ::pthread_cond_init(&this->cond, nullptr);

	if (ret != 0)
		LOG.e(TRACE, Log::format("Condition initialize failed: %d", ret));
#endif
}


Condition::~Condition()
{
#ifdef HAS_CONDITION_VARIABLE
#elif defined(USE_FAKE_CONDITION_VARIABLE)
	for (std::list<ConditionEvent *>::iterator it = this->cq->list.begin();
		it != this->cq->list.end(); ++it)
	{
		delete *it;	// Destroyed while threads are still waiting.
	}

	for (std::list<ConditionEvent *>::iterator it = this->cq->freeList.begin();
		it != this->cq->freeList.end(); ++it)
	{
		delete *it;
	}

#else
	CHECK_SUCCESS_RETURN(::pthread_cond_destroy(&this->cond), int, 0);
#endif
}


void Condition::wait(Mutex &mutex)
{
#ifdef HAS_CONDITION_VARIABLE
	::SleepConditionVariableCS(&this->cond, &mutex.self(), INFINITE);

#elif defined(USE_FAKE_CONDITION_VARIABLE)
	ConditionEvent *ce = this->cq->prepare();
	mutex.unlock();

	bool ret = this->cq->wait(ce);

	mutex.lock();

	if (ret == true)
		this->cq->post(ce);

#else
	CHECK_SUCCESS_RETURN(::pthread_cond_wait(&this->cond, &mutex.self()), int, 0);
#endif
}


bool Condition::timedWait(Mutex &mutex, int millisecond)
{
#ifdef HAS_CONDITION_VARIABLE
	return ::SleepConditionVariableCS(&this->cond, &mutex.self(), millisecond) != 0;

#elif defined(USE_FAKE_CONDITION_VARIABLE)
	ConditionEvent *ce = this->cq->prepare();
	mutex.unlock();

	bool ret = this->cq->wait(ce, millisecond);

	mutex.lock();

	if (ret == true)
		this->cq->post(ce);

	return ret;

#else
	struct timeval now;
	struct timespec ts;

	::gettimeofday(&now, nullptr);

	long result = now.tv_usec + millisecond * 1000;
	long us = result % 1000000;
	long s = result / 1000000 + now.tv_sec;

	ts.tv_sec = s;
	ts.tv_nsec = us * 1000;

	int ret = ::pthread_cond_timedwait(&this->cond, &mutex.self(), &ts);

	return ret == 0 ? true : false;
#endif
}


void Condition::notifyOne()
{
#ifdef HAS_CONDITION_VARIABLE
	::WakeConditionVariable(&this->cond);

#elif defined(USE_FAKE_CONDITION_VARIABLE)
	this->cq->mutex.lock();

	for (std::list<ConditionEvent *>::iterator it = this->cq->list.begin();
		it != this->cq->list.end(); ++it)
	{
		if ((*it)->wokenUp == false)
		{
			::SetEvent((*it)->event);
			(*it)->wokenUp = true;
			break;
		}
	}

	this->cq->mutex.unlock();

#else
	CHECK_SUCCESS_RETURN(::pthread_cond_signal(&this->cond), int, 0);
#endif
}


void Condition::notifyAll()
{
#ifdef HAS_CONDITION_VARIABLE
	::WakeAllConditionVariable(&this->cond);

#elif defined(USE_FAKE_CONDITION_VARIABLE)
	this->cq->mutex.lock();

	for (std::list<ConditionEvent *>::iterator it = this->cq->list.begin();
		it != this->cq->list.end(); ++it)
	{
		::SetEvent((*it)->event);
		(*it)->wokenUp = true;
	}

	this->cq->mutex.unlock();

#else
	CHECK_SUCCESS_RETURN(::pthread_cond_broadcast(&this->cond), int, 0);
#endif
}
