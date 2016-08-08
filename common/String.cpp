#include <stdarg.h>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <limits>

#include "String.hpp"
#include "common/Exception.hpp"


#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp  _strnicmp
#endif

#define ASSERT_INDEX_RANGE(value, max)\
do\
{\
	if ((value) > max)\
	{\
		Exception ex(TRACE, "");\
		ex << '\"' << #value << "\" = " << (value) << " is out of ";\
		ex << '(' << #max << ')' << max;\
		throw ex;\
	}\
} while (0)

#define STR_EX_ADJUST_SIZE  "Adjust size failed"
#define STR_EX_BAD_ALLOCATE "Bad allocated memory"
#define STR_EX_OUT_OF_RANGE "Out of range"


String::String(size_t allocSize)
{
	this->stdStr.reserve(allocSize);
}


String::String(const char *str)
{
	this->stdStr.assign(str);
}


String::String(const char *str, size_t strLength)
{
	this->stdStr.assign(str, strLength);
}


String::String(const std::string &str)
{
	this->stdStr.assign(str);
}


String::String(const String &str)
{
	this->stdStr.assign(str.inner());
}


void String::swap(std::string &str)
{
	this->stdStr.swap(str);
}


void String::swap(String &str)
{
	this->stdStr.swap(str.inner());
}


char * String::getSpace(std::string &buffer, size_t offset, size_t length)
{
	buffer.resize(offset + length);
	return &buffer[offset];
}


char * String::getSpace(size_t offset, size_t length)
{
	return String::getSpace(this->inner(), offset, length);
}


void String::assign(const char ch)
{
	this->stdStr = ch;
}


void String::assign(const char *str)
{
	this->assign(str, ::strlen(str));
}


void String::assign(const char *str, size_t strLength)
{
	this->stdStr.assign(str, strLength);
}


void String::assign(const std::string &str)
{
	this->stdStr.assign(str);
}


void String::assign(const String &str)
{
	this->stdStr.assign(str.inner());
}


void String::assign(std::string &&str)
{
	this->stdStr.assign(std::move(str));
}


void String::assign(String &&str)
{
	this->stdStr.assign(std::move(str.move()));
}


void String::append(const char ch)
{
	this->stdStr.push_back(ch);
}


void String::append(const char *str)
{
	this->stdStr.append(str);
}


void String::append(const std::string &str)
{
	this->stdStr.append(str);
}


void String::append(const char *str, size_t strLength)
{
	this->stdStr.append(str, strLength);
}


void String::append(const String &str)
{
	this->append(str.getCharArray(), str.getLength());
}


void String::insert(String &srcStr, size_t startAt, char ch)
{
	String::insert(srcStr, startAt, &ch, 1);
}


void String::insert(String &srcStr, size_t startAt, const char *str)
{
	String::insert(srcStr, startAt, str, ::strlen(str));
}


void String::insert(String &srcStr, size_t startAt, const char *str, size_t strLength)
{
	ASSERT_INDEX_RANGE(startAt, srcStr.getLength());
	srcStr.inner().insert(startAt, str, strLength);
}


void String::insert(String &srcStr, size_t startAt, const std::string &str)
{
	String::insert(srcStr, startAt, str.data(), str.length());
}


void String::insert(String &srcStr, size_t startAt, const String &str)
{
	String::insert(srcStr, startAt, str.getCharArray(), str.getLength());
}


String String::insert(size_t startAt, char ch) const
{
	String tmpStr(this->stdStr);
	String::insert(tmpStr, startAt, &ch, 1);
	return tmpStr;
}


String String::insert(size_t startAt, const char *str) const
{
	String tmpStr(this->stdStr);
	String::insert(tmpStr, startAt, str, ::strlen(str));
	return tmpStr;
}


String String::insert(size_t startAt, const char *str, size_t strLength) const
{
	String tmpStr(this->stdStr);
	String::insert(tmpStr, startAt, str, strLength);
	return tmpStr;
}


