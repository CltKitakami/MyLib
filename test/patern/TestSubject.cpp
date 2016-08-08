#include "common/Log.hpp"
#include "pattern/Subject.hpp"
#include "pattern/Observer.hpp"


void foo(int x) { LOGT("foo(" << x << ")"); }

class A
{
public:
	virtual ~A() {}
	virtual void foo(int p) { LOGT("A::foo(" << p << ")"); }
};

class B : public A
{
public:
	virtual void foo(int p) { LOGT("B::foo(" << p << ")"); }
};

class C {};

class D : public C, public A
{
public:
	virtual void foo(int p) { LOGT("D::foo(" << p << ")"); }
};

class E
{
public:
	static void foo(int p) { LOGT("E::foo(" << p << ")"); }
};

class F
{
public:
	void foo(int p) const { LOGT("F::foo(" << p << ")"); }
};


class Price : public Observee {};

class Store : public Observer
{
	char ch;
public:
	Store(char ch) : ch(ch) {}
	void onNotify() { LOGT("Store" << ch << " update"); }
};



int main()
{
	LOGT("=====TestSubject=====");

	Subject<int> observee;
	A observerA;
	B observerB;
	D observerD;
	F observerF;

	Delegate<void, int> d[6];

	d[0] = Delegate<void, int>(foo);
	d[1] = Delegate<void, int>(observerA, &A::foo);
	d[2] = Delegate<void, int>(observerB, &B::foo);
	d[3] = Delegate<void, int>(observerD, &D::foo);
	d[4] = Delegate<void, int>(E::foo);
	d[5] = Delegate<void, int>(observerF, &F::foo);

	for (int i = 0; i < 6; ++i)
		observee.add(&d[i]);

	observee.notify(101);


	LOGT("=====TestObserver=====");

	Store storeA('A');
	Store storeB('B');
	Store storeC('C');
	Price price;
	price.add(&storeA);
	price.add(&storeB);
	price.add(&storeC);
	price.notify();

	return 0;
}
