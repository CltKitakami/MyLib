#include <string>

#include "ProcessTimes.hpp"
#include "common/Convert.hpp"


ProcessTimes::ProcessTimes() :
	isReported(false)
{
#ifdef _WIN32
	this->currentProcess = ::GetCurrentProcess();
#else
	this->systemTickPreSecond = ::sysconf(_SC_CLK_TCK);
#endif
	this->start();
}


ProcessTimes::~ProcessTimes()
{
	if (this->isReported == false)
	{
		this->stop();
		this->report();
	}
}


void ProcessTimes::report(std::ostream &stm) const
{
	Time time = this->elapsed();
	stm << "user: ";
	stm << Convert::toString((long double)time.user / 1000.0L);
	stm << " us, system: ";
	stm << Convert::toString((long double)time.system / 1000.0L);
	stm << " us\n";
}


void ProcessTimes::getTimes(Time &time)
{
#ifdef _WIN32
	FILETIME creation, exit;

	if (::GetProcessTimes(this->currentProcess, &creation, &exit,
		(LPFILETIME)&time.system, (LPFILETIME)&time.user))
	{
		// Windows uses 100 nanosecond ticks
		time.system *= 100;
		time.user *= 100;
	}
	else
	{
		time.system = ~0;
		time.user = ~0;
	}
#else
	struct tms buffer;

	if (::times(&buffer) != (clock_t)-1)
	{
		time.system = (long long)buffer.tms_stime * 1000000 /
			this->systemTickPreSecond;
		time.user = (long long)buffer.tms_utime * 1000000 /
			this->systemTickPreSecond;
	}
	else
	{
		time.system = ~0;
		time.user = ~0;
	}
#endif
}
