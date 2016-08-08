#ifndef _TRACE_HPP_
#define _TRACE_HPP_

#define TRACE (Trace(__FILE__, __FUNCTION__, __LINE__))


class Trace
{
public:
	Trace() : file(nullptr), func(nullptr), line(0) {}

	Trace(const char *file, const char *func, unsigned int line) :
		file(file), func(func), line(line) {}

	const char *file;
	const char *func;
	unsigned int line;
};

#endif
