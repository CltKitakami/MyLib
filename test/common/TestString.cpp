#include <iostream>
#include <cstdlib>
#include "common/Log.hpp"
#include "common/String.hpp"
#include "common/Stopwatch.hpp"
#include "common/Exception.hpp"


Log & operator << (Log &LOG, const String &str)
{ return LOG << str.inner(); }


void testAlloc();
void testOperater();
void testInsert();
void testRemove();
void testIndexOf();
void testReplace();
void testSubstring();
void testTrim();
void testContains();
void testCompare();
void testSplitJoin();

void testAppendTime();
void testStdAppendTime();

void testInsertTime();
void testStdInsertTime();

void testRemoveTime();
void testStdRemoveTime();

void testIndexOfTime();
void testStdIndexOfTime();


void testTime()
{
	String str1("@");
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		for (int i = 0; i < 1 << 16; ++i)
		{
			String str2(String("#"));
			str1.append(str2);
		}
	}

	stopWatch.stop();
	LOGT("String[" << str1.getLength() << "]: "
			<< stopWatch.getMicros() << " us");
}


int main()
{
	LOG.setFileInfo(false);

	testTime();

	testAlloc();
	testOperater();
	testInsert();
	testRemove();
	testIndexOf();
	testReplace();
	testSubstring();
	testTrim();
	testContains();
	testCompare();
	testSplitJoin();

	LOGT("\nAppendTime");
	testAppendTime();
	testStdAppendTime();

	LOGT("\nInsertTime");
	testInsertTime();
	testStdInsertTime();

	LOGT("\nRemoveTime");
	testRemoveTime();
	testStdRemoveTime();

	LOGT("\nIndexOfTime");
	testIndexOfTime();
	testStdIndexOfTime();


	String s;
	char *data = s.getSpace(0, 256);

	const char *arr = s.getCharArray();

	for (int i = 0; i < 256; ++i)
	{
		*data++ = (char)((i % 10) + '0');
	}

	*data = '\0';
	LOG_VAR(arr);
	LOG_VAR(s.getLength());
	s.resize(10);
	LOG_VAR(arr);
	LOG_VAR(s.getLength());

	String formated = String::format("%s%c", "Is it work?", '\n');
	LOG_VAR(formated);

	return 0;
}


void testAlloc()
{
	String str1(1);
	str1.append("12345678");
	LOGT("testAlloc: \"" << str1 << "\"");
}


void testOperater()
{
	String str1;
	char ch = '!';
	char space = ' ';
	char cstr[10] = "@_@";
	std::string str = "#";
	str1 += ch;
	str1 += cstr;
	str1 += str;
	str1 += str1;
	str1 = space + str1;
	str1 = str1 + space;
	str1 = str1 + cstr;
	str1 = cstr + str1;
	str1 = str1 + str;
	str1 = str + str1;
	str1[0] = '?';
	LOGT("testOperater: \"" << (const char *)str1 << "\"");
}


void testInsert()
{
	String str1("'m String");
	LOGT(str1);
	str1 = str1.insert(0, 'I');
	LOGT(str1);
	str1 = str1.insert(4, "inserted ");
	LOGT(str1);
	str1 = str1.insert(str1.getLength(), '.');
	LOGT(str1);
}


void testRemove()
{
	String str1("I'm removed String.");
	LOGT(str1);
	str1 = str1.remove(str1.getLength() - 1, 1);
	LOGT(str1);
	str1 = str1.remove(0, 1);
	LOGT(str1);
	str1 = str1.remove(3, 8);
	LOGT(str1);
}


void testIndexOf()
{
	String str1("abdbcabc");
	LOGT("\"abc\" indexOf: " << (int)str1.indexOf("abc"));
	LOGT("\"d\" indexOf: " << (int)str1.indexOf('d'));
	LOGT("str1 indexOf: " << (int)str1.indexOf("abdbcabc"));

	LOGT("\"ab\" lastIndexOf: " << (int)str1.lastIndexOf("ab"));
	LOGT("\"ab\" lastIndexOf: " << (int)str1.lastIndexOf("ab", 4));
}


void testReplace()
{
	String str1("I'm old old String String.");
	str1 = str1.replace("old", "new");
	LOGT(str1);
	str1 = str1.replaceFirst("String", "@");
	LOGT(str1);
}

void testSubstring()
{
	String str1("I'm substring.");
	LOGT(str1.substring(4));
	LOGT(str1.substring(4, 9));
}


