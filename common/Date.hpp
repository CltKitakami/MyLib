#ifndef _DATE_HPP_
#define _DATE_HPP_

#include <cstdint>
#include <string>
#include <map>


class Date
{
public:
	std::string toString(const char *format = "yyyy-MM-dd HH:mm:ss") const;

	static Date parse(const char *format, const char *timeString);

	static Date timestampToDate(uint64_t seconds);
	static uint64_t dateToTimestamp(const Date &date);

	static uint64_t getTimestamp();

	static Date now();

	static std::string getMonthFullName(int month);
	static std::string getMonthShortName(int month);
	static std::string getWeekFullName(int daysOfWeek);
	static std::string getWeekShortName(int daysOfWeek);

	static int findDaysOfWeek(int year, int month, int day);

	void findDaysOfWeek();

	int getDaysOfWeek() const { return daysOfWeek; }
	void setDaysOfWeek(int daysOfWeek)
	{ this->daysOfWeek = (uint8_t)daysOfWeek; }

	int getIsPm() const { return isPm; }
	void setIsPm(bool isPm) { this->isPm = isPm; }

	int getYear() const { return year; }
	int getMonth() const { return (int)month; }
	int getDay() const { return (int)day; }
	int getHour() const { return (int)hour; }
	int getMinute() const { return (int)min; }
	int getSecond() const { return (int)sec; }

	void setYear(int year) { this->year = year; }
	void setMonth(int month) { this->month = (uint8_t)month; }
	void setDay(int day) { this->day = (uint8_t)day; }
	void setHour(int hour) { this->hour = (uint8_t)hour; }
	void setMinute(int minute) { this->min = (uint8_t)minute; }
	void setSecond(int second) { this->sec = (uint8_t)second; }

private:
	static void initialDateNameMap();
	static int getDateNameLength(const char *ptr, char termination);
	static int getParseDigitals(const char *ptr);
	static int parseInt(char symbol, const char *ptr,
		int digitals, const char *&nextPosition);
	static const char * intToString(int value, int digitals,
		char *outBuffer, char paddingChar);

	static std::map<std::string, int> dateNameMap;

	int year;
	uint8_t month, day;
	uint8_t hour, min, sec;
	uint8_t daysOfWeek;
	bool isPm;
};

#endif
