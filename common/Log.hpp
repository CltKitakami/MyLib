#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <iostream>
#include <string>
#include "common/Trace.hpp"


#define LOGP(var) LOG << var << '\n'
#define LOGT(var) LOG << TRACE << var << '\n'
#define LOGF(...) LOG << TRACE << Log::format(__VA_ARGS__) << '\n'
#define LOG_VAR(var) LOG << TRACE << #var " = " << var << '\n'
#define LOG_VAR_HEX(var) LOGF(#var " = 0x%x", var)


class Log
{
public:
	Log() : fileInfo(true), funcInfo(true),
		lineInfo(true), timeInfo(false),
		debugLog(true), errorLog(true),
		warningLog(true), infoLog(true),
		out(std::cout.rdbuf()) {}

	void setFileInfo(bool enable) { this->fileInfo = enable; }
	bool getFileInfo() const { return this->fileInfo; }

	void setFunctionInfo(bool enable) { this->funcInfo = enable; }
	bool getFunctionInfo() const { return this->funcInfo; }

	void setLineInfo(bool enable) { this->lineInfo = enable; }
	bool getLineInfo() const { return this->lineInfo; }

	void setTimeInfo(bool enable) { this->timeInfo = enable; }
	bool getTimeInfo() const { return this->timeInfo; }

	void setDebug(bool enable) { this->debugLog = enable; }
	bool getDebug() const { return this->debugLog; }

	void setError(bool enable) { this->errorLog = enable; }
	bool getError() const { return this->errorLog; }

	void setWarning(bool enable) { this->warningLog = enable; }
	bool getWarning() const { return this->warningLog; }

	void setInfo(bool enable) { this->infoLog = enable; }
	bool getInfo() const { return this->infoLog; }

	std::streambuf * redirect(std::streambuf *sb);

	void d(const Trace &t, const std::string &tag, const std::string &msg);
	void e(const Trace &t, const std::string &tag, const std::string &msg);
	void w(const Trace &t, const std::string &tag, const std::string &msg);
	void i(const Trace &t, const std::string &tag, const std::string &msg);

	void d(const Trace &t, const std::string &msg);
	void e(const Trace &t, const std::string &msg);
	void w(const Trace &t, const std::string &msg);
	void i(const Trace &t, const std::string &msg);

	void d(const std::string &msg);
	void e(const std::string &msg);
	void w(const std::string &msg);
	void i(const std::string &msg);

	void output(const Trace &t, const std::string &tag, const std::string &msg);
	void output(const Trace &t, const std::string &msg);
	void outputTrace(const Trace &t);
	void outputMessage(const std::string &msg);
	void output(const std::string &msg);
	void output(const char *msg);
	void output(char ch);

	static std::string format(const char *args, ...);

private:
	void outputTime();
	void outputTag(const std::string &tag);

	bool fileInfo, funcInfo, lineInfo, timeInfo;
	bool debugLog, errorLog, warningLog, infoLog;
	std::ostream out;
};


Log & operator << (Log &log, const Trace &t);
Log & operator << (Log &log, const std::string &msg);
Log & operator << (Log &log, const char *msg);
Log & operator << (Log &log, char msg);
Log & operator << (Log &log, int value);
Log & operator << (Log &log, long value);
Log & operator << (Log &log, long long value);
Log & operator << (Log &log, unsigned value);
Log & operator << (Log &log, unsigned long value);
Log & operator << (Log &log, unsigned long long value);
Log & operator << (Log &log, float value);
Log & operator << (Log &log, double value);
Log & operator << (Log &log, long double value);


extern Log LOG;

#endif
