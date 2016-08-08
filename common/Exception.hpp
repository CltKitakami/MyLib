#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>
#include "common/Convert.hpp"
#include "common/Trace.hpp"


#ifndef NO_EXCEPTIONS
#define THROW(msg) \
do\
{\
	Exception ex(TRACE, "error: ");\
	ex << msg;\
	throw ex;\
} while (0)
#else
#define THROW(msg) \
do\
{\
	LOGP(TRACE << "error: " << msg);\
	exit(0);\
} while (0)
#endif

#define ASSERT(checkCondition) \
do\
{\
	if ((checkCondition) == false)\
		THROW(#checkCondition);\
} while (0)

#define ASSERT_MSG(checkCondition, msg) \
do\
{\
	if ((checkCondition) == false)\
		THROW(msg);\
} while (0)

#define CHECK_SUCCESS_RETURN(code, type, successValue) \
do\
{\
	type returnValue = code;\
	ASSERT_MSG(returnValue == successValue, #code " return != " #successValue);\
} while (0)

#define CHECK_FAIL_RETURN(code, type, failValue) \
do\
{\
	type returnValue = code;\
	ASSERT_MSG(returnValue != failValue, #code " return == " #failValue);\
} while (0)

#define TRY_CATCH(code, tag, msg) \
do {\
	try { \
		code;\
	} catch (Exception &e) { \
		e << "\n";\
		e.addTrace(TRACE);\
		e.add(tag, msg);\
		throw e;\
	}\
} while (0)


class Exception : public std::exception
{
public:
	Exception() = default;
	Exception(const char *msg);
	Exception(const std::string &msg);
	Exception(const Trace &trace, const char *msg);
	Exception(const Trace &trace, const std::string &msg);
	Exception(const Trace &trace, const char *msg, int code);
	Exception(const Trace &trace, const std::string &msg, int code);
	Exception(const Trace &trace, const char *msg, unsigned int code);
	Exception(const Trace &trace, const std::string &msg, unsigned int code);
	virtual ~Exception() noexcept = default;

	const std::string & getMessage() const noexcept { return this->msg; }

	virtual const char * what() const noexcept
	{ return this->msg.data(); }

	void addNewLine() { this->msg += '\n'; }
	void add(const char *tag, const char *msg);
	void add(const std::string &tag, const std::string &msg);
	void addTrace(const Trace &trace);
	void addCode(int value);
	void addCode(unsigned int value);

	Exception & operator << (const char ch);
	Exception & operator << (const char *str);
	Exception & operator << (const std::string &str);
	Exception & operator << (int code);
	Exception & operator << (long code);
	Exception & operator << (long long code);
	Exception & operator << (unsigned code);
	Exception & operator << (unsigned long code);
	Exception & operator << (unsigned long long code);
	Exception & operator << (float code);
	Exception & operator << (double code);
	Exception & operator << (long double code);

private:
	std::string msg;
};

#endif
