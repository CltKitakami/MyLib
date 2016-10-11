#ifndef _JSONCONTEXT_HPP_
#define _JSONCONTEXT_HPP_

#include "JsonLexer.hpp"


class JsonContext
{
public:
	typedef JsonLexer::Tokens Tokens;

	JsonContext() = default;
	JsonContext(const char *path);
	JsonContext(const void *data, size_t length);

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
