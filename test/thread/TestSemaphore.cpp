#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "thread/Thread.hpp"
#include "thread/Semaphore.hpp"


void * foo(void *arg);
void testSemaphore();


Semaphore semaphore(2);


int main()
{
	LOGT("start");
	testSemaphore();

	return 0;
}


void * worker(void *)
{
	while (true)
	{
		if (semaphore.timedAcquire(500) == true)
			break;
		else
			LOGT("wait timeout");
	}

	LOGT(Thread::getId());
	Thread::sleep(1000);
	semaphore.release();

	Thread::exit(0);
	return 0;
}


void testSemaphore()
{
	Thread *th[5];

	for (int i = 0; i < 5; ++i)
	{
		th[i] = new Thread((Thread::Routine)worker);
		th[i]->start();
	}

	for (int i = 0; i < 5; ++i)
	{
		th[i]->join();
		delete th[i];
	}
}
