#ifndef _CALLABLE_HPP_
#define _CALLABLE_HPP_

template <typename Return, typename Input>
class Callable
{
public:
	Callable(Return (*function)(Input), Input input) :
		function(function), input(input) {}

	Return call() { return function(input); }

	Return (*function)(Input);
	Input input;
};

#endif
