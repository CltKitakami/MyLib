#include "Exception.hpp"


Exception::Exception(const char *msg) : msg("Exception: ")
{
	this->msg += msg;
}


Exception::Exception(const std::string &msg) : msg("Exception: ")
{
	this->msg += msg;
}


Exception::Exception(const Trace &trace, const char *msg)
{
	this->addTrace(trace);
	this->add("Exception", msg);
}


Exception::Exception(const Trace &trace, const std::string &msg)
{
	this->addTrace(trace);
	this->add("Exception", msg);
}


Exception::Exception(const Trace &trace, const char *msg, int code)
{
	this->addTrace(trace);
	this->add("Exception", msg);
	this->addCode(code);
}


Exception::Exception(const Trace &trace, const std::string &msg, int code)
{
	this->addTrace(trace);
	this->add("Exception", msg);
	this->addCode(code);
}


Exception::Exception(const Trace &trace, const char *msg, unsigned int code)
{
	this->addTrace(trace);
	this->add("Exception", msg);
	this->addCode(code);
}


Exception::Exception(const Trace &trace, const std::string &msg, unsigned int code)
{
	this->addTrace(trace);
	this->add("Exception", msg);
	this->addCode(code);
}


void Exception::add(const char *tag, const char *msg)
{
	*this << ' ' << tag << ": " << msg;
}


void Exception::add(const std::string &tag, const std::string &msg)
{
	*this << ' ' << tag << ": " << msg;
}


void Exception::addTrace(const Trace &trace)
{
	*this << trace.file << ':' <<
		Convert::uintToString(trace.line) << ": " << trace.func << ':';
}


void Exception::addCode(int value)
{
	this->add("code", Convert::intToString(value));
}



void Exception::addCode(unsigned int value)
{
	this->add("code", Convert::uintToString(value));
}


Exception & Exception::operator << (const char ch)
{
	this->msg += ch;
	return *this;
}


Exception & Exception::operator << (const char *str)
{
	this->msg += str;
	return *this;
}


Exception & Exception::operator << (const std::string &str)
{
	this->msg += str;
	return *this;
}


Exception & Exception::operator << (int code)
{
	this->msg += Convert::intToString(code);
	return *this;
}


Exception & Exception::operator << (long code)
{
	this->msg += Convert::intToString(code);
	return *this;
}


Exception & Exception::operator << (long long code)
{
	this->msg += Convert::intToString(code);
	return *this;
}


Exception & Exception::operator << (unsigned code)
{
	this->msg += Convert::uintToString(code);
	return *this;
}


Exception & Exception::operator << (unsigned long code)
{
	this->msg += Convert::uintToString(code);
	return *this;
}


Exception & Exception::operator << (unsigned long long code)
{
	this->msg += Convert::uintToString(code);
	return *this;
}


Exception & Exception::operator << (float code)
{
	this->msg += Convert::toString(code);
	return *this;
}


Exception & Exception::operator << (double code)
{
	this->msg += Convert::toString(code);
	return *this;
}


Exception & Exception::operator << (long double code)
{
	this->msg += Convert::toString(code);
	return *this;
}