String String::insert(size_t startAt, const std::string &str) const
{
	String tmpStr(this->stdStr);
	String::insert(tmpStr, startAt, str.data(), str.length());
	return tmpStr;
}


String String::insert(size_t startAt, const String &str) const
{
	String tmpStr(this->stdStr);
	String::insert(tmpStr, startAt, str.getCharArray(), str.getLength());
	return tmpStr;
}


void String::remove(String &srcStr, size_t startAt)
{
	String::remove(srcStr, startAt, srcStr.getLength() - startAt);
}


void String::remove(String &srcStr, size_t startAt, size_t counter)
{
	ASSERT_INDEX_RANGE(startAt + counter, srcStr.getLength());
	srcStr.inner().erase(startAt, counter);
}


String String::remove(size_t startAt) const
{
	String tmpStr(this->stdStr);
	String::remove(tmpStr, startAt, this->getLength() - startAt);
	return tmpStr;
}


String String::remove(size_t startAt, size_t counter) const
{
	String tmpStr(this->stdStr);
	String::remove(tmpStr, startAt, counter);
	return tmpStr;
}


size_t String::indexOf(const char *str, const char *pattern, size_t startAt)
{
	ASSERT_INDEX_RANGE(startAt, ::strlen(str));
	const char *findAt = ::strstr(str + startAt, pattern);
	return findAt == nullptr ? ~0u : (size_t)(findAt - str);
}


size_t String::indexOf(char pattern, size_t startAt) const
{
	ASSERT_INDEX_RANGE(startAt, this->stdStr.length());
	return this->stdStr.find(pattern, startAt);
}


size_t String::indexOf(const char *pattern, size_t startAt) const
{
	return this->indexOf(pattern, ::strlen(pattern), startAt);
}


size_t String::indexOf(const char *pattern, size_t patLen, size_t startAt) const
{
	return this->stdStr.find(pattern, startAt, patLen);
}


size_t String::indexOf(const std::string &pattern, size_t startAt) const
{
	return this->indexOf(pattern.data(), pattern.length(), startAt);
}


size_t String::indexOf(const String &pattern, size_t startAt) const
{
	return this->indexOf(pattern.getCharArray(), pattern.getLength(), startAt);
}


size_t String::lastIndexOf(char pattern, size_t startAt) const
{
	ASSERT_INDEX_RANGE(startAt, this->stdStr.length());
	return this->stdStr.rfind(pattern, startAt);
}


size_t String::lastIndexOf(const char *pattern, size_t startAt) const
{
	return lastIndexOf(pattern, ::strlen(pattern),
		startAt == ~0u ? this->getLength() - 1 : startAt);
}


size_t String::lastIndexOf(const char *pattern, size_t patLen, size_t startAt) const
{
	return this->stdStr.rfind(pattern, startAt, patLen);
}


size_t String::lastIndexOf(const std::string &pattern, size_t startAt) const
{
	return lastIndexOf(pattern.data(), pattern.length(),
		startAt == ~0u ? this->getLength() - 1 : startAt);
}


size_t String::lastIndexOf(const String &pattern, size_t startAt) const
{
	return lastIndexOf(pattern.getCharArray(), pattern.getLength(),
		startAt == ~0u ? this->getLength() - 1 : startAt);
}


void String::replace(String &srcStr, char oldCh, char newCh)
{
	for (std::string::iterator i = srcStr.begin(),
		end = srcStr.end(); i != end; ++i)
	{
		if ((char)(*i) == oldCh)
			*i = newCh;
	}
}


void String::replace(String &srcStr, const char *oldStr, const char *newStr)
{
	String::replace(srcStr, oldStr, ::strlen(oldStr), newStr, ::strlen(newStr));
}


