#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include <cstddef>

#include "pattern/NonCopyable.hpp"


template <class T>
class Singleton : private NonCopyable
{
public:
	static T* instance()
	{
		if (pInstance != nullptr)
			return pInstance;
		else
		{
			pInstance = new T;
			return pInstance;
		}
	}

	static void release()
	{
		if (pInstance != nullptr)
		{
			delete pInstance;
			pInstance = nullptr;
		}
	}

private:
	static T *pInstance;
};


template <class T>
T * Singleton<T>::pInstance = nullptr;

#endif
