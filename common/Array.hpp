#ifndef _ARRAY_HPP_
#define _ARRAY_HPP_

#include <cstdlib>
#include <algorithm>


template <typename T>
class Array
{
public:
	Array() : data(nullptr) {}
	Array(size_t size) : data(nullptr) { this->allocate(size); }
	Array(const Array &array) : data(nullptr) { this->copy(array); }
	Array(Array &&array) : data(nullptr) { this->swap(std::move(array)); }

	~Array() { this->deallocate(); }

	void allocate(size_t size)
	{
		this->deallocate();
		this->size = size;
		this->data = new T [size];
	}

	void deallocate()
	{
		if (this->data != nullptr)
		{
			delete [] this->data;
			this->data = nullptr;
			this->size = 0;
		}
	}

	void copy(const Array &array)
	{
		this->allocate(array.size);
		std::copy(array.begin(), array.end(), this->data);
	}

	void swap(Array &&array)
	{
		std::swap(this->data, array.data);
		std::swap(this->size, array.size);
	}

	Array<T> clone() const
	{
		Array<T> array;
		array.copy(*this);
		return array;
	}

	size_t getSize() const { return this->size; }

	T * begin() { return this->data; }
	const T * begin() const { return this->data; }
	T * end() { return this->data + this->size; }
	const T * end() const { return this->data + this->size; }

	Array & operator = (const Array &array)
	{ this->copy(array); return *this; }

	Array & operator = (Array &&array)
	{ this->swap(array); return *this; }

	T & operator [] (size_t index) { return this->data[index]; }
	const T & operator [] (size_t index) const { return this->data[index]; }

	operator T * () { return this->begin(); }
	operator const T * () const { return this->begin(); }

private:
	T *data;
	size_t size;
};

#endif
