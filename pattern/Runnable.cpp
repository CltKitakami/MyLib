#include "Runnable.hpp"


int staticRunableFunction(void *arg)
{
	Runnable *runable = (Runnable *)arg;
	runable->run();
	return 0;
}


void Runnable::start()
{
	thread.setRoutine((Thread::Routine)staticRunableFunction, this);
	thread.start();
}