void String::replace(String &srcStr, const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen)
{
	size_t srcIndex = 0;
	size_t srcLen = srcStr.getLength();
	const char *srcBuf = srcStr.getCharArray();
	std::string tmpBuf;

	tmpBuf.reserve(newStrLen > oldStrLen ?
		srcLen + 10 * (newStrLen - oldStrLen) : srcLen);

	for (size_t i = srcStr.indexOf(oldStr, oldStrLen, 0);
		 i != ~0u;
		 i = srcStr.indexOf(oldStr, oldStrLen, srcIndex))
	{
		tmpBuf.append(&srcBuf[srcIndex], (size_t)(i - srcIndex));
		tmpBuf.append(newStr, (size_t)newStrLen);
		srcIndex = i + oldStrLen;

		if (srcIndex >= srcLen)
			break;
	}

	tmpBuf.append(&srcBuf[srcIndex], (size_t)(srcLen - srcIndex));
	srcStr.swap(tmpBuf);
}


void String::replace(String &srcStr, const std::string &oldStr, const std::string &newStr)
{
	String::replace(srcStr, oldStr.data(), oldStr.length(),
		newStr.data(), newStr.length());
}


void String::replace(String &srcStr, const String &oldStr, const String &newStr)
{
	String::replace(srcStr, oldStr.getCharArray(), oldStr.getLength(),
		newStr.getCharArray(), newStr.getLength());
}


String String::replace(char oldCh, char newCh) const
{
	String tmpStr(this->stdStr);
	String::replace(tmpStr, oldCh, newCh);
	return tmpStr;
}


String String::replace(const char *oldStr, const char *newStr) const
{
	String tmpStr(this->stdStr);
	String::replace(tmpStr, oldStr, ::strlen(oldStr), newStr, ::strlen(newStr));
	return tmpStr;
}


String String::replace(const char *oldStr, size_t oldStrLen, const char *newStr, size_t newStrLen) const
{
	String tmpStr(this->stdStr);
	String::replace(tmpStr, oldStr, oldStrLen, newStr, newStrLen);
	return tmpStr;
}


String String::replace(const std::string &oldStr, const std::string &newStr) const
{
	String tmpStr(this->stdStr);
	String::replace(tmpStr, oldStr.data(), oldStr.length(), newStr.data(), newStr.length());
	return tmpStr;
}


String String::replace(const String &oldStr, const String &newStr) const
{
	String tmpStr(this->stdStr);
	String::replace(tmpStr, oldStr.getCharArray(), oldStr.getLength(), newStr.getCharArray(), newStr.getLength());
	return tmpStr;
}


void String::replaceFirst(String &srcStr, char oldCh, char newCh)
{
	for (std::string::iterator i = srcStr.begin(),
		end = srcStr.end(); i != end; ++i)
	{
		if ((char)*i == oldCh)
		{
			*i = newCh;
			break;
		}
	}
}


void String::replaceFirst(String &srcStr, const char *oldStr, const char *newStr)
{
	String::replaceFirst(srcStr, oldStr, ::strlen(oldStr), newStr, ::strlen(newStr));
}


void String::replaceFirst(String &srcStr, const char *oldStr,
	size_t oldStrLen, const char *newStr, size_t newStrLen)
{
	size_t findAt = srcStr.indexOf(oldStr, oldStrLen, 0);

	if (findAt != ~0u)
		srcStr.inner().replace(findAt, oldStrLen, newStr, newStrLen);
}


void String::replaceFirst(String &srcStr, const std::string &oldStr, const std::string &newStr)
{
	String::replaceFirst(srcStr, oldStr.data(), oldStr.length(),
		newStr.data(), newStr.length());
}


void String::replaceFirst(String &srcStr, const String &oldStr, const String &newStr)
{
	String::replaceFirst(srcStr, oldStr.getCharArray(), oldStr.getLength(),
		newStr.getCharArray(), newStr.getLength());
}


String String::replaceFirst(char oldCh, char newCh) const
{
	String tmpStr(this->stdStr);
	String::replaceFirst(tmpStr, oldCh, newCh);
	return tmpStr;
}


String String::replaceFirst(const char *oldStr, const char *newStr) const
{
	String tmpStr(this->stdStr);
	String::replaceFirst(tmpStr, oldStr, ::strlen(oldStr), newStr, ::strlen(newStr));
	return tmpStr;
}


