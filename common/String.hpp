#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>


class String
{
public:
	String() = default;
	String(size_t allocSize);
	String(const char *str);
	String(const char *str, size_t strLength);
	String(const std::string &str);
	String(const String &str);
	String(std::string &&str) : stdStr(std::move(str)) {}
	String(String &&str) = default;

	virtual ~String() = default;

	size_t getLength() const { return this->stdStr.length(); }

	String clone() const { return String(this->stdStr); }

	std::string & inner() { return this->stdStr; }
	const std::string & inner() const { return this->stdStr; }

	const char * getCharArray() const { return this->stdStr.data(); }
	char * getCharArray() { return &this->stdStr[0]; }

	std::string && move() { return std::move(this->stdStr); }

	void swap(std::string &str);
	void swap(String &str);

	static char * getSpace(std::string &buffer, size_t offset, size_t length);

	char * getSpace(size_t offset, size_t length);

	void assign(const char ch);
	void assign(const char *str);
	void assign(const char *str, size_t strLength);
	void assign(const std::string &str);
	void assign(const String &str);
	void assign(std::string &&str);
	void assign(String &&str);

	void append(const char ch);
	void append(const char *str);
	void append(const std::string &str);
	void append(const char *str, size_t strLength);
	void append(const String &str);

	static void insert(String &srcStr, size_t startAt, char ch);
	static void insert(String &srcStr, size_t startAt, const char *str);
	static void insert(String &srcStr, size_t startAt, const std::string &str);
	static void insert(String &srcStr, size_t startAt, const char *str, size_t strLength);
	static void insert(String &srcStr, size_t startAt, const String &str);

	String insert(size_t startAt, char ch) const;
	String insert(size_t startAt, const char *str) const;
	String insert(size_t startAt, const std::string &str) const;
	String insert(size_t startAt, const char *str, size_t strLength) const;
	String insert(size_t startAt, const String &str) const;

	static void remove(String &srcStr, size_t startAt);
	static void remove(String &srcStr, size_t startAt, size_t counter);

	String remove(size_t startAt) const;
	String remove(size_t startAt, size_t counter) const;

	static size_t indexOf(const char *str, const char *pattern, size_t startAt = 0);

	size_t indexOf(char pattern, size_t startAt = 0) const;
	size_t indexOf(const char *pattern, size_t startAt = 0) const;
	size_t indexOf(const std::string &pattern, size_t startAt = 0) const;
	size_t indexOf(const char *pattern, size_t patLength, size_t startAt) const;
	size_t indexOf(const String &pattern, size_t startAt = 0) const;

	// static size_t lastIndexOf(const char *str, const char *pattern, size_t startAt = ~0u);

	size_t lastIndexOf(char pattern, size_t startAt = ~0u) const;
	size_t lastIndexOf(const char *pattern, size_t startAt = ~0u) const;
	size_t lastIndexOf(const std::string &pattern, size_t startAt = ~0u) const;
	size_t lastIndexOf(const char *pattern, size_t patLength, size_t startAt) const;
	size_t lastIndexOf(const String &pattern, size_t startAt = ~0u) const;

	static void replace(String &srcStr, char oldCh, char newCh);
	static void replace(String &srcStr, const char *oldStr, const char *newStr);
	static void replace(String &srcStr, const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen);
	static void replace(String &srcStr, const std::string &oldStr, const std::string &newStr);
	static void replace(String &srcStr, const String &oldStr, const String &newStr);

	String replace(char oldCh, char newCh) const;
	String replace(const char *oldStr, const char *newStr) const;
	String replace(const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen) const;
	String replace(const std::string &oldStr, const std::string &newStr) const;
	String replace(const String &oldStr, const String &newStr) const;

	static void replaceFirst(String &srcStr, char oldCh, char newCh);
	static void replaceFirst(String &srcStr, const char *oldStr, const char *newStr);
	static void replaceFirst(String &srcStr, const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen);
	static void replaceFirst(String &srcStr, const std::string &oldStr, const std::string &newStr);
	static void replaceFirst(String &srcStr, const String &oldStr, const String &newStr);

