#include "JsonLexer.hpp"
#include "common/Log.hpp"
#include "common/Convert.hpp"


// #define JSON_LOG(var) LOGT(var)
#define JSON_LOG(var)


std::string JsonLexer::resolveEscaping(const char *str, size_t length)
{
	std::string buffer;
	buffer.reserve(length * 2);

	for (const char *s = str; s != str + length; ++s)
	{
		switch (*s)
		{
			case '\"': case '\\': case '/':
				buffer.push_back('\\');
				buffer.push_back(*s);
				break;
			case '\n': buffer.append("\\n"); break;
			case '\r': buffer.append("\\r"); break;
			case '\t': buffer.append("\\t"); break;
			// case '\x08': buffer.append("\\b"); break;
			// case '\x0c': buffer.append("\\f"); break;
			default:
				buffer.push_back(*s);
				break;
		}
	}

	return buffer;
}


size_t JsonLexer::findWordLength(const char *wordStart)
{
	const char *w = wordStart;

	while (*w != '\0' && (Convert::isHexChar(*w) == true ||
		*w == '_' || *w == '.' || *w == '-' || *w == '+'))
	{
		++w;
	}

	return (size_t)(w - wordStart);
}


size_t JsonLexer::takeString(const char *stringStart, const CodeLocation &location, Tokens &tokens)
{
	const char *stringEnd = stringStart + 1;
	std::string str;
	str.reserve(16);
	str.push_back('\"');

	while (*stringEnd != '\0' && *stringEnd != '\"')
	{
		if (*stringEnd == '\\')
		{
			switch (*(stringEnd + 1))
			{
				case '\"': case '\\': case '/':
					str.push_back(*(stringEnd + 1)); break;
				case 'n': str.push_back('\n'); break;
				case 'r': str.push_back('\r'); break;
				case 't': str.push_back('\t'); break;
				// case 'b': str.push_back('\x08'); break;
				// case 'f': str.push_back('\x0c'); break;
				default:
					str.push_back('\\');
					str.push_back(*(stringEnd + 1));
					break;
			}

			stringEnd += 2;
		}
		else
		{
			str.push_back(*stringEnd);
			++stringEnd;
		}
	}

	str.push_back('\"');
	tokens.push_back(Token(str, location));
	JSON_LOG("takeString: " << str);

	return (size_t)(stringEnd - stringStart + 1);
}


size_t JsonLexer::takeWord(const char *wordStart, const CodeLocation &location, Tokens &tokens)
{
	size_t length = findWordLength(wordStart);

	if (length != 0)
	{
		std::string takeWordStr = std::string(wordStart, length);
		tokens.push_back(Token(takeWordStr, location));
		JSON_LOG("takeWord: " << takeWordStr);
		return length;
	}
	else
	{
		tokens.push_back(Token(std::string(1, *wordStart), location));
		JSON_LOG(Log::format("take other: (0x%x)\'%c\'",
			(uint32_t)(uint8_t)*wordStart, *wordStart));
		return 1;
	}
}


JsonLexer::Tokens JsonLexer::splitTokens(const std::string &content)
{
	return splitTokens(content.data(), content.length());
}


JsonLexer::Tokens JsonLexer::splitTokens(const void *content, size_t length)
{
	const char *p = reinterpret_cast<const char *>(content);
	const char *end = p + length;
	Tokens tokens;
	CodeLocation location;

	while (p < end)
	{
		if (*p == '\"')
		{
			size_t stringLength = takeString(p, location, tokens);
			p += stringLength;
			location.column += stringLength;
		}
		else if (*p == ' ')
		{
			JSON_LOG("skip space \' \'");
			p += 1;
			location.column += 1;
		}
		else if (*p == '\t')
		{
			JSON_LOG("skip TAB \'\\t\'");
			p += 1;
			location.column += 4;
		}
		else if (*p == '\n')
		{
			JSON_LOG("skip LF \'\\n\'");
			p += 1;
			location.line += 1;
			location.column = 1;
		}
		else if (*p == '\r')
		{
			JSON_LOG("skip CR \'\\r\'");
			p += 1;
		}
		else
		{
			size_t wordLength = takeWord(p, location, tokens);
			p += wordLength;
			location.column += wordLength;
		}
	}

	return tokens;
}