String String::replaceFirst(const char *oldStr, size_t oldStrLen,
	const char *newStr, size_t newStrLen) const
{
	String tmpStr(this->stdStr);
	String::replaceFirst(tmpStr, oldStr, oldStrLen, newStr, newStrLen);
	return tmpStr;
}


String String::replaceFirst(const std::string &oldStr,
	const std::string &newStr) const
{
	String tmpStr(this->stdStr);
	String::replaceFirst(tmpStr, oldStr.data(), oldStr.length(),
		newStr.data(), newStr.length());
	return tmpStr;
}


String String::replaceFirst(const String &oldStr, const String &newStr) const
{
	String tmpStr(this->stdStr);
	String::replaceFirst(tmpStr, oldStr.getCharArray(), oldStr.getLength(),
		newStr.getCharArray(), newStr.getLength());
	return tmpStr;
}


bool String::startsWith(const char *str, const char *prefix)
{
	return String::startsWith(str, ::strlen(str), prefix, ::strlen(prefix));
}


bool String::startsWith(const char *str, size_t strLength,
	const char *prefix, size_t prefixLength)
{
	if (prefixLength <= strLength)
		return ::memcmp(str, prefix, prefixLength) == 0;
	else
		return false;
}


bool String::startsWith(const char *prefix) const
{
	return this->startsWith(prefix, ::strlen(prefix));
}


bool String::startsWith(const char *prefix, size_t prefixLength) const
{
	if (prefixLength <= this->getLength())
		return ::memcmp(this->getCharArray(), prefix, prefixLength) == 0;
	else
		return false;
}


bool String::startsWith(const std::string &prefix) const
{
	return this->startsWith(prefix.data(), prefix.length());
}


bool String::startsWith(const String &prefix) const
{
	return this->startsWith(prefix.getCharArray(), prefix.getLength());
}


bool String::endsWith(const char *str, const char *suffix)
{
	return String::endsWith(str, ::strlen(str), suffix, ::strlen(suffix));
}


bool String::endsWith(const char *str, size_t strLength,
	const char *suffix, size_t suffixLength)
{
	const char *src = str + strLength - suffixLength;

	if (suffixLength <= strLength)
		return ::memcmp(src, suffix, suffixLength) == 0;
	else
		return false;
}


bool String::endsWith(const char *suffix) const
{
	return this->endsWith(suffix, ::strlen(suffix));
}


bool String::endsWith(const char *suffix, size_t suffixLength) const
{
	const char *src = this->getCharArray() + this->getLength() - suffixLength;

	if (suffixLength <= this->getLength())
		return ::memcmp(src, suffix, suffixLength) == 0;
	else
		return false;
}


bool String::endsWith(const std::string &suffix) const
{
	return this->endsWith(suffix.data(), suffix.length());
}


bool String::endsWith(const String &suffix) const
{
	return this->endsWith(suffix.getCharArray(), suffix.getLength());
}


void String::substring(char *str, size_t startAt)
{
	String::substring(str, startAt, ::strlen(str));
}


void String::substring(char *str, size_t startAt, size_t length)
{
	ASSERT_INDEX_RANGE(startAt + length, ::strlen(str));
	::memmove(str, str + startAt, length);
	str[length] = '\0';
}


void String::substring(String &str, size_t startAt)
{
	String::substring(str, startAt, str.getLength());
}


void String::substring(String &str, size_t startAt, size_t length)
{
	ASSERT_INDEX_RANGE(startAt + length, str.getLength());

	for (size_t i = 0; i <= length - startAt; ++i)
		str[i] = str[i + startAt];

	str.resize(length);
}


String String::substring(size_t startAt) const
{
	ASSERT_INDEX_RANGE(startAt, this->getLength());
	return this->substring(startAt, this->getLength() - startAt);
}


String String::substring(size_t startAt, size_t length) const
{
	return String(this->stdStr.substr(startAt, length));
}


size_t String::trimStartIndex(const char *src, size_t srcLen)
{
	const char *start = src, *end = src + srcLen;

	while ((unsigned char)*start <= (unsigned char)' ')
		if (++start >= end) break;

	return (size_t)(start - src);
}


