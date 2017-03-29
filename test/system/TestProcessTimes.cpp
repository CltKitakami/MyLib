#include "common/Log.hpp"
#include "common/Stopwatch.hpp"
#include "system/ProcessTimes.hpp"
#include "thread/Thread.hpp"

int main()
{
	Stopwatch sw;
	double sum = 0;

	sw.start();

	{
		ProcessTimes pt;

		sw.stop();

		while (sw.getMicros() < 3000000)
		{
			sw.stop();

			for (int i = 0; i < 100000000; ++i)
			{
				sum *= i;
				sum = sum / 123433353309;
				sum = sum * sum * sum * sum * sum * sum;
			}
		}
	}

	LOGP("elapsed " << sw.getMicros() << " us");

	(void)sum;

	return 0;
}
