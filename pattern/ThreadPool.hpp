#ifndef _THREADPOOL_HPP_
#define _THREADPOOL_HPP_

#include <list>
#include "pattern/Runnable.hpp"
#include "thread/ConditionVariable.hpp"


class WorkerThread : public Runnable
{
public:
	typedef void (*Routine)(void *);

	typedef struct Request
	{
		Request(Routine routine = nullptr, void *arg = nullptr) :
			routine(routine), arg(arg) {}

		void execute()
		{
			if (routine != nullptr)
				routine(arg);
		}

		void reset() { routine = nullptr; arg = nullptr; }

		Routine routine;
		void *arg;
	} Request;

	typedef enum State { BUSY, IDLE, EXIT } State;

	WorkerThread() : state(BUSY) {}

	State getState() const { return this->state; }

	bool setRequest(const Request &request);

	void waitIdle();
	void timedWaitIdle(int millisecond);

	void run();

	void terminate();

private:
	bool waitRequest();
	void notify(State state);

	ConditionVariable cv;
	Request request;
	State state;
};


class ThreadPool
{
	typedef std::list<WorkerThread *> Workers;

public:
	typedef WorkerThread::Request Request;

	ThreadPool(size_t maxThreads = 8) : maxThreads(maxThreads) {}
	~ThreadPool() { this->killAll(); }

	size_t size() const { return workers.size(); }

	void service(const Request &request);
	bool tryService(const Request &request);

	void killAll() { workers.remove_if(deleteAll); }
	void killIdle() { workers.remove_if(deleteIdle); }

private:
	static bool deleteIdle(WorkerThread *w);
	static bool deleteAll(WorkerThread *w);

	void hireNewWorker(const Request &request);
	void waitIdleWorker(const Request &request);

	Workers workers;
	size_t maxThreads;
};


#endif