size_t String::trimEndIndex(const char *src, size_t srcLen, size_t left)
{
	const char *start = src + srcLen - 1, *end = src + left;

	if (start > end)
	{
		while ((unsigned char)*start <= (unsigned char)' ')
			if (--start == end) break;

		return (size_t)(start - src);
	}
	else
		return left;
}


void String::trim(char *srcStr)
{
	String::trim(srcStr, ::strlen(srcStr));
}


void String::trimStart(char *srcStr)
{
	String::trimStart(srcStr, ::strlen(srcStr));
}


void String::trimEnd(char *srcStr)
{
	String::trimEnd(srcStr, ::strlen(srcStr));
}


void String::trim(char *srcStr, size_t srcLength)
{
	if (srcLength > 0)
	{
		size_t i = String::trimStartIndex(srcStr, srcLength);
		size_t j = String::trimEndIndex(srcStr, srcLength, i);
		size_t len = j - i + 1;
		String::substring(srcStr, i, len);
	}
}


void String::trimStart(char *srcStr, size_t srcLength)
{
	if (srcLength > 0)
	{
		size_t i = String::trimStartIndex(srcStr, srcLength);
		String::substring(srcStr, i, srcLength - i);
	}
}


void String::trimEnd(char *srcStr, size_t srcLength)
{
	if (srcLength > 0)
	{
		size_t j = String::trimEndIndex(srcStr, srcLength, 0);
		String::substring(srcStr, 0, j + 1);
	}
}


void String::trim(String &srcStr)
{
	size_t srcLen = srcStr.getLength();

	if (srcLen > 0)
	{
		const char *src = srcStr.getCharArray();
		size_t i = String::trimStartIndex(src, srcLen);
		size_t j = String::trimEndIndex(src, srcLen, i);
		size_t len = j - i + 1;

		for (size_t index = 0; index < len; ++index)
			srcStr[index] = srcStr[index + i];

		srcStr.resize(len);
	}
}


void String::trimStart(String &srcStr)
{
	size_t srcLen = srcStr.getLength();

	if (srcLen > 0)
	{
		const char *src = srcStr.getCharArray();
		size_t i = String::trimStartIndex(src, srcLen);
		size_t len = srcLen - i;

		for (size_t index = 0; index < len; ++index)
			srcStr[index] = srcStr[index + i];

		srcStr.resize(len);
	}
}


void String::trimEnd(String &srcStr)
{
	size_t srcLen = srcStr.getLength();

	if (srcLen > 0)
	{
		const char *src = srcStr.getCharArray();
		size_t len = String::trimEndIndex(src, srcLen, 0) + 1;
		srcStr.resize(len);
	}
}


String String::trim() const
{
	size_t srcLen = this->getLength();

	if (srcLen > 0)
	{
		const char *src = this->getCharArray();
		size_t i = String::trimStartIndex(src, srcLen);
		size_t j = String::trimEndIndex(src, srcLen, i);

		return this->substring(i, j - i + 1);
	}
	else
		return String();
}


String String::trimStart() const
{
	size_t srcLen = this->getLength();

	if (srcLen > 0)
	{
		const char *src = this->getCharArray();
		size_t i = String::trimStartIndex(src, srcLen);

		return this->substring(i, srcLen - i);
	}
	else
		return String();

}


String String::trimEnd() const
{
	size_t srcLen = this->getLength();

	if (srcLen > 0)
	{
		const char *src = this->getCharArray();
		size_t j = String::trimEndIndex(src, srcLen, 0);

		return this->substring((size_t)0, j + 1);
	}
	else
		return String();
}


bool String::contains(const char *str) const
{
	return this->indexOf(str) != ~0u;
}


bool String::contains(const char *str, size_t strLength) const
{
	return indexOf(str, strLength) != ~0u;
}

bool String::contains(const std::string &str) const
{
	return this->indexOf(str) != ~0u;
}


