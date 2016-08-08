#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <vector>


class Observer
{
public:
	virtual ~Observer() {}
	virtual void onNotify() = 0;
};


class Observee
{
	typedef std::vector<Observer *> Observers;

	Observers observers;

public:
#ifdef _MSC_VER
	typedef Observers::iterator ObserversIterator;
#else
	typedef typename Observers::iterator ObserversIterator;
#endif

	virtual ~Observee() = default;

	void add(Observer *observer) { observers.push_back(observer); }

	void remove(Observer *observer)
	{
		for (ObserversIterator it = observers.begin();
			it != observers.end(); ++it)
		{
			if (*it == observer)
			{
				observers.erase(it);
				return;
			}
		}
	}

	void notify()
	{
		for (ObserversIterator it = observers.begin();
			it != observers.end(); ++it)
		{
			(*it)->onNotify();
		}
	}

	void reset() { observers.clear(); }
};

#endif
