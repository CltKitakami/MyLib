#include <cstdlib>
#include <cstring>

#include "common/Exception.hpp"
#include "Thread.hpp"


#define TH_EX_CREATING "Thread creating failed."
#define TH_EX_JOIN "Thread join failed."
#define TH_EX_TERMINAL "Thread terminal failed."
#define TH_EX_PRIORITY "Thread priority failed: "
#define TH_EX_RESUME "Thread resume failed: "
#define TH_EX_SUSPEND "Thread suspend failed: "
#define TH_EX_DETACHED "Thread is not joinable"


Thread::Thread(Routine routine, void *arg) :
#ifdef _WIN32
	handle((HANDLE)~0),
#else
	tid(0),
#endif
	routine(routine),
	arg(arg),
	isBackground(false),
	state(UNSTART)
{
#ifdef _WIN32
	if (routine != nullptr)
		this->beginThreadEx();
#endif
}


Thread::~Thread()
{
	if (this->isBackground == false)
		this->join();
	else
	{
#ifdef _WIN32
		::CloseHandle(this->handle);
#else
		::pthread_detach(this->tid);
#endif
	}
}


int Thread::start()
{
	int ret = ~0;

	if (this->state != RUNNING)
	{
#ifdef _WIN32
		if (this->handle != (HANDLE)(~0))
			ret = this->resume();
		else
		{
			if (this->beginThreadEx() == (HANDLE)(~0))
				throw Exception(TRACE, TH_EX_CREATING, ret);
		}
#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		if (this->isBackground == true)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		ret = ::pthread_create(&this->tid, &attr, this->routine, this->arg);

		if (ret == 0)
			this->state = RUNNING;
		else
			throw Exception(TRACE, TH_EX_CREATING, ret);
#endif
	}

	return ret;
}


int Thread::join()
{
	int ret = ~0;

	if (this->isBackground == true)
		throw Exception(TRACE, TH_EX_DETACHED);
	else if (this->state == RUNNING)
	{
#ifdef _WIN32
		ret = (int)::WaitForSingleObject(this->handle, INFINITE);

		if (ret != ~0)
			this->state = STOPPED;
		else
			throw Exception(TRACE, TH_EX_JOIN, ret);

		unsigned long int code = 0;
		if (::GetExitCodeThread(this->handle, &code) == false)
			throw Exception(TRACE, TH_EX_JOIN);

		ret = (int)code;
#else
		void *code = 0;
		if (::pthread_join(this->tid, &code) == 0)
			this->state = STOPPED;
		else
			throw Exception(TRACE, TH_EX_JOIN);

		ret = (int)reinterpret_cast<intptr_t>(code);
#endif
	}

	return ret;
}


int Thread::stop()
{
	int ret = ~0;

	if (this->state == RUNNING)
	{
#ifdef _WIN32
		// Warning
		// If the target thread owns a critical section, the critical section will not be released.
		// If the target thread is allocating memory from the heap, the heap lock will not be released.
		// If the target thread is executing certain kernel32 calls when it is terminated, the kernel32 state for the thread's process could be inconsistent.
		// If the target thread is manipulating the global state of a shared DLL, the state of the DLL could be destroyed, affecting other users of the DLL.
		ret = ::TerminateThread(this->handle, (DWORD)~0);

		if (ret != 0)
			this->state = STOPPED;
		else
			throw Exception(TRACE, TH_EX_TERMINAL, ret);
#else
		ret = ::pthread_cancel(this->tid);

		if (ret == 0)
			this->state = STOPPED;
		else
			throw Exception(TRACE, TH_EX_TERMINAL, ret);
#endif
	}

	return ret;
}


void Thread::exit(unsigned int code)
{
#ifdef _WIN32
	Thread::endThreadEx(code);
#else
	::pthread_exit(reinterpret_cast<void *>(code));
#endif
}


void Thread::setRoutine(Routine routine, void *arg)
{
#ifdef _WIN32
	if (this->handle == (HANDLE)~0)
		::CloseHandle(this->handle);
#endif
	this->routine = routine;
	this->arg = arg;
#ifdef _WIN32
	this->beginThreadEx();
#endif
}


#ifdef _WIN32
void Thread::setPriority(Thread::Priority value)
#else
void Thread::setPriority(int value)
#endif
{
#ifdef _WIN32
	int ret = ::SetThreadPriority(this->handle, value);

	if (ret != 0)
		throw Exception(TRACE, TH_EX_PRIORITY "Error.");
#else
	int ret = ::pthread_setschedprio(this->tid, value);

	if (ret != 0)
		throw Exception(TRACE, TH_EX_PRIORITY, ret);
#endif
}


#ifdef _WIN32
int Thread::resume()
{
	int ret = ~0;

	if (this->state != RUNNING)
	{
		ret = (int)::ResumeThread(this->handle);

		if (ret != ~0)
			this->state = RUNNING;
		else
			throw Exception(TRACE, TH_EX_RESUME, ret);
	}

	return ret;
}


int Thread::suspend()
{
	int ret = ~0;

	if (this->state == RUNNING)
	{
		ret = (int)::SuspendThread(this->handle);

		if (ret != ~0)
			this->state = STOPPED;
		else
			throw Exception(TRACE, TH_EX_SUSPEND, ret);
	}

	return ret;
}
#endif


void Thread::sleep(int milliSeconds)
{
#ifdef _WIN32
	::Sleep((unsigned int)milliSeconds);
#else
	::usleep((__useconds_t)milliSeconds * 1000);
#endif
}


#ifdef _WIN32
HANDLE Thread::beginThreadEx()
{
	this->handle = (HANDLE)::_beginthreadex(
		nullptr, 0, this->routine, this->arg, CREATE_SUSPENDED, nullptr);
	return this->handle;
}
#endif
