#ifndef _JSONLEXER_HPP_
#define _JSONLEXER_HPP_

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


class JsonLexer
{
public:
	typedef std::vector<Token> Tokens;

	static Tokens splitTokens(const std::string &content);
	static Tokens splitTokens(const void *content, size_t length);

	static std::string resolveEscaping(const char *str, size_t length);
	static std::string resolveEscaping(const std::string &str)
	{ return resolveEscaping(str.data(), str.length()); }

private:
	typedef struct TokenRange
	{
		TokenRange(const std::string::const_iterator &start,
			const std::string::const_iterator &end) : start(start), end(end) {}
		const std::string::const_iterator &start, &end;
	} TokenRange;

	static size_t findWordLength(const char *wordStart);
	static size_t takeString(const char *stringStart,
		const CodeLocation &location, Tokens &tokens);
	static size_t takeWord(const char *wordStart,
		const CodeLocation &location, Tokens &tokens);
};

#endif
