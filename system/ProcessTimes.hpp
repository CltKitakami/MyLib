#ifndef _PROCESSTIMES_HPP_
#define _PROCESSTIMES_HPP_

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#else
#error "Unknow OS"
#endif


class ProcessTimes
{
public:
	typedef struct Time
	{
		Time(long long user = 0, long long system = 0) :
			user(user), system(system) {}
		long long user, system;
	} Time;

	ProcessTimes();
	~ProcessTimes();

	void start() { this->getTimes(begin); }
	void stop() { this->getTimes(end); }

	Time elapsed() const
	{
		if (end.user >= 0 && begin.user >= 0 &&
			end.system >= 0 && begin.system >= 0)
			return Time(end.user - begin.user, end.system - begin.system);
		else
			return Time();
	}

	void report(std::ostream &stm = std::cout) const;

private:
	void getTimes(Time &time);

	bool isReported;
	Time begin, end;
#ifdef _WIN32
	HANDLE currentProcess;
#else
	long systemTickPreSecond;
#endif
};

#endif
