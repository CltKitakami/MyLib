#include <sstream>

#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "thread/Thread.hpp"
#include "thread/Atomic.hpp"


void * foo(void *arg);
void testThread();

void testAtomic();


int main()
{
	testThread();
	testAtomic();

	return 0;
}


void * foo(void *arg)
{
	for (int i = 0; i < 50; ++i)
	{
		std::ostringstream ss;
		ss << (const char *)arg << i;
		LOGT(ss.str());
		Thread::sleep(50);
	}

	Thread::exit(11);
	return 0;
}


void testThread()
{
	char arg1[] = "th1";
	char arg2[] = "th2";

	Thread th1((Thread::Routine)foo, arg1);
	Thread th2((Thread::Routine)foo, arg2);

	th1.setBackground(true);
	th1.start();
	th2.start();

#ifdef _WIN32
	Thread::sleep(500);
	th1.suspend();
	LOGT("th1 was suspended.");

	Thread::sleep(500);
	th2.suspend();
	LOGT("th2 was suspended.");

	Thread::sleep(500);
	th1.resume();
	LOGT("th1 was resumed.");

	Thread::sleep(500);
	th2.resume();
	LOGT("th2 was resumed.");
#endif

	int ret2 = th2.join();

	LOG_VAR(ret2);
}


void testAtomic()
{
	Atomic<int> i(0);
	i += 1;
	LOG_VAR((i++).value());
	LOG_VAR((++i).value());
}
