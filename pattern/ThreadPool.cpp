#include "ThreadPool.hpp"


bool WorkerThread::setRequest(const WorkerThread::Request &request)
{
	if (this->state == IDLE)
	{
		this->request = request;
		this->notify(BUSY);
		return true;
	}
	else
		return false;
}


void WorkerThread::waitIdle()
{
	ScopedMutex<ConditionVariable> m(&this->cv);
	while (this->state == BUSY)
		this->cv.wait();
}


void WorkerThread::timedWaitIdle(int millisecond)
{
	ScopedMutex<ConditionVariable> m(&this->cv);
	if (this->state == BUSY)
		this->cv.timedWait(millisecond);
}


void WorkerThread::run()
{
	while (this->Runnable::getExit() == false)
	{
		if (this->waitRequest() == true)
		{
			this->request.execute();
			this->request.reset();
		}
	}
}


void WorkerThread::terminate()
{
	this->Runnable::setExit();
	this->notify(EXIT);
	this->Runnable::join();
}


bool WorkerThread::waitRequest()
{
	if (this->state == EXIT)
		return false;
	else
	{
		this->notify(IDLE);

		ScopedMutex<ConditionVariable> m(&this->cv);
		while (this->state == IDLE)
			this->cv.wait();

		return this->Runnable::getExit() == false;
	}
}


void WorkerThread::notify(WorkerThread::State state)
{
	ScopedMutex<ConditionVariable> m(&this->cv);
	this->state = state;
	this->cv.notifyAll();
}


void ThreadPool::service(const ThreadPool::Request &request)
{
	if (this->tryService(request) == false)
		this->waitIdleWorker(request);
}


bool ThreadPool::tryService(const Request &request)
{
	bool isAccepted = false;
	for (Workers::iterator it = workers.begin();
		it != workers.end(); ++it)
	{
		WorkerThread *w = (WorkerThread *)*it;
		if (w->getState() == WorkerThread::IDLE)
		{
			w->setRequest(request);
			isAccepted = true;
			break;
		}
	}

	if (isAccepted == false)
	{
		if (workers.size() < maxThreads)
		{
			this->hireNewWorker(request);
			return true;
		}
		else
			return false;
	}
	else
		return true;
}


bool ThreadPool::deleteIdle(WorkerThread *w)
{
	if (w->getState() == WorkerThread::IDLE)
	{
		w->terminate();
		delete w;
		return true;
	}
	else
		return false;
}


bool ThreadPool::deleteAll(WorkerThread *w)
{
	w->terminate();
	delete w;
	return true;
}


void ThreadPool::hireNewWorker(const ThreadPool::Request &request)
{
	WorkerThread *w = new WorkerThread;
	workers.push_back(w);
	w->start();
	w->waitIdle();
	w->setRequest(request);
}


void ThreadPool::waitIdleWorker(const ThreadPool::Request &request)
{
	while (true)
	{
		for (Workers::iterator it = workers.begin();
			it != workers.end(); ++it)
		{
			WorkerThread *w = (WorkerThread *)*it;
			w->timedWaitIdle(10);
			if (w->getState() == WorkerThread::IDLE)
			{
				w->setRequest(request);
				return;
			}
		}
	}
}