bool String::contains(const String &str) const
{
	return this->indexOf(str) != ~0u;
}


int String::compare(const char *str) const
{
	return this->compare(str, ::strlen(str));
}


int String::compare(const char *str, size_t strLength) const
{
	return stdStr.compare(0, strLength, str);
}


int String::compare(const std::string &str) const
{
	return this->compare(str.data(), str.length());
}


int String::compare(const String &str) const
{
	return this->compare(str.getCharArray(), str.getLength());
}


int String::compareIgnoreCase(const char *lhStr, const char *rhStr)
{
	std::string lhsLower(lhStr), rhsLower(rhStr);
	String::toLower(lhsLower);
	String::toLower(rhsLower);
	return lhsLower.compare(rhsLower);
}


int String::compareIgnoreCase(const std::string &lhStr, const std::string &rhStr)
{
	std::string lhsLower(lhStr), rhsLower(rhStr);
	String::toLower(lhsLower);
	String::toLower(rhsLower);
	return lhsLower.compare(rhsLower);
}


int String::compareIgnoreCase(const char *str) const
{
	return this->compareIgnoreCase(str, ::strlen(str));
}


int String::compareIgnoreCase(const char *str, size_t strLen) const
{
	return String::compareIgnoreCase(this->stdStr, std::string(str, strLen));
}


int String::compareIgnoreCase(const std::string &str) const
{
	return String::compareIgnoreCase(this->stdStr, str);
}


int String::compareIgnoreCase(const String &str) const
{
	return String::compareIgnoreCase(this->stdStr, str.inner());
}


bool String::equals(const char *lhStr, const char *rhStr)
{
	size_t lhStrLen = ::strlen(lhStr);
	size_t rhStrLen = ::strlen(rhStr);

	if (lhStrLen == rhStrLen)
		return ::memcmp(lhStr, rhStr, lhStrLen) == 0;
	else
		return false;
}


bool String::equals(const char *str) const
{
	return this->equals(str, ::strlen(str));
}


bool String::equals(const char *str, size_t strLength) const
{
	if (this->stdStr.length() == strLength)
		return ::memcmp(this->getCharArray(), str, strLength) == 0;
	else
		return false;
}


bool String::equals(const std::string &str) const
{
	return this->equals(str.data(), str.length());
}


bool String::equals(const String &str) const
{
	return this->equals(str.getCharArray(), str.getLength());
}


bool String::equalsIgnoreCase(const char *lhStr, const char *rhStr)
{
	return String::equalsIgnoreCase(std::string(lhStr), std::string(rhStr));
}


bool String::equalsIgnoreCase(const std::string &lhStr, const std::string &rhStr)
{
	std::string lh(lhStr), rh(rhStr);
	String::toLower(lh);
	String::toLower(rh);
	return lh == rh;
}


bool String::equalsIgnoreCase(const char *str)
{
	return String::equalsIgnoreCase(this->inner(), std::string(str));
}


bool String::equalsIgnoreCase(const std::string &str)
{
	return String::equalsIgnoreCase(this->inner(), str);
}


bool String::equalsIgnoreCase(const String &str)
{
	return String::equalsIgnoreCase(this->inner(), str.inner());
}


void String::toUpper(char *str)
{
	String::toUpper(str, ::strlen(str));
}


void String::toUpper(char *str, size_t strLength)
{
	for (size_t i = 0; i < strLength; ++i, ++str)
		*str = (char)std::toupper((unsigned char)*str);
}


void String::toUpper(std::string &str)
{
	for (std::string::iterator i = str.begin(),
		end = str.end(); i != end; ++i)
		*i = (char)std::toupper((unsigned char)*i);
}


void String::toUpper(String &str)
{
	String::toUpper(str.inner());
}


void String::toLower(char *str)
{
	String::toLower(str, ::strlen(str));
}


void String::toLower(char *str, size_t strLength)
{
	for (size_t i = 0; i < strLength; ++i, ++str)
		*str = (char)std::tolower((unsigned char)*str);
}


