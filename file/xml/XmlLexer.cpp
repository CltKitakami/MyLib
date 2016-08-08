#include <cstring>
#include <cstdint>

#include "XmlLexer.hpp"
#include "common/Log.hpp"
#include "common/Convert.hpp"


// #define XML_LOG(var) LOGT(var)
#define XML_LOG(var)


bool XmlLexer::isSymbolChar(char c)
{
	return Convert::isHexChar(c) == true ||
		c == '_' || c == '-' || c == ':';
}


size_t XmlLexer::findSymbolLength(const char *symbolStart)
{
	const char *symbolEnd = symbolStart + 1;

	while (isSymbolChar(*symbolEnd) == true)
		++symbolEnd;

	return (size_t)(symbolEnd - symbolStart);
}


size_t XmlLexer::findStringLength(const char *stringStart)
{
	const char *stringEnd = stringStart + 1;

	while (*stringEnd != '\0' && *stringEnd != *stringStart)
		++stringEnd;

	return (size_t)(stringEnd - stringStart + 1);
}


size_t XmlLexer::findContentLength(const char *contentStart)
{
	const char *contentEnd = contentStart;

	while (*contentEnd != '\0' && *contentEnd != '<')
		++contentEnd;

	return (size_t)(contentEnd - contentStart);
}


size_t XmlLexer::takeSymbol(const char *symbolStart, const CodeLocation &location, Tokens &tokens)
{
	size_t length = findSymbolLength(symbolStart);
	std::string sym = std::string(symbolStart, length);

	tokens.push_back(Token(sym, location));
	XML_LOG("takeSymbol: " << sym);

	return length;
}


size_t XmlLexer::takeString(const char *stringStart, const CodeLocation &location, Tokens &tokens)
{
	size_t length = findStringLength(stringStart);
	std::string str = std::string(stringStart, length);

	tokens.push_back(Token(str, location));
	XML_LOG("takeString: " << str);

	return length;
}


size_t XmlLexer::takeContent(const char *contentStart, const CodeLocation &location, Tokens &tokens)
{
	const char *trimdBegin = contentStart;
	while (*trimdBegin != 0 && (unsigned char)*trimdBegin <= (unsigned char)' ')	// trim begin
		++trimdBegin;

	size_t length = findContentLength(trimdBegin);

	if (length != 0)
	{
		while (--length > 0 && (unsigned char)trimdBegin[length] <= (unsigned char)' ');	// trim end

		length += 1;
		std::string content = std::string(trimdBegin, length);
		tokens.push_back(Token(content, location));
		XML_LOG("takeContent: " << content);
	}

	return (size_t)(trimdBegin + length - contentStart);
}


size_t XmlLexer::takeCdata(const char *cdataStart, const CodeLocation &location, Tokens &tokens)
{
	const char *p = cdataStart;

	if (p[0] == 0 || p[1] == 0 || p[2] == 0)
		return 0;

	while (p[3] != 0)
	{
		if (p[0] == ']' && p[1] == ']' && p[2] == '>')
			break;

		p += 1;
	}

	size_t length = (size_t)(p - cdataStart);
	std::string cdata = std::string(cdataStart, length);

	tokens.push_back(Token(cdata, location));
	XML_LOG("takeCdata: " << cdata);

	return length;
}


std::string XmlLexer::parseUnicode(const char *numberStart, int &parseLength)
{
	uint32_t uni = 0, base;
	const char *p = numberStart;
	parseLength = 0;

	if (*p == 'x')
		base = 16, p += 1;
	else
		base = 10;

	while (*p != ';')
	{
		uint8_t hexBase = Convert::getHexBase((uint8_t)*p);

		if (hexBase == 0)
			return std::string();
		else
			uni = uni * base + (uint32_t)((uint8_t)*p - hexBase);

		p += 1;
	}

	if (uni == 0)
		return std::string();

	parseLength = p - numberStart + 1 + 1;	// (end - begin + 1) + strlen(";")
	std::string uniBytes;
	uniBytes.reserve(4);

	while (uni > 0)
	{
		uniBytes.push_back(char(uni & 0xff));
		uni >>= 8;
	}

	return uniBytes;
}


