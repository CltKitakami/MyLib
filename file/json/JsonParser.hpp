#ifndef _JSONPARSER_HPP_
#define _JSONPARSER_HPP_

#include <stack>
#include "JsonContext.hpp"
#include "../Json.hpp"


typedef struct JsonBuilder
{
	std::stack<JsonObject *> objectStack;
	std::stack<JsonArray *> arrayStack;
	JsonObject json;
} JsonBuilder;


class JsonExprAst
{
public:
	virtual bool parse(JsonContext &context) = 0;
	virtual void generate(JsonBuilder &builder) = 0;
	virtual ~JsonExprAst() {}
};


class JsonValueAst : public JsonExprAst
{
	JsonValue self;
public:
	const JsonValue & getValue() const { return self; }
	virtual bool parse(JsonContext &context);
	virtual void generate(JsonBuilder &builder);
};


class JsonArrayAst : public JsonExprAst
{
	std::vector<JsonValueAst> values;
public:
	virtual bool parse(JsonContext &context);
	virtual void generate(JsonBuilder &builder);
};


class JsonPairAst : public JsonExprAst
{
	std::string name;
	JsonValueAst value;
public:
	virtual bool parse(JsonContext &context);
	virtual void generate(JsonBuilder &builder);
};


class JsonObjectAst : public JsonExprAst
{
	std::vector<JsonPairAst> pairs;
public:
	virtual bool parse(JsonContext &context);
	virtual void generate(JsonBuilder &builder);
};


class JsonParser
{
	JsonObjectAst objects;
public:
	virtual ~JsonParser() {}
	virtual bool parse(JsonContext &context);
	virtual JsonObject generate();

	static void reportError(const CodeLocation &location, const std::string &message);
	static void reportError(const std::string &message);
};

#endif
