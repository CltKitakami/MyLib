#include "common/Log.hpp"
#include "common/Stopwatch.hpp"
#include "pattern/Future.hpp"

int foo(char c)
{
	Thread::sleep(1000);
	return (int)c;
}

int main()
{
	const int num = 10;

	FutureHost<int, char> host(8);
	Future<int> future[num];

	for (int i = 0; i < num; ++i)
		host.submit(&future[i], FutureHost<int, char>::Event((char)i, foo));

	Thread::sleep(2000);

	{
		Stopwatch sw;
		sw.start();

		for (int i = 0; i < num; ++i)
			LOG_VAR(future[i].get());

		sw.stop();
		LOG_VAR(sw.getMillis());
	}

	return 0;
}
