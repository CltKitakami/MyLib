#ifndef _ITERATORADAPTOR_HPP_
#define _ITERATORADAPTOR_HPP_

#include <iterator>


template <class Category, class T>
class IteratorAdaptor : public std::iterator<Category, T>
{
public:
	IteratorAdaptor(T *ptr) : ptr(ptr) {}
	IteratorAdaptor(const IteratorAdaptor &b) : ptr(b.ptr) {}
	virtual ~IteratorAdaptor() = default;

	IteratorAdaptor & operator ++ ()
	{
		this->next();
		return *this;
	}

	IteratorAdaptor operator ++ (int)
	{
		IteratorAdaptor tmp = this->clone();
		this->next();
		return tmp;
	}

	bool operator == (const IteratorAdaptor &rhs) const
	{ return this->equal(rhs); }

	bool operator != (const IteratorAdaptor &rhs) const
	{ return !this->equal(rhs); }

	T & operator * () { return *this->ptr; }
	T * operator -> () { return this->ptr; }

protected:
	T *ptr;

	virtual IteratorAdaptor clone() const
	{ return IteratorAdaptor(*this); }

	virtual bool equal(const IteratorAdaptor &rhs) const
	{ return this->ptr == rhs.ptr; }

	virtual void next() { ++this->ptr; }
};

#endif
