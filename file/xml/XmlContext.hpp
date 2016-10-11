#ifndef _XMLCONTEXT_HPP_
#define _XMLCONTEXT_HPP_

#include "XmlLexer.hpp"


class XmlContext
{
public:
	typedef XmlLexer::Tokens Tokens;

	XmlContext() = default;
	XmlContext(const char *path);
	XmlContext(const void *data, size_t length);

	void open(const char *path);
	void openFromMemory(const void *data, size_t length);

	const Token & getToken() const
	{ return this->tokens[this->currentToken]; }

	const Token & getNextToken()
	{ return this->tokens[++this->currentToken]; }

	const Token & getBackToken()
	{ return this->tokens[--this->currentToken]; }

	bool hasToken() const
	{ return this->currentToken < this->tokens.size(); }

	void nextToken() { ++this->currentToken; }
	void backToken() { --this->currentToken; }

	size_t getAvailableToken() const
	{ return this->tokens.size() - this->currentToken; }

private:
	Tokens tokens;
	size_t currentToken;
};

#endif
