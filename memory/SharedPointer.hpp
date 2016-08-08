#ifndef _SHAREDPOINTER_HPP_
#define _SHAREDPOINTER_HPP_

#include <cstddef>

#include "SmartPointer.hpp"
#include "thread/Atomic.hpp"


template <typename T>
class SharedPointer : public SmartPointer<T>
{
public:
	SharedPointer() : SmartPointer<T>(),
		count(new Atomic<int>(1)) {}

	SharedPointer(T *ptr) : SmartPointer<T>(ptr),
		count(new Atomic<int>(1)) {}

	SharedPointer(const SharedPointer<T> &sp) : SmartPointer<T>(sp.ptr),
		count(sp.count) { ++(*count); }

	~SharedPointer() { this->release(); }

	void swap(SharedPointer<T> &sp)
	{
		T *tmp = sp.ptr;
		sp.ptr = this->ptr;
		this->ptr = tmp;

		Atomic<int> *tmp2 = sp.count;
		sp.count = this->count;
		this->count = tmp2;
	}

	void reset()
	{
		this->release();
		this->ptr = nullptr;
		this->count = nullptr;
	}

	SharedPointer<T> & operator = (const SharedPointer<T> &sp)
	{
		if (this != &sp)
		{
			this->release();
			this->copy(sp);
		}

		return *this;
	}

private:
	virtual void release()
	{
		if (count != nullptr && (--(*count)).value() == 0)
		{
			delete this->ptr;
			delete this->count;
		}
	}

	void copy(const SharedPointer<T> &sp)
	{
		this->ptr = sp.ptr;
		this->count = sp.count;
		++(*this->count);
	}

	Atomic<int> *count;
};

#endif
