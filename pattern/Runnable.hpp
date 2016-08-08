#ifndef _RUNNABLE_HPP_
#define _RUNNABLE_HPP_

#include "thread/Thread.hpp"
#include "pattern/NonCopyable.hpp"


class Runnable : private NonCopyable
{
	Thread thread;
	bool isExit;

public:
	Runnable() : isExit(false) {}
	virtual ~Runnable() { this->setExit(); }

	virtual void start();
	virtual void join() { this->thread.join(); }

	void setExit() { isExit = true; }
	bool getExit() const { return isExit; }

	virtual void run() = 0;
};

#endif
