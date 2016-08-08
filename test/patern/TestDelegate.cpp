#include "common/Log.hpp"
#include "pattern/Delegate.hpp"


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


int main()
{
	A a;
	B b;
	D d;
	F f;

	// static function
	Delegate<void, int> d1(foo);

	// member function
	Delegate<void, int> d2(a, &A::foo);

	// member function + subclass instance
	Delegate<void, int> d3(b, &B::foo);

	// member function + subclass instance + multiple inheritance
	Delegate<void, int> d4(d, &D::foo);

	// static member function
	Delegate<void, int> d5(E::foo);

	// const member function
	Delegate<void, int> d6(f, &F::foo);

	d1(100); // "foo(100)"
	d2(200); // "A::foo(200)"
	d3(300); // "B::foo(300)"
	d4(400); // "D::foo(400)"
	d5(500); // "E::foo(500)"
	d6(600); // "F::foo(600)"

	return 0;
}
