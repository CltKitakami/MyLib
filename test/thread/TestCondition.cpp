#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "thread/Thread.hpp"
#include "thread/Mutex.hpp"
#include "thread/Condition.hpp"


void * changer(void *);
void * watcher(void *);
void testCondition();


Condition condition;
Mutex conditionMutex;
int conditionVariable = 0;


int main()
{
	testCondition();

	return 0;
}


void * changer(void *)
{
	for (int i = 0; i < 20; ++i)
	{
		conditionMutex.lock();
		++conditionVariable;

		if (conditionVariable % 3 == 0)
		{
			condition.notifyOne();
			LOGT("mod 3");
		}

		conditionMutex.unlock();

		Thread::sleep(100);
	}

	conditionMutex.lock();
	conditionVariable = ~0;
	conditionMutex.unlock();
	condition.notifyOne();

	return 0;
}


void * watcher(void *)
{
	while (conditionVariable != ~0)
	{
		conditionMutex.lock();

		while (conditionVariable != ~0)
		{
			if (condition.timedWait(conditionMutex, 50) == true)
				LOGT("conditionVariable = " << conditionVariable);
			else
				LOGT("wait timeout");
		}

		conditionMutex.unlock();
	}

	return 0;
}


void testCondition()
{
	Thread th1((Thread::Routine)changer);
	Thread th2((Thread::Routine)watcher);

	th1.start();
	th2.start();

	th1.join();
	th2.join();
}
