#ifndef _XMLCONTEXT_HPP_
#define _XMLCONTEXT_HPP_

#include "XmlLexer.hpp"


class XmlContext
{
public:
	typedef XmlLexer::Tokens Tokens;

	XmlContext() {}
	XmlContext(const char *path) { open(path); }
	XmlContext(const void *data, size_t length) { openFromMemory(data, length); }

	void open(const char *path);
	void openFromMemory(const void *data, size_t length);

	inline const Token & getToken() const { return this->tokens[this->currentToken]; }
	inline const Token & getNextToken() { return this->tokens[++this->currentToken]; }
	inline const Token & getBackToken() { return this->tokens[--this->currentToken]; }
	inline bool hasToken() const { return this->currentToken < this->tokens.size(); }
	inline void nextToken() { ++this->currentToken; }
	inline void backToken() { --this->currentToken; }
	inline size_t getAvailableToken() const { return this->tokens.size() - this->currentToken;}

private:
	Tokens tokens;
	size_t currentToken;
};

#endif
