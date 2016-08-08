#include <stdarg.h>
#include "Log.hpp"
#include "common/Convert.hpp"
#include "common/Date.hpp"
#include "common/String.hpp"


Log LOG;


std::streambuf * Log::redirect(std::streambuf *sb)
{
	return out.rdbuf(sb);
}


void Log::outputTime()
{
	if (this->timeInfo == true)
		*this << '[' << Date::now().toString() << "] ";
}


void Log::outputTag(const std::string &tag)
{
	if (tag.length() > 0)
		*this << '<' << tag << "> ";
}


void Log::output(const Trace &t, const std::string &tag,
	const std::string &msg)
{
	this->outputTime();
	this->outputTrace(t);
	this->outputTag(tag);
	this->outputMessage(msg);
}


void Log::output(const Trace &t, const std::string &msg)
{
	this->outputTime();
	this->outputTrace(t);
	this->outputMessage(msg);
}


void Log::outputTrace(const Trace &t)
{
	if (this->fileInfo == true)
	{
		*this << t.file;
		*this << (this->lineInfo == true ? ":" : ": ");
	}

	if (this->lineInfo == true)
		*this << t.line << ": ";

	if (this->funcInfo == true)
		*this << t.func << ": ";
}


void Log::outputMessage(const std::string &msg)
{
	*this << msg << '\n';
}


void Log::output(const std::string &msg)
{
	out << msg;
}


void Log::output(const char *msg)
{
	out << msg;
}


void Log::output(char ch)
{
	out << ch;
}


void Log::d(const Trace &t, const std::string &tag, const std::string &msg)
{
	if (this->debugLog == true)
		this->output(t, tag, msg);
}


void Log::e(const Trace &t, const std::string &tag, const std::string &msg)
{
	if (this->errorLog == true)
		this->output(t, tag, msg);
}


void Log::w(const Trace &t, const std::string &tag, const std::string &msg)
{
	if (this->warningLog == true)
		this->output(t, tag, msg);
}


void Log::i(const Trace &t, const std::string &tag, const std::string &msg)
{
	if (this->infoLog == true)
		this->output(t, tag, msg);
}


void Log::d(const Trace &t, const std::string &msg)
{
	if (this->debugLog == true)
		this->output(t, msg);
}


void Log::e(const Trace &t, const std::string &msg)
{
	if (this->errorLog == true)
		this->output(t, msg);
}


void Log::w(const Trace &t, const std::string &msg)
{
	if (this->warningLog == true)
		this->output(t, msg);
}


void Log::i(const Trace &t, const std::string &msg)
{
	if (this->infoLog == true)
		this->output(t, msg);
}


void Log::d(const std::string &msg)
{
	if (this->infoLog == true)
		this->outputMessage(msg);
}


void Log::e(const std::string &msg)
{
	if (this->errorLog == true)
		this->outputMessage(msg);
}


void Log::w(const std::string &msg)
{
	if (this->warningLog == true)
		this->outputMessage(msg);
}


void Log::i(const std::string &msg)
{
	if (this->infoLog == true)
		this->outputMessage(msg);
}


std::string Log::format(const char *args, ...)
{
	va_list va;
	va_start(va, args);
	String str = String::format(args, va);
	va_end(va);
	return str.inner();
}


Log & operator << (Log &log, const Trace &t)
{
	log.outputTrace(t);
	return log;
}


Log & operator << (Log &log, const std::string &msg)
{
	log.output(msg);
	return log;
}


Log & operator << (Log &log, const char *msg)
{
	log.output(msg);
	return log;
}


Log & operator << (Log &log, char msg)
{
	log.output(msg);
	return log;
}


Log & operator << (Log &log, int value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, long value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, long long value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, unsigned value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, unsigned long value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, unsigned long long value)
{
	log << Convert::intToString(value);
	return log;
}


Log & operator << (Log &log, float value)
{
	log << Convert::toString(value);
	return log;
}


Log & operator << (Log &log, double value)
{
	log << Convert::toString(value);
	return log;
}


Log & operator << (Log &log, long double value)
{
	log << Convert::toString(value);
	return log;
}
