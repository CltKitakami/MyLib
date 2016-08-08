#include "common/Log.hpp"
#include "thread/Thread.hpp"
#include "pattern/Runnable.hpp"


class Worker : public Runnable
{
	void run()
	{
		for (int i = 0; i < 3; ++i)
			LOGT("hello " << i);
	}
};


int main()
{
	Worker worker;
	worker.start();
	Thread::sleep(1000);
	return 0;
}
