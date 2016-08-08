#include "common/Log.hpp"
#include "memory/ScopedPointer.hpp"
#include "memory/SharedPointer.hpp"


class TestPointer
{
	int num;
public:
	static int commonNum;

	TestPointer()
	{
		num = ++commonNum;
		LOGP("default constructor " << num);
	}

	~TestPointer() { LOGP("destructor " << num); }
	void print() { LOGP("hello " << num); }
};


int TestPointer::commonNum = 0;


void testScopedPointer();
void testSharedPointer();


int main()
{
	LOGP("===testScopedPointer===");
	testScopedPointer();

	LOGP("\n===testSharedPointer===");
	testSharedPointer();

	return 0;
}


void testScopedPointer()
{
	ScopedPointer<TestPointer> sp1(new TestPointer());
	sp1->print();
}


void testSharedPointer()
{
	TestPointer::commonNum = 0;

	SharedPointer<TestPointer> sp1(new TestPointer());
	sp1->print();

	SharedPointer<TestPointer> sp2;
	sp2 = new TestPointer();
	sp2->print();

	{
		SharedPointer<TestPointer> sp3 = sp1;
		sp3->print();

		SharedPointer<TestPointer> sp4;
		sp4.swap(sp1);
		if (sp1 != sp4)
			sp4->print();
	}
}
