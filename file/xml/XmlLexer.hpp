#ifndef _XMLLEXER_HPP_
#define _XMLLEXER_HPP_

#include <vector>
#include <string>


typedef struct CodeLocation
{
	CodeLocation() : line(1), column(1) {}
	CodeLocation(const CodeLocation &c) :
		line(c.line), column(c.column) {}
	size_t line;
	size_t column;
} CodeLocation;


typedef struct Token
{
	Token() {}
	Token(const std::string &value, const CodeLocation &location) :
		value(value), location(location) {}
	std::string value;
	CodeLocation location;
} Token;


class XmlLexer
{
public:
	typedef std::vector<Token> Tokens;

	static Tokens splitTokens(const std::string &content);
	static Tokens splitTokens(const void *content, size_t length);
	static bool isSymbolChar(char c);
	static std::string resolveEscaping(const char *contentStart, size_t length);

private:
	typedef struct TokenRange
	{
		TokenRange(const std::string::const_iterator &start,
			const std::string::const_iterator &end) : start(start), end(end) {}
		const std::string::const_iterator &start, &end;
	} TokenRange;

	static size_t findSymbolLength(const char *symbolStart);
	static size_t findStringLength(const char *stringStart);
	static size_t findContentLength(const char *contentStart);
	static size_t takeSymbol(const char *symbolStart, const CodeLocation &location, Tokens &tokens);
	static size_t takeString(const char *stringStart, const CodeLocation &location, Tokens &tokens);
	static size_t takeContent(const char *contentStart, const CodeLocation &location, Tokens &tokens);
	static size_t takeCdata(const char *cdataStart, const CodeLocation &location, Tokens &tokens);
	static std::string parseUnicode(const char *numberStart, int &parseLength);
	static size_t skipInternalElement(const char *start);
	static size_t skipElement(const char *start, char meet);
};

#endif
