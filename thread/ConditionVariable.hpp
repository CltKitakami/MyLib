#ifndef _CONDITIONVARIABLE_HPP_
#define _CONDITIONVARIABLE_HPP_

#include "Mutex.hpp"
#include "Condition.hpp"
#include "pattern/NonCopyable.hpp"


class ConditionVariable : private NonCopyable
{
public:
	void lock() { m.lock(); }
	bool trylock() { return m.trylock(); }
	void unlock() { m.unlock(); }

	void wait() { c.wait(m); }
	bool timedWait(int millisecond) { return c.timedWait(m, millisecond); }
	void notifyOne() { c.notifyOne(); }
	void notifyAll() { c.notifyAll(); }

private:
	Mutex m;
	Condition c;
};

#endif
