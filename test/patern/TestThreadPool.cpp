#include "common/Log.hpp"
#include "pattern/ThreadPool.hpp"

Mutex m;

void test(void *arg)
{
	m.lock();
	LOGT("worker[" << arg << "] start");
	m.unlock();
	Thread::sleep(5000);
	m.lock();
	LOGT("worker[" << arg << "] stop");
	m.unlock();
}

int main()
{
	ThreadPool pool;

	for (int i = 1; i <= 11; ++i)
		pool.service(ThreadPool::Request(test, reinterpret_cast<void *>(i)));

	Thread::sleep(500);
	pool.killIdle();
	m.lock();
	LOGT("pool.size = " << pool.size());
	m.unlock();

	int i;
	std::cin >> i;

	return 0;
}
