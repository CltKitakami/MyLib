#include <ctime>

#include "Date.hpp"
#include "common/Exception.hpp"


#define MAX_DIGITALS 7


std::map<std::string, int> Date::dateNameMap;


std::string Date::getMonthFullName(int month)
{
	const char *names[] = {
		"{month}",
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	if (month < 1 && month > 12)
		return std::string(names[0]);
	else
		return std::string(names[month]);
}


std::string Date::getMonthShortName(int month)
{
	const char *names[] = {
		"{month}",
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};

	if (month < 1 && month > 12)
		return std::string(names[0]);
	else
		return std::string(names[month]);
}


std::string Date::getWeekFullName(int daysOfWeek)
{
	const char *names[] = {
		"{week}",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday"
	};

	if (daysOfWeek < 1 && daysOfWeek > 7)
		return std::string(names[0]);
	else
		return std::string(names[daysOfWeek]);
}


std::string Date::getWeekShortName(int daysOfWeek)
{
	const char *names[] = {
		"{week}",
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat",
		"Sun"
	};

	if (daysOfWeek < 1 && daysOfWeek > 7)
		return std::string(names[0]);
	else
		return std::string(names[daysOfWeek]);
}


void Date::initialDateNameMap()
{
	if (dateNameMap.size() != 0)
		return;

	for (int i = 1; i <= 12; ++i)
	{
		dateNameMap[getMonthFullName(i)] = i;
		dateNameMap[getMonthShortName(i)] = i;
	}

	for (int i = 1; i <= 7; ++i)
	{
		dateNameMap[getWeekFullName(i)] = i;
		dateNameMap[getWeekShortName(i)] = i;
	}

	dateNameMap["Sept"] = 9;

	dateNameMap["Tu"] = 2;
	dateNameMap["Tues"] = 2;
	dateNameMap["Thurs"] = 4;
	dateNameMap["Thur"] = 4;
	dateNameMap["Th"] = 4;

	dateNameMap["AM"] = 0;
	dateNameMap["am"] = 0;
	dateNameMap["PM"] = 12;
	dateNameMap["pm"] = 12;
}


int Date::getDateNameLength(const char *ptr, char termination)
{
	int length = 1;

	while (ptr[length] != termination &&
		ptr[length] >= 'a' && ptr[length] <= 'z')
		length += 1;

	return length;
}


int Date::getParseDigitals(const char *ptr)
{
	char symbol = *ptr;
	int digitals = 1;

	while (ptr[digitals] == symbol)
		digitals += 1;

	return digitals;
}


int Date::parseInt(char symbol, const char *ptr, int digitals,
	const char *&nextPosition)
{
	int result = 0;
	const char *p = ptr;

	while (*p < '0' || *p > '9')
	{
		if (*p == '\0')
			break;
		else
			++p;
	}

	if (*p < '0' || *p > '9')
	{
		char e[] = { "parse \'?\' error when: \'?\'" };
		e[7] = symbol;
		e[23] = *ptr;
		throw Exception(TRACE, e);
	}

	for (int i = 0; i < digitals; ++i)
	{
		if (*p < '0' || *p > '9')
		{
			break;
		}
		else
		{
			result *= 10;
			result += (int)(*p - '0');
		}

		++p;
	}

	nextPosition = p;

	return result;
}


const char * Date::intToString(int value, int digitals,
	char *outBuffer, char paddingChar)
{
	char *ptr = &outBuffer[MAX_DIGITALS];
	*ptr = '\0';

	if (digitals > MAX_DIGITALS)
		digitals = MAX_DIGITALS;

	for (int i = 0; i < digitals; ++i)
	{
		ptr -= 1;
		int dig = value % 10;

		if (value != 0)
		{
			*ptr = char(dig + (int)'0');
		}
		else if (paddingChar == 0)
		{
			ptr += 1;
			break;
		}
		else
		{
			*ptr = paddingChar;
		}

		value /= 10;
	}

	return ptr;
}


Date Date::parse(const char *format, const char *timeString)
{
	Date::initialDateNameMap();

	const char *f = format;
	const char *t = timeString;
	int year = 0, month = 0, day = 0;
	int hour = 0, min = 0, sec = 0;
	int daysOfWeek = 0;
	int amOrPm = 0;
	int *saveIntVariable = nullptr;
	int *saveNameVariable = nullptr;

	while (*f != '\0' && *t != '\0')
	{
		switch (*f)
		{
			case 'y':
				saveIntVariable = &year;
				break;
			case 'M':
				saveIntVariable = &month;
				break;
			case 'd':
				saveIntVariable = &day;
				break;
			case 'h':
			case 'H':
				saveIntVariable = &hour;
				break;
			case 'm':
				saveIntVariable = &min;
				break;
			case 's':
				saveIntVariable = &sec;
				break;
			case 'n':
			case 'N':
				saveNameVariable = &month;
				break;
			case 'e':
				saveNameVariable = &daysOfWeek;
				break;
			case 'E':
				saveNameVariable = &daysOfWeek;
				break;
			case 'a':
				saveNameVariable = &amOrPm;
				break;
			case 'A':
				saveNameVariable = &amOrPm;
				break;
			default:
				f += 1;
				t += 1;
				break;
		}

		if (saveIntVariable != nullptr)
		{
			int parseDigitals = Date::getParseDigitals(f);
			const char *nextPosition = t;

			*saveIntVariable = Date::parseInt(*f, t,
				parseDigitals, nextPosition);
			saveIntVariable = nullptr;
			t = nextPosition;
			f += parseDigitals;
		}
		else if (saveNameVariable != nullptr)
		{
			int nameLength = Date::getDateNameLength(t, f[1]);
			int value = Date::dateNameMap[
				std::string(t, (size_t)nameLength)];

			if (value == 0)
			{
				std::string e("unrecognized name: ");
				e.append(t, (size_t)nameLength);
				throw Exception(TRACE, e);
			}
			else
			{
				*saveNameVariable = value;
				saveNameVariable = nullptr;
				f += 2;
				t += nameLength + 1;
			}
		}
	}

	hour += amOrPm;

	Date date;

	date.setIsPm(amOrPm == 12 ? true : false);
	date.setYear(year);
	date.setMonth(month);
	date.setDay(day);
	date.setHour(hour);
	date.setMinute(min);
	date.setSecond(sec);
	date.setDaysOfWeek(daysOfWeek);

	return date;
}


std::string Date::toString(const char *format) const
{
	const char *f = format;
	char buffer[MAX_DIGITALS + 1];
	char paddingChar = '0';
	int toStringInt = -1;
	std::string result;
	result.reserve(64);

	while (*f != '\0')
	{
		switch (*f)
		{
			case 'y':
				toStringInt = this->year;
				break;
			case 'M':
				toStringInt = (int)this->month;
				break;
			case 'd':
				toStringInt = (int)this->day;
				break;
			case 'h':
				toStringInt = (int)this->hour - (this->isPm ? 12 : 0);
				break;
			case 'H':
				toStringInt = (int)this->hour;
				break;
			case 'm':
				toStringInt = (int)this->min;
				break;
			case 's':
				toStringInt = (int)this->sec;
				break;
			case 'n':
				result.append(Date::getMonthShortName(this->month));
				f += 1;
				break;
			case 'N':
				result.append(Date::getMonthFullName(this->month));
				f += 1;
				break;
			case 'e':
				result.append(Date::getWeekShortName(this->daysOfWeek));
				f += 1;
				break;
			case 'E':
				result.append(Date::getWeekFullName(this->daysOfWeek));
				f += 1;
				break;
			case 'a':
				result.append(this->isPm == true ? "pm" : "am");
				f += 1;
				break;
			case 'A':
				result.append(this->isPm == true ? "PM" : "AM");
				f += 1;
				break;
			case 'z':	// disable digitals padding
				paddingChar = 0;
				f += 1;
				break;
			case 'Z':	// set digitals padding char
				if (f[1] == '=')
				{
					paddingChar = f[2];
					f += 3;
				}
				else
				{
					paddingChar = '0';
					f += 1;
				}
				break;
			default:
				result.push_back(*f);
				f += 1;
				break;
		}

		if (toStringInt != -1)
		{
			int parseDigitals = Date::getParseDigitals(f);
			result.append(Date::intToString(
				toStringInt, parseDigitals, buffer, paddingChar));
			f += parseDigitals;
			toStringInt = -1;
		}
	}

	return result;
}


uint64_t Date::getTimestamp()
{
	return (uint64_t)std::time(0);
}


Date Date::now()
{
	return Date::timestampToDate(Date::getTimestamp());
}


Date Date::timestampToDate(uint64_t seconds)
{
	time_t timestamp = (time_t)seconds;

#if defined(__GNUC__) && !defined(__MINGW32__)
	struct tm timeInfo = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	::localtime_r(&timestamp, &timeInfo);
#elif _MSC_VER
	struct tm timeInfo = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	::localtime_s(&timeInfo, &timestamp);
#else
	struct tm timeInfo = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	struct tm *p = ::localtime(&timestamp);
	if (p != nullptr)	// Statically allocated memory, do not free.
		timeInfo = *p;
#endif

	Date date;

	date.setYear(timeInfo.tm_year + 1900);
	date.setMonth(timeInfo.tm_mon + 1);
	date.setDay(timeInfo.tm_mday);
	date.setHour(timeInfo.tm_hour);
	date.setMinute(timeInfo.tm_min);
	date.setSecond(timeInfo.tm_sec);
	date.setIsPm(timeInfo.tm_hour >= 12);
	date.setDaysOfWeek(timeInfo.tm_wday == 0 ? 7 : timeInfo.tm_wday);

	return date;
}


uint64_t Date::dateToTimestamp(const Date &date)
{
	struct tm timeInfo;

	timeInfo.tm_year = date.getYear() - 1900;
	timeInfo.tm_mon = date.getMonth() - 1;
	timeInfo.tm_mday = date.getDay();
	timeInfo.tm_hour = date.getHour();
	timeInfo.tm_min = date.getMinute();
	timeInfo.tm_sec = date.getSecond();
	timeInfo.tm_wday = date.getDaysOfWeek();
	timeInfo.tm_yday = 0;
	timeInfo.tm_isdst = 0;

	return (uint64_t)::mktime(&timeInfo);
}


int Date::findDaysOfWeek(int year, int month, int day)
{
	uint8_t blankBase[] = { 6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	int blankDay = blankBase[month - 1] + year +
		int(year / 4) - int(year / 100) + int(year / 400);

	if (year % 4 == 0 && month < 3)
	{
		blankDay -= 1;
		if (year % 100 == 0) blankDay += 1;
		if (year % 400 == 0) blankDay -= 1;
	}

	blankDay = (blankDay + day) % 7;

	return blankDay == 0 ? 7 : blankDay;
}


void Date::findDaysOfWeek()
{
	this->daysOfWeek = (uint8_t)
		findDaysOfWeek(year, (int)month, (int)day);
}
