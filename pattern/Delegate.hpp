#ifndef _Delegate_HPP_
#define _Delegate_HPP_

#include "memory/SharedPointer.hpp"


template <typename Ret, typename Param>
class Callback
{
public:
	virtual ~Callback() {}
	virtual Ret invoke(Param param) = 0;
};


template <typename Ret, typename Param>
class StaticFunctionCallback : public Callback<Ret, Param>
{
	Ret (*func)(Param);

public:
	StaticFunctionCallback(Ret (*func)(Param)) : func(func) {}

	virtual Ret invoke(Param param)
	{ return func(param); }
};


template <typename Ret, typename Param, typename T, typename Method>
class MethodCallback : public Callback<Ret, Param>
{
	void *object;
	Method method;

public:
	MethodCallback(void *object, Method method) :
		object(object) , method(method) {}

	virtual Ret invoke(Param param)
	{
		T *obj = (T *)object;
		return (obj->*method)(param);
	}
};


template <typename Ret, typename Param>
class Delegate
{
	SharedPointer<Callback<Ret, Param>> callback;

public:
	Delegate() : callback(nullptr) {}

	Delegate(Ret (*func)(Param)) :
		callback(new StaticFunctionCallback<Ret, Param>(func)) {}

	template <typename T, typename Method>
	Delegate(T &object, Method method) :
		callback(new MethodCallback<Ret, Param, T, Method>(&object, method)) {}

	Ret operator () (Param param)
	{ return callback->invoke(param); }
};

#endif
