#include <cmath>

#include "common/Log.hpp"
#include "common/Date.hpp"
#include "common/Stopwatch.hpp"
#include "system/ProcessTimes.hpp"
#include "thread/Thread.hpp"


void TestCpuTimes();


int main()
{
	LOG.setFileInfo(false);
	LOGT("Test Date");
	LOG_VAR(Date::now().toString());
	LOG_VAR(Date::getTimestamp());

	Date date = Date::timestampToDate(Date::getTimestamp() + 60 * 5);
	date = Date::timestampToDate(Date::dateToTimestamp(date));
	LOGT(date.toString("e, dd-MM-yy hh:mm:ss A"));
	date = Date::parse("e, dd n yyyy HH:mm:ss", "Sat, 30 May 2015 13:02:31 +0800");
	LOGT(date.toString("e, dd-MM-yy hh:mm:ss A"));

	LOGT("Test CPU Times");
	TestCpuTimes();

	return 0;
}


void TestCpuTimes()
{
	Stopwatch s;
	s.start();
	ProcessTimes processTimes;

	Thread::sleep(1000);	// This won't consume CPU times.

	double j = 0;
	for (int i = 0; i < 1e7; ++i)
	{
		j += std::sqrt((double)((i * j) / 37));
		j -= j / 11;
		j -= i % 23;
	}

	s.stop();

	LOG_VAR(j);
	LOGT("Total time (+ sleep time): " << s.getMicros() << " us");
}
