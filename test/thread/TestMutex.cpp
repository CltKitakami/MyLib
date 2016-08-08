#include <sstream>

#include "common/Log.hpp"
#include "thread/Thread.hpp"
#include "thread/Mutex.hpp"


void * foo(void *arg);
void testMutex();


Mutex mutex;


int main()
{
	testMutex();
	return 0;
}


void * foo(void *arg)
{
	for (int i = 0; i < 20; ++i)
	{
		mutex.lock();
		LOGP((const char *)arg);
		mutex.unlock();
		Thread::sleep(10);
	}

	Thread::exit(0);
	return 0;
}


void testMutex()
{
	char arg1[] = "th1";
	char arg2[] = "th2";

	Thread th1((Thread::Routine)foo, arg1);
	Thread th2((Thread::Routine)foo, arg2);

	th1.start();
	th2.start();

	th1.join();
	th2.join();
}