void String::toLower(std::string &str)
{
	for (std::string::iterator i = str.begin(),
		end = str.end(); i != end; ++i)
	{
		*i = (char)std::tolower((unsigned char)*i);
	}
}


void String::toLower(String &str)
{
	String::toLower(str.inner());
}


String String::toUpper() const
{
	String str(this->getLength());

	for (std::string::const_iterator i = this->stdStr.begin(),
		end = this->stdStr.end(); i != end; ++i)
	{
		str.append((char)std::toupper((unsigned char)*i));
	}

	return str;
}


String String::toLower() const
{
	String str(this->getLength());

	for (std::string::const_iterator i = this->stdStr.begin(),
		end = this->stdStr.end(); i != end; ++i)
	{
		str.append((char)std::tolower((unsigned char)*i));
	}

	return str;
}


std::vector<String> String::split(char separator) const
{
	return this->split(&separator, 1);
}


std::vector<String> String::split(const char *separator) const
{
	return this->split(separator, ::strlen(separator));
}


std::vector<String> String::split(const char *separator, size_t sepLen) const
{
	std::vector<String> splits;
	size_t start = 0;
	size_t end = this->indexOf(separator, sepLen, 0);
	size_t len = this->getLength();

	while (end != ~0u)
	{
		splits.push_back(String(this->getCharArray() + start, end - start));

		start = end + sepLen;

		if (start >= len)
			break;

		end = this->indexOf(separator, sepLen, start);
	}

	if (start < len)
		splits.push_back(String(this->getCharArray() + start, len - start));
	else	// else there is a separator at end of string.
		splits.push_back(String());

	return splits;
}


std::vector<String> String::split(const std::string &separator) const
{
	return this->split(separator.data(), separator.length());
}


std::vector<String> String::split(const String &separator) const
{
	return this->split(separator.getCharArray(), separator.getLength());
}


String String::join(char separator, std::vector<String> strs)
{
	return String::join(&separator, 1, strs);
}


String String::join(const char *separator, std::vector<String> strs)
{
	return String::join(separator, ::strlen(separator), strs);
}


String String::join(const char *separator, size_t sepLength, std::vector<String> strs)
{
	std::vector<String>::const_iterator it = strs.begin(), end = strs.end();
	String tmpStr((*it).inner());
	tmpStr.reserve(strs.size() * ((*it).getLength() + sepLength));

	while (++it != end)
	{
		tmpStr.append(separator, sepLength);
		tmpStr.append((*it).inner());
	}

	return tmpStr;
}


String String::join(const std::string &separator, std::vector<String> strs)
{
	return String::join(separator.data(), separator.length(), strs);
}


String String::join(const String &separator, std::vector<String> strs)
{
	return String::join(separator.inner(), strs);
}


size_t String::extract(const std::string &srcStr, const std::string &startStr)
{
	return String::extract(srcStr, 0, startStr);
}


size_t String::extract(const std::string &srcStr, size_t srcOffset,
	const std::string &startStr)
{
	size_t startAt = srcStr.find(startStr, srcOffset);
	return startAt != std::string::npos ? startAt + startStr.length() : ~0u;
}


std::string String::extract(const std::string &srcStr,
	const std::string &startStr, const std::string &endStr)
{
	return String::extract(srcStr, 0, startStr, endStr);
}


std::string String::extract(const std::string &srcStr, size_t srcOffset,
	const std::string &startStr, const std::string &endStr)
{
	size_t startAt = String::extract(srcStr, srcOffset, startStr);

	if (startAt != ~0u)
	{
		size_t endAt = srcStr.find(endStr, startAt);

		if (endAt != std::string::npos)
			return srcStr.substr(startAt, endAt - startAt);
	}

	return std::string();
};


size_t String::extract(const std::string &startStr)
{
	return String::extract(this->inner(), 0, startStr);
}


size_t String::extract(const String &startStr)
{
	return String::extract(this->inner(), 0, startStr.inner());
}


size_t String::extract(size_t srcOffset, const std::string &startStr)
{
	return String::extract(this->inner(), srcOffset, startStr);
}