std::string XmlLexer::resolveEscaping(const char *contentStart, size_t length)
{
	const char *contentEnd = contentStart + length, *p = contentStart;
	std::string result;
	result.reserve(length);

	while (p < contentEnd)
	{
		if (*p != '&')
		{
			result.push_back(*p);
			p += 1;
		}
		else if (p[1] == '#')
		{
			int parseLength = 0;
			std::string uniBytes = parseUnicode(p + 2, parseLength);

			if (parseLength != 0)
			{
				result.append(uniBytes);
				p += parseLength + 1;
			}
			else
			{
				result.push_back('&');
				p += 1;
			}
		}
		else
		{
			bool isFail = false;

			switch (p[1])
			{
				case 'l':
					if (p[2] == 't' && p[3] == ';')
						result.push_back('<'), p += 4;
					else
						isFail = true;
					break;

				case 'g':
					if (p[2] == 't' && p[3] == ';')
						result.push_back('>'), p += 4;
					else
						isFail = true;
					break;

				case 'a':
					if (p[2] == 'm' && p[3] == 'p' && p[4] == ';')
						result.push_back('&'), p += 5;
					else if (p[2] == 'p' && p[3] == 'o' && p[4] == 's' && p[5] == ';')
						result.push_back('\''), p += 6;
					else
						isFail = true;
					break;

				case 'q':
					if (p[2] == 'u' && p[3] == 'o' && p[4] == 't' && p[5] == ';')
						result.push_back('\"'), p += 6;
					else
						isFail = true;
					break;

				default:
					isFail = true;
					break;
			}

			if (isFail == true)
			{
				result.push_back('&');
				p += 1;
			}
		}
	}

	return result;
}


size_t XmlLexer::skipInternalElement(const char *start)
{
	const char *end = start;

	while (*end != '\0' && (*end != '?' || *(end + 1) != '>'))
		++end;

	XML_LOG("skipInternalElement: " << std::string(start, end - start + 2));

	return *end != '\0' ? (size_t)(end - start + 2) : 0;
}


size_t XmlLexer::skipElement(const char *start, char meet)
{
	const char *end = start;

	while (*end != '\0' &&
		(*end != meet || *(end + 1) != meet || *(end + 2) != '>'))
		++end;

	XML_LOG("skipElement: " << std::string(start, end - start + 3));

	return *end != '\0' ? (size_t)(end - start + 3) : 0;
}


XmlLexer::Tokens XmlLexer::splitTokens(const std::string &content)
{
	return splitTokens(content.data(), content.length());
}


XmlLexer::Tokens XmlLexer::splitTokens(const void *content, size_t length)
{
	const char *p = reinterpret_cast<const char *>(content);
	const char *end = p + length;
	Tokens tokens;
	CodeLocation location;

	while (p < end)
	{
		if (isSymbolChar(*p) == true)
		{
			size_t symbolLength = takeSymbol(p, location, tokens);
			p += symbolLength;
			location.column += symbolLength;
		}
		else if (*p == '<')
		{
			size_t skipLength = 0;

			if (*(p + 1) == '?')
				skipLength = skipInternalElement(p);
			else if (*(p + 1) == '!')
			{
				if (*(p + 2) == '-')
					skipLength = skipElement(p, '-');
				else if (::strncmp(p + 2, "[CDATA[", 7) == 0)
				{
					tokens.push_back(Token(std::string("<![CDATA["), location));
					location.column += 9;
					size_t cdataLength = takeCdata(p + 9, location, tokens);
					tokens.push_back(Token(std::string("]]>"), location));
					skipLength = cdataLength + 9 + 3;
				}
				else
				{
					const char *tagEnd = strchr(p + 2, (int)'>');

					if (tagEnd != nullptr)
						skipLength = size_t(tagEnd - p) + 1;
				}
			}

			if (skipLength == 0)
			{
				skipLength = 1;
				tokens.push_back(Token(std::string(1, '<'), location));
				XML_LOG("take: <");
			}

			p += skipLength;
			location.column += skipLength;
		}
		else if (*p == '>')
		{
			tokens.push_back(Token(std::string(1, '>'), location));
			XML_LOG("take: >");
			size_t contentLength = takeContent(p + 1, location, tokens);
			p += contentLength + 1;
			location.column += contentLength + 1;
		}
		else if (*p == '\"' || *p == '\'')
		{
			size_t stringLength = takeString(p, location, tokens);
			p += stringLength;
			location.column += stringLength;
		}
		else if (*p == ' ')
		{
			XML_LOG("skip space \' \'");
			p += 1;
			location.column += 1;
		}
		else if (*p == '\t')
		{
			XML_LOG("skip TAB \'\\t\'");
			p += 1;
			location.column += 4;
		}
		else if (*p == '\n')
		{
			XML_LOG("skip LF \'\\n\'");
			p += 1;
			location.line += 1;
			location.column = 1;
		}
		else if (*p == '\r')
		{
			XML_LOG("skip CR \'\\r\'");
			p += 1;
		}
		// else if (isDelimiter(*p) == true)
		else
		{
			XML_LOG("take unknow: (0x" << std::hex << (unsigned int)(unsigned char)*p << std::dec
				<< ")\'" << *p << '\'');
			tokens.push_back(Token(std::string(1, *p), location));
			p += 1;
			location.column += 1;
		}
	}

	return tokens;
}
