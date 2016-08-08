#ifndef _SCOPEDPOINTER_HPP_
#define _SCOPEDPOINTER_HPP_

#include "SmartPointer.hpp"
#include "pattern/NonCopyable.hpp"


template <typename T>
class ScopedPointer : public SmartPointer<T>, private NonCopyable
{
public:
	ScopedPointer() : SmartPointer<T>() {}
	ScopedPointer(T *ptr) : SmartPointer<T>(ptr) {}
	ScopedPointer(const ScopedPointer<T> &sp) : SmartPointer<T>(sp.ptr) {}

	~ScopedPointer() { this->SmartPointer<T>::release(); }

	ScopedPointer<T> & operator = (const ScopedPointer<T> &sp)
	{ this->ptr = sp.ptr; return *this; }
};

#endif
