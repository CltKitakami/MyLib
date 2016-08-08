#include <iostream>
#include <vector>
#include "pattern/IteratorAdaptor.hpp"

template <class T>
class Array
{
	T *p;
	int size;
public:
	typedef IteratorAdaptor<std::input_iterator_tag, T> iterator;
	typedef IteratorAdaptor<std::input_iterator_tag, T> const_iterator;

	Array(int size) : size(size) { p = new T[size]; }
	~Array() { delete p; }

	T & operator [] (int index) { return p[index]; }

	iterator begin() { return iterator(p); }
	iterator end() { return iterator(p + size); }
	const_iterator begin() const { return const_iterator(p); }
	const_iterator end() const { return const_iterator(p + size); }
};

int main()
{
	Array<double> point3d(3);
	point3d[0] = 2.3;
	point3d[1] = 3.2;
	point3d[2] = 4.2;

	for (Array<double>::const_iterator i = point3d.begin();
		i != point3d.end(); ++i)
		std::cout << *i << " ";
	std::cout << std::endl;

	std::vector<double> vec;
	std::copy(point3d.begin(), point3d.end(), std::back_inserter(vec));

	for (std::vector<double>::iterator i = vec.begin();
		i != vec.end(); ++i)
		std::cout << *i << " ";
	std::cout << std::endl;

	return 0;
}
