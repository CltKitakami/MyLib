#include "common/Log.hpp"
#include "common/Stopwatch.hpp"
#include "thread/Thread.hpp"
#include "thread/TimerHandler.hpp"
#include "pattern/Runnable.hpp"


Stopwatch sw1, sw2;


void testTimer();

void timeout1(void *);
void timeout2(void *);
void testTimerHandler();

void testTimerEvent();


int main()
{
	testTimer();
	testTimerHandler();
	testTimerEvent();
	return 0;
}


void testTimer()
{
	LOGT("<testTimer>");

	Timer t1(timeout1);
	t1.setInterval(50);

	sw1.start();

	t1.start();
	Thread::sleep(1000);
	t1.stop();
}


void timeout1(void *)
{
	sw1.stop();
	LOGT("timeout1: " << sw1.getMillis());
}


void timeout2(void *)
{
	sw2.stop();
	LOGT("timeout2: " << sw2.getMillis());
}


void testTimerHandler()
{
	LOGT("<testTimerHandler>");

	Timer t1, t2;
	TimerHandler handler;
	handler.attach(t1, timeout1);
	handler.attach(t2, timeout2);

	t1.setInterval(1);
	t2.setInterval(2);

	sw1.start();
	sw2.start();

	t1.start();
	t2.start();

	Thread::sleep(15);
	handler.detach(t2);
	Thread::sleep(30);

	t1.stop();
	t2.stop();

	Thread::sleep(105);
}



class Test : public Runnable, public Timer
{
public:
	Test() : Timer() {}
	~Test() {}
	void onTimed() { LOGT("timeout"); }

	void start()
	{
		this->Runnable::start();
		this->Timer::start();
	}

	void run()
	{
		LOGT("hello");
		while (this->getExit() == false)
		{
			LOGT("hello");
			Thread::sleep(1000);
		}
	}
};

void testTimerEvent()
{
	LOGT("<testTimerEvent>");
	Test t;
	t.setInterval(500);
	t.start();
	Thread::sleep(3300);
}
