#include <omp.h>

#include "common/Log.hpp"
#include "thread/Thread.hpp"


#define OPENMP


void testOpemMP();


int main()
{
	testOpemMP();

	return 0;
}


void testOpemMP()
{
#ifdef OPENMP
#pragma omp parallel num_threads(4)
{
	LOGT("Hello, my number is " << omp_get_thread_num());

	#pragma omp barrier

	#pragma omp single
	{
		Thread::sleep(1000);
		LOGT("single");
	}

	#pragma omp master
	{
		Thread::sleep(500);
		LOGT("master");
	}

	#pragma omp barrier


	int copyedVar = 4;
	#pragma omp sections private(copyedVar)
	{
		#pragma omp section
		{
			copyedVar = 0;
			LOGT("section " << copyedVar);
		}
		#pragma omp section
		{
			copyedVar = 1;
			LOGT("section " << copyedVar);
		}
		#pragma omp section
		{
			copyedVar = 2;
			LOGT("section " << copyedVar);
		}
		#pragma omp section
		{
			copyedVar = 3;
			LOGT("section " << copyedVar);
		}
	}

	#pragma omp master
	LOG_VAR(copyedVar);

	#pragma omp sections
	{
		#pragma omp section
		LOGT("section2 0");
		#pragma omp section
		LOGT("section2 1");
		#pragma omp section
		LOGT("section2 2");
		#pragma omp section
		LOGT("section2 3");
	}
}
#endif
}