size_t String::extract(size_t srcOffset, const String &startStr)
{
	return String::extract(this->inner(), srcOffset, startStr.inner());
}


String String::extract(size_t srcOffset, const std::string &startStr,
	const std::string &endStr)
{
	return String::extract(this->inner(), srcOffset, startStr, endStr);
}


String String::extract(size_t srcOffset, const String &startStr, const String &endStr)
{
	return String::extract(this->inner(), srcOffset, startStr.inner(), endStr.inner());
}


void String::clear()
{
	this->stdStr.clear();
}


void String::reserve(size_t size)
{
	this->stdStr.reserve(size);
}


void String::resize(size_t size)
{
	this->stdStr.resize(size);
}


void String::resize(size_t size, char ch)
{
	this->stdStr.resize(size, ch);
}


String String::format(const char *args, ...)
{
	va_list va;
	va_start(va, args);
	String str = String::format(args, va);
	va_end(va);
	return str;
}


String String::format(const char *args, va_list va)
{
	String str;

	int bufferSize = ::vsnprintf(nullptr, 0, args, va) + 1;
	String::format(str, (size_t)bufferSize, args, va);

	return str;
}


String String::format(size_t bufferSize, const char *args, ...)
{
	String str;
	va_list va;

	va_start(va, args);
	String::format(str, bufferSize, args, va);
	va_end(va);

	return str;
}


void String::format(String &outStr, const char *args, ...)
{
	va_list va;
	va_start(va, args);

	int bufferSize = ::vsnprintf(nullptr, 0, args, va) + 1;
	String::format(outStr, (size_t)bufferSize, args, va);

	va_end(va);
}


void String::format(String &outStr, size_t bufferSize,
	const char *args, ...)
{
	va_list va;
	va_start(va, args);
	String::format(outStr, bufferSize, args, va);
	va_end(va);
}


void String::format(String &outStr, size_t bufferSize,
	const char *args, va_list va)
{
	outStr.clear();
	outStr.resize(bufferSize);
#ifdef _MSC_VER
	int ret = ::vsnprintf_s(&outStr[0], bufferSize, bufferSize, args, va);
#else
	int ret = ::vsnprintf(&outStr[0], bufferSize, args, va);
#endif
	if (ret > 0)
		outStr.resize((size_t)ret);
}


void * String::malloc(size_t size)
{
	void *pNew = ::malloc(size);

	if (pNew == nullptr)
		throw Exception(TRACE, STR_EX_BAD_ALLOCATE);

	return pNew;
}


void * String::realloc(void *oldPointer, size_t oldSize, size_t newSize)
{
	void *pNew = ::malloc(newSize);

	if (pNew == nullptr)
		throw Exception(TRACE, STR_EX_BAD_ALLOCATE);

	::memcpy(pNew, oldPointer, oldSize);

	if (oldPointer != nullptr)
		String::free(oldPointer);

	return pNew;
}


void String::free(void *p)
{
	if (p != nullptr)
		::free(p);
}


String operator + (const char lhCh, const String &rhStr)
{
	String str(rhStr.getLength() + 2);
	str += lhCh;
	str += rhStr;
	return str;
}


String operator + (const char *lhStr, const String &rhStr)
{
	size_t len = ::strlen(lhStr);
	String str(len + rhStr.getLength());
	str.append(lhStr, len);
	str += rhStr;
	return str;
}


String operator + (const std::string &lhStr, const String &rhStr)
{
	String str(lhStr.length() + rhStr.getLength());
	str += lhStr;
	str += rhStr;
	return str;
}


String operator + (const String &lhStr, const String &rhStr)
{
	String str(lhStr.getLength() + rhStr.getLength());
	str += lhStr;
	str += rhStr;
	return str;
}


String operator + (std::string &&lhStr, const String &rhStr)
{
	String str(std::move(lhStr));
	str.append(rhStr);
	return str;
}


String operator + (String &&lhStr, const String &rhStr)
{
	String str(std::move(lhStr));
	str.append(rhStr);
	return str;
}
