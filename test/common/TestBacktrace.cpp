#include "common/Backtrace.hpp"

static void foo() { int *f = nullptr; *f = 0; }
static void bar() { foo(); }

template<unsigned Depth> struct recurser
{ static void f() { recurser<Depth - 1>::f(); } };

template<> struct recurser<0>
{ static void f() { bar(); } };

int main()
{
	installBacktrace();
	recurser<10>::f();
	bar();
	return 0;
}
