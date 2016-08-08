#ifndef _SUBJECT_HPP_
#define _SUBJECT_HPP_

#include <cstddef>
#include <vector>

#include "Delegate.hpp"


template <typename Param>
class Subject
{
	typedef std::vector<Delegate<void, Param> *> DelegateVector;

	DelegateVector list;

public:
	void add(Delegate<void, Param> *delegate)
	{ list.push_back(delegate); }

	void remove(Delegate<void, Param> *delegate)
	{
		for (typename DelegateVector::iterator it = list.begin();
			it != list.end(); ++it)
		{
			if (*it == delegate)
			{
				list.erase(it);
				return;
			}
		}
	}

	void notify(Param param)
	{
		for (typename DelegateVector::iterator it = list.begin();
			it != list.end(); ++it)
		{
			(**it)(param);
		}
	}

	void reset() { list.clear(); }
};

#endif
