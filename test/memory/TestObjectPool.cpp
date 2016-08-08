#include <vector>
#include <list>
#include <map>
#include "common/Log.hpp"
#include "common/Stopwatch.hpp"
#include "memory/ObjectPool.hpp"


template<size_t DUMP_SIZE>
class Test
{
public:
	char dump[DUMP_SIZE];
	Test() {}
	~Test() {}
};


void testNewDelete();
void testMap();
template <typename T> void testStl();

int main()
{
	testNewDelete();
	testStl<std::vector<int, ObjectPool<int>>>();
	testStl<std::list<int, ObjectPool<int>>>();
	testMap();

	return 0;
}


void testNewDelete()
{
	const int TEST_LOOP_COUNTER = 2500000;
	const int TEST_OBJECT_BYTES = 1024000;

	ObjectPool<Test<TEST_OBJECT_BYTES>> pool;

	Stopwatch s;
	s.start();

	for (int i = 0; i < TEST_LOOP_COUNTER; ++i)
	{
		{
			// Test pool allocate de-allocate
			Test<TEST_OBJECT_BYTES> *p = pool.allocate(1);
			pool.construct(p, Test<TEST_OBJECT_BYTES>());
			pool.destroy(p);
			pool.deallocate(p, 1);
		}

		{	// Test defalut new delete
			// Test<TEST_OBJECT_BYTES> *p = new Test<TEST_OBJECT_BYTES>;
			// delete p;
		}
	}

	s.stop();

	LOGP("\nTotal time: " << s.getMicros() << " us");
}


void testMap()
{
	std::map<int, int, std::less<int>, ObjectPool<int>> v;
	for (int i = 0; i < 6; ++i)
	{
		LOGP("===========");
		LOGP("map: insert " << i);
		v[i] = i;
	}
	LOGP("===========");
}

template <typename T> void testStl()
{
	T v;
	for (int i = 0; i < 6; ++i)
	{
		LOGP("===========");
		LOGP("push " << i);
		v.push_back(i);
	}
	LOGP("===========");
}