	String replaceFirst(char oldCh, char newCh) const;
	String replaceFirst(const char *oldStr, const char *newStr) const;
	String replaceFirst(const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen) const;
	String replaceFirst(const std::string &oldStr, const std::string &newStr) const;
	String replaceFirst(const String &oldStr, const String &newStr) const;

	static bool startsWith(const char *str, const char *prefix);
	static bool startsWith(const char *str, size_t strLen, const char *prefix, size_t prefixLen);

	bool startsWith(const char *prefix) const;
	bool startsWith(const char *prefix, size_t prefixLen) const;
	bool startsWith(const std::string &prefix) const;
	bool startsWith(const String &prefix) const;

	static bool endsWith(const char *str, const char *suffix);
	static bool endsWith(const char *str, size_t strLen, const char *suffix, size_t suffixLen);

	bool endsWith(const char *suffix) const;
	bool endsWith(const char *suffix, size_t suffixLen) const;
	bool endsWith(const std::string &suffix) const;
	bool endsWith(const String &suffix) const;

	static void substring(char *str, size_t startAt);
	static void substring(char *str, size_t startAt, size_t length);

	static void substring(String &str, size_t startAt);
	static void substring(String &str, size_t startAt, size_t length);

	String substring(size_t startAt) const;
	String substring(size_t startAt, size_t length) const;

	static size_t trimStartIndex(const char *src, size_t srcLen);
	static size_t trimEndIndex(const char *src, size_t srcLen, size_t left);

	static void trim(char *srcStr);
	static void trimStart(char *srcStr);
	static void trimEnd(char *srcStr);

	static void trim(char *srcStr, size_t srcLen);
	static void trimStart(char *srcStr, size_t srcLen);
	static void trimEnd(char *srcStr, size_t srcLen);

	static void trim(String &srcStr);
	static void trimStart(String &srcStr);
	static void trimEnd(String &srcStr);

	String trim() const;
	String trimStart() const;
	String trimEnd() const;

	bool contains(const char *str) const;
	bool contains(const char *str, size_t strLength) const;
	bool contains(const std::string &str) const;
	bool contains(const String &str) const;

	int compare(const char *str) const;
	int compare(const char *str, size_t strLength) const;
	int compare(const std::string &str) const;
	int compare(const String &str) const;

	static int compareIgnoreCase(const char *lhStr, const char *rhStr);
	static int compareIgnoreCase(const std::string &lhStr, const std::string &rhStr);

	int compareIgnoreCase(const char *str) const;
	int compareIgnoreCase(const char *str, size_t strLength) const;
	int compareIgnoreCase(const std::string &str) const;
	int compareIgnoreCase(const String &str) const;

	static bool equals(const char *lhStr, const char *rhStr);

	bool equals(const char *str) const;
	bool equals(const char *str, size_t strLength) const;
	bool equals(const std::string &str) const;
	bool equals(const String &str) const;

	static bool equalsIgnoreCase(const char *lhStr, const char *rhStr);
	static bool equalsIgnoreCase(const std::string &lhStr, const std::string &rhStr);

	bool equalsIgnoreCase(const char *str);
	bool equalsIgnoreCase(const std::string &str);
	bool equalsIgnoreCase(const String &str);

	static void toUpper(char *str);
	static void toUpper(char *str, size_t strLength);
	static void toUpper(std::string &str);
	static void toUpper(String &str);

	static void toLower(char *str);
	static void toLower(char *str, size_t strLength);
	static void toLower(std::string &str);
	static void toLower(String &str);

	String toUpper() const;
	String toLower() const;

	std::vector<String> split(char separator) const;
	std::vector<String> split(const char *separator) const;
	std::vector<String> split(const char *separator, size_t sepLen) const;
	std::vector<String> split(const std::string &separator) const;
	std::vector<String> split(const String &separator) const;

	static String join(char separator, std::vector<String> strs);
	static String join(const char *separator, std::vector<String> strs);
	static String join(const char *separator, size_t sepLength, std::vector<String> strs);
	static String join(const std::string &separator, std::vector<String> strs);
	static String join(const String &separator, std::vector<String> strs);

