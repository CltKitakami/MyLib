#include <cstdlib>
#include "JsonParser.hpp"
#include "JsonLexer.hpp"
#include "common/Log.hpp"


bool JsonValueAst::parse(JsonContext &context)
{
	const Token &token = context.getToken();

	if (token.value[0] == '\"')
	{
		this->self.createString(token.value.data() + 1, token.value.length() - 2);
		return true;
	}
	else if (token.value[0] == '{')
	{
		this->self.createObject();
		return this->self.getObjectAst()->parse(context);
	}
	else if (token.value == "true" || token.value == "false")
	{
		this->self.createBoolean(token.value[0] == 't');
		return true;
	}
	else if (Convert::isInRange(token.value[0], '0', '9') == true ||
		token.value[0] == '-' || token.value[0] == '+')
	{
		this->self.createNumber(std::atof(token.value.data()));
		return true;
	}
	else if (token.value[0] == '[')
	{
		this->self.createArray();
		return this->self.getArrayAst()->parse(context);
	}
	else if (token.value == "null")
	{
		return true;
	}
	else
	{
		JsonParser::reportError(token.location, "unknown value.");
		return false;
	}
}


bool JsonArrayAst::parse(JsonContext &context)
{
	if (context.hasToken() && context.getToken().value[0] == '[')
	{
		context.nextToken();

		if (context.hasToken() && context.getToken().value[0] != ']')
		{
			while (true)
			{
				this->values.push_back(JsonValueAst());

				if (this->values.back().parse(context) == false)
					return false;

				context.nextToken();

				if (context.hasToken() == true)
				{
					if (context.getToken().value[0] == ',')
					{
						context.nextToken();

						if (context.hasToken() != true)
						{
							JsonParser::reportError(context.getToken().location, "expected \']\'.");
							return false;
						}
					}
					else if (context.getToken().value[0] == ']')
						break;
					else
					{
						JsonParser::reportError(context.getToken().location, "expected \']\'.");
						return false;
					}
				}
				else
				{
					break;
				}
			}
		}

		if (context.hasToken() && context.getToken().value[0] == ']')
			return true;
		else
		{
			if (context.hasToken() == true)
				JsonParser::reportError(context.getToken().location, "expected \']\'.");
			else
				JsonParser::reportError("expected \']\' at end of input.");

			return false;
		}
	}
	else
		return false;
}


bool JsonPairAst::parse(JsonContext &context)
{
	const Token &token = context.getToken();

	if (token.value[0] == '\"')
	{
		this->name = token.value.substr(1, token.value.length() - 2);

		if (context.hasToken() == true &&
			context.getNextToken().value[0] == ':')
		{
			if (context.hasToken() == true)
			{
				context.nextToken();
				return this->value.parse(context);
			}
			else
			{
				JsonParser::reportError("expected value after key at end of input.");
				return false;
			}
		}
		else
		{
			JsonParser::reportError(context.getToken().location, "expected \':\'.");
			return false;
		}

	}
	else
	{
		JsonParser::reportError(token.location,
			"the name should be a string warpped in double quotes.");
		return false;
	}
}


bool JsonObjectAst::parse(JsonContext &context)
{
	if (context.hasToken() && context.getToken().value[0] == '{')
	{
		context.nextToken();

		if (context.hasToken() && context.getToken().value[0] != '}')
		{
			while (true)
			{
				this->pairs.push_back(JsonPairAst());

				if (this->pairs.back().parse(context) == false)
					return false;

				context.nextToken();

				if (context.hasToken() == true)
				{
					if (context.getToken().value[0] == ',')
					{
						context.nextToken();

						if (context.hasToken() != true)
						{
							JsonParser::reportError(context.getToken().location, "expected \'}\'.");
							return false;
						}
					}
					else if (context.getToken().value[0] == '}')
						break;
					else
					{
						JsonParser::reportError(context.getToken().location, "expected \'}\'.");
						return false;
					}
				}
				else
				{
					break;
				}
			}
		}

		if (context.hasToken() && context.getToken().value[0] == '}')
			return true;
		else
		{
			if (context.hasToken() == true)
				JsonParser::reportError(context.getToken().location, "expected \'}\'.");
			else
				JsonParser::reportError("expected \'}\' at end of input.");

			return false;
		}
	}
	else
		return false;
}


bool JsonParser::parse(JsonContext &context)
{
	return this->objects.parse(context);
}


void JsonValueAst::generate(JsonBuilder &builder)
{
	if (this->self.getType() == JsonValue::OBJECT)
	{
		builder.objectStack.push(this->self.getObject());
		this->self.getObjectAst()->generate(builder);
		builder.objectStack.pop();
	}
	else if (this->self.getType() == JsonValue::ARRAY)
	{
		builder.arrayStack.push(this->self.getArray());
		this->self.getArrayAst()->generate(builder);
		builder.arrayStack.pop();
	}

	builder.objectStack.top()->back().setValue(this->self);
}


void JsonPairAst::generate(JsonBuilder &builder)
{
	builder.objectStack.top()->addPair(JsonPair(this->name));
	this->value.generate(builder);
}


void JsonArrayAst::generate(JsonBuilder &builder)
{
	for (std::vector<JsonValueAst>::iterator it = values.begin();
		it != values.end(); ++it)
	{
		it->generate(builder);
		builder.arrayStack.top()->put(it->getValue());
	}
}


void JsonObjectAst::generate(JsonBuilder &builder)
{
	for (std::vector<JsonPairAst>::iterator it = pairs.begin();
		it != pairs.end(); ++it)
	{
		it->generate(builder);
	}
}


JsonObject JsonParser::generate()
{
	JsonBuilder builder;
	builder.objectStack.push(&builder.json);
	this->objects.generate(builder);
	return builder.json;
}


void JsonParser::reportError(const CodeLocation &location, const std::string &message)
{
	LOG << location.line << ':' << location.column <<
		": " << message << '\n';
}


void JsonParser::reportError(const std::string &message)
{
	LOG << message << '\n';
}