void testTrim()
{
	String str1(" \t\r\n .  string  .  \t\r\n ");
	String str2 = str1;
	String::trim(str2);
	LOGT(str2);
	str2 = str1;
	String::trimStart(str2);
	LOGT(str2);
	str2 = str1;
	String::trimEnd(str2);
	LOGT(str2);
	LOGT(str1.trim());
	LOGT(str1.trimStart());
	LOGT(str1.trimEnd());
}


void testContains()
{
	String str1("abcdefg1234567");
	if (str1.contains("g123") == true)
		LOGT("str1 contains(\"g123\")");
	else
		LOGT("str1 not contains(\"g123\")");
}


void testCompare()
{
	String str1("123abc");
	if (str1.compare("123abc") == 0)
		LOGT("str1 == \"123abc\"");
	else
		LOGT("str1 != \"123abc\"");
}


void testSplitJoin()
{
	String str1("a,b,c,d, e, f, g.h.i,,,");
	std::vector<String> splits = str1.split(",", 1);

	for (std::vector<String>::iterator it = splits.begin(),
		end = splits.end(); it != end; ++it)
	{
		LOGT("\"" << *it << "\"\n");
	}

	LOGT(str1);
	LOGT(String::join(",", splits));
}


void testAppendTime()
{
	String str1("@");
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		for (int i = 0; i < 1 << 16; ++i)
		{
			str1.append("@@", 2);
		}
	}

	stopWatch.stop();
	LOGT("String[" << str1.getLength() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testStdAppendTime()
{
	std::string str1 = "@";
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		for (int i = 0; i < 1 << 16; ++i)
		{
			str1.append("@@", 2);
		}
	}

	stopWatch.stop();
	LOGT("std::string[" << str1.length() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testInsertTime()
{
	String str1;
	String s("@");
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 5; ++j)
	{
		for (int i = 0; i < 1 << 10; ++i)
		{
			String::insert(str1, 0, s);
		}
	}

	stopWatch.stop();
	LOGT("String[" << str1.getLength() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testStdInsertTime()
{
	std::string str1 = "";
	std::string s = "@";
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 5; ++j)
	{
		for (int i = 0; i < 1 << 10; ++i)
		{
			str1.insert(0, s);
		}
	}

	stopWatch.stop();
	LOGT("std::string[" << str1.length() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testRemoveTime()
{
	String str1 = "@";
	String s("@");

	for (int i = 0; i < 1 << 16; ++i)
		String::insert(str1, 0, s);

	s = str1;
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		str1 = s;
		for (int i = 0; i < 1 << 16; ++i)
			String::remove(str1, 0, 1);
	}

	stopWatch.stop();
	LOGT("String[" << str1.getLength() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testStdRemoveTime()
{
	std::string str1 = "@";
	std::string s = "@";

	for (int i = 0; i < 1 << 16; ++i)
		str1.insert(0, s);

	s = str1;
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		str1 = s;
		for (int i = 0; i < 1 << 16; ++i)
			str1.erase(0, 1);
	}

	stopWatch.stop();
	LOGT("std::string[" << str1.length() << "]: "
			<< stopWatch.getMicros() << " us");
}


void testIndexOfTime()
{
	String str1(255);
	char pat[16];

	for (int i = 0; i < 255; ++i)
		str1 += (char)(i + 1);

	for (int i = 0; i < 16; ++i)
		pat[i] = (char)(i + 255 - 16);
	pat[15] = '\0';

	size_t find = ~0u;
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
		for (int i = 0; i < 1 << 16; ++i)
		{
			find = str1.indexOf(pat, 15, 0);
		}

	stopWatch.stop();
	LOGT("String[" << find << "]: "
			<< stopWatch.getMicros() << " us");
}


void testStdIndexOfTime()
{
	std::string str1 = "";
	char pat[16];

	for (int i = 0; i < 255; ++i)
		str1 += (char)(i + 1);

	for (int i = 1; i < 16; ++i)
		pat[i] = (char)(i + 255 - 16);
	pat[15] = '\0';

	int find = -1;
	Stopwatch stopWatch;
	stopWatch.start();

	for (int j = 0; j < 1 << 4; ++j)
	{
		for (int i = 0; i < 1 << 16; ++i)
		{
			find = (int)str1.find(pat, 0, 15);
		}
	}

	stopWatch.stop();
	LOGT("std::string[" << find << "]: "
			<< stopWatch.getMicros() << " us");
}
