#ifndef _SMARTPOINTER_HPP_
#define _SMARTPOINTER_HPP_

#include <cstddef>


template <typename T>
class SmartPointer
{
public:
	SmartPointer() : ptr(nullptr) {}
	SmartPointer(T *ptr) : ptr(ptr) {}
	virtual ~SmartPointer() = default;

	T * get() { return this->ptr; }
	const T * get() const { return this->ptr; }

	virtual void swap(SmartPointer<T> &sp)
	{
		T *tmp = sp.ptr;
		sp.ptr = this->ptr;
		this->ptr = tmp;
	}

	bool operator == (const SmartPointer<T> &sp) const
	{ return sp.get() == this->ptr; }

	bool operator != (const SmartPointer<T> &sp) const
	{ return sp.get() != this->ptr; }

	T & operator * () { return *ptr; }
	const T & operator * () const { return *ptr; }

	T * operator -> () { return ptr; }
	const T * operator -> () const { return ptr; }

protected:
	virtual void release() { delete this->ptr; }

	T *ptr;
};

#endif
