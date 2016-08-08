#ifndef _ATOMIC_HPP_
#define _ATOMIC_HPP_

#include <cstdint>

// #define USE_CPP11_ATOMIC

#ifdef _MSC_VER
	#if _MSC_VER >= 1700	// Up to Visual C++ 2012
	#define USE_CPP11_ATOMIC
	#define INTERLOCKED
	#endif
#endif


#ifdef USE_CPP11_ATOMIC
#include <atomic>
#define FETCH_ADD(addr, delta) *addr += delta
#define FETCH_SUB(addr, delta) *addr -= delta
#define FETCH_OR(addr, delta) *addr |= delta
#define FETCH_AND(addr, delta) *addr &= delta
#define FETCH_XOR(addr, delta) *addr ^= delta
#define FETCH_NAND(addr, delta)
#define COMPARE_SWAP(addr, comparedValue, newValue) (*addr).compare_exchange_weak(comparedValue, newValue)
#define LOCK_TEST_SET(addr, value) *addr = value
#define LOCK_RELEASE(addr)
#define MEMORY_BARRIER()

#elif defined(_MSC_VER)
#include <intrin.h>
#define INTERLOCKED_ADDR(addr) (reinterpret_cast<volatile long *>(addr))
#define FETCH_ADD(addr, delta) _InterlockedExchangeAdd(INTERLOCKED_ADDR(addr), (long)delta)
#define FETCH_SUB(addr, delta) FETCH_ADD(addr, -delta)
#define FETCH_OR(addr, delta) _InterlockedOr(INTERLOCKED_ADDR(addr), (long)delta)
#define FETCH_AND(addr, delta) _InterlockedAnd(INTERLOCKED_ADDR(addr), (long)delta)
#define FETCH_XOR(addr, delta) _InterlockedXor(INTERLOCKED_ADDR(addr), (long)delta)
#define FETCH_NAND(addr, delta)
#define COMPARE_SWAP(addr, comparedValue, newValue) _InterlockedCompareExchange(INTERLOCKED_ADDR(addr), (long)newValue, (long)comparedValue)
#define LOCK_TEST_SET(addr, value) _InterlockedExchange(INTERLOCKED_ADDR(addr), (long)value)
#define LOCK_RELEASE(addr)
#define MEMORY_BARRIER()

#else
#define FETCH_ADD(addr, delta) __sync_fetch_and_add(addr, delta)
#define FETCH_SUB(addr, delta) __sync_fetch_and_sub(addr, delta)
#define FETCH_OR(addr, delta) __sync_fetch_and_or(addr, delta)
#define FETCH_AND(addr, delta) __sync_fetch_and_and(addr, delta)
#define FETCH_XOR(addr, delta) __sync_fetch_and_xor(addr, delta)
#define FETCH_NAND(addr, delta) __sync_fetch_and_nand(addr, delta)
#define COMPARE_SWAP(addr, comparedValue, newValue) __sync_bool_compare_and_swap(addr, comparedValue, newValue)
#define LOCK_TEST_SET(addr, value) (void)__sync_lock_test_and_set(addr, value)
#define LOCK_RELEASE(addr) __sync_lock_release(addr)
#define MEMORY_BARRIER() __sync_synchronize()
#endif


template <class T>
class Atomic
{
public:
	Atomic(T value = 0) : self(value) {}

	T value() const { return this->self; }

	Atomic & operator = (const T value)
	{
		LOCK_TEST_SET(&this->self, value);
		return *this;
	}

	Atomic & operator = (const Atomic<T> &atomic)
	{
		LOCK_TEST_SET(&this->self, atomic.value());
		return *this;
	}

	Atomic & operator += (const T delta)
	{
		FETCH_ADD(&this->self, delta);
		return *this;
	}

	Atomic & operator -= (const T delta)
	{
		FETCH_SUB(&this->self, delta);
		return *this;
	}

	Atomic & operator ++ ()
	{
		FETCH_ADD(&this->self, 1);
		return *this;
	}

	Atomic operator ++ (int)
	{
		Atomic tmp = *this;
		FETCH_ADD(&this->self, 1);
		return tmp;
	}

	Atomic & operator -- ()
	{
		FETCH_SUB(&this->self, 1);
		return *this;
	}

	Atomic operator -- (int)
	{
		Atomic tmp = *this;
		FETCH_SUB(&this->self, 1);
		return tmp;
	}

	Atomic & operator |= (const T delta)
	{
		FETCH_OR(&this->self, delta);
		return *this;
	}

	Atomic & operator &= (const T delta)
	{
		FETCH_AND(&this->self, delta);
		return *this;
	}

	Atomic & operator ^= (const T delta)
	{
		FETCH_XOR(&this->self, delta);
		return *this;
	}

private:
#ifdef USE_CPP11_ATOMIC
	std::atomic<T> self;
#elif defined(INTERLOCKED)
	long self;
#else
	T self;
#endif
};

#ifdef INTERLOCKED
#undef INTERLOCKED
#undef INTERLOCKED_ADDR
#endif

#endif