	static size_t extract(const std::string &srcStr, const std::string &startStr);
	static size_t extract(const std::string &srcStr, size_t srcOffset, const std::string &startStr);
	static std::string extract(const std::string &srcStr, const std::string &startStr, const std::string &endStr);
	static std::string extract(const std::string &srcStr, size_t srcOffset,
		const std::string &startStr, const std::string &endStr);

	size_t extract(const std::string &startStr);
	size_t extract(const String &startStr);
	size_t extract(size_t srcOffset, const std::string &startStr);
	size_t extract(size_t srcOffset, const String &startStr);
	String extract(size_t srcOffset, const std::string &startStr, const std::string &endStr);
	String extract(size_t srcOffset, const String &startStr, const String &endStr);

	void clear();

	void reserve(size_t size);

	void resize(size_t size);
	void resize(size_t size, char ch);

	std::string::iterator begin() { return this->stdStr.begin(); }
	std::string::iterator end() { return this->stdStr.end(); }
	std::string::const_iterator begin() const { return this->stdStr.begin(); }
	std::string::const_iterator end() const { return this->stdStr.end(); }

	static String format(const char *args, ...);
	static String format(const char *args, va_list va);
	static String format(size_t bifferSize, const char *args, ...);
	static void format(String &outStr, const char *args, ...);
	static void format(String &outStr, size_t bufferSize, const char *args, ...);
	static void format(String &outStr, size_t bufferSize, const char *args, va_list va);

	static void * malloc(size_t size);
	static void * realloc(void *oldPointer, size_t oldSize, size_t newSize);
	static void free(void *p);

	String operator + (const char ch) { return String(this->inner() + ch); }
	String operator + (const char *str) { return String(this->inner() + str); }
	String operator + (const std::string &str) { return String(this->inner() + str); }
	String operator + (const String &str) { return String(this->inner() + str.inner()); }

	String & operator += (const char ch) { this->append(ch); return *this; }
	String & operator += (const char *str) { this->append(str); return *this; }
	String & operator += (const std::string &str) { this->append(str); return *this; }
	String & operator += (const String &str) { this->append(str); return *this; }

	String & operator = (const char ch) { this->assign(ch); return *this; }
	String & operator = (const char *str) { this->assign(str); return *this; }
	String & operator = (const std::string &str) { this->assign(str); return *this; }
	String & operator = (const String &str) { this->assign(str); return *this; }
	String & operator = (std::string &&str) { this->assign(std::move(str)); return *this; }
	String & operator = (String &&str) = default;

	bool operator == (const char *str) const { return this->equals(str); }
	bool operator == (const std::string &str) const { return this->equals(str); }
	bool operator == (const String &str) const { return this->equals(str); }

	bool operator != (const char *str) const { return this->equals(str) == false; }
	bool operator != (const std::string &str) const { return this->equals(str) == false; }
	bool operator != (const String &str) const { return this->equals(str) == false; }

	char & operator [] (int index) { return this->stdStr[(size_t)index]; }
	char & operator [] (size_t index) { return this->stdStr[index]; }

	operator std::string & () { return this->inner(); }
	operator const std::string & () const { return this->inner(); }
	operator const char * () const { return this->stdStr.data(); }
	operator char * () { return &this->stdStr[0]; }

private:
	std::string stdStr;
};


String operator + (const char lhCh, const String &rhStr);
String operator + (const char *lhStr, const String &rhStr);
String operator + (const std::string &lhStr, const String &rhStr);
String operator + (const String &lhStr, const String &rhStr);
String operator + (std::string &&lhStr, const String &rhStr);
String operator + (String &&lhStr, const String &rhStr);

inline bool operator == (const char *lhStr, const String &rhStr) { return rhStr.equals(lhStr); }
inline bool operator == (const std::string &lhStr, const String &rhStr) { return rhStr.equals(lhStr); }

inline bool operator != (const char *lhStr, const String &rhStr) { return rhStr.equals(lhStr) == false; }
inline bool operator != (const std::string &lhStr, const String &rhStr) { return rhStr.equals(lhStr) == false; }

inline std::ostream & operator << (std::ostream &strm, const String &rhStr) { return strm << rhStr.inner(); }

#endif
