#include "Json.hpp"
#include "json/JsonParser.hpp"
#include "json/JsonLexer.hpp"
#include "json/JsonContext.hpp"
#include "file/File.hpp"


std::string JsonValue::typeToString(JsonValue::Type type)
{
	switch (type)
	{
		case STRING: return "string";
		case NUMBER: return "number";
		case OBJECT: return "object";
		case ARRAY: return "array";
		case BOOLEAN: return "boolean";
		default: return "nullptr";
	}
}


void JsonValue::assertType(JsonValue::Type type) const
{
	if (this->type != type)
	{
		Exception ex(TRACE, "can't convert ");
		ex << typeToString(this->type) << " to ";
		ex << typeToString(type);
		throw ex;
	}
}


JsonValue JsonValue::create(const char *str, size_t length)
{
	JsonValue v;
	v.createString(str, length);
	return v;
}


JsonValue JsonValue::create(bool value)
{
	JsonValue v;
	v.createBoolean(value);
	return v;
}


JsonValue JsonValue::create(double value)
{
	JsonValue v;
	v.createNumber(value);
	return v;
}


void JsonValue::createString(const char *str, size_t length)
{
	if (this->value == nullptr)
	{
		this->value = new std::string(str, length);
		this->type = STRING;
	}
}


void JsonValue::createBoolean(bool value)
{
	if (this->value == nullptr)
	{
		this->value = value ? (void *)1 : nullptr;
		this->type = BOOLEAN;
	}
}


void JsonValue::createNumber(double value)
{
	if (this->value == nullptr)
	{
		this->value = new double(value);
		this->type = NUMBER;
	}
}


void JsonValue::createArray()
{
	if (this->value == nullptr)
	{
		this->value = new Array;
		((Array *)this->value)->arr = new JsonArray;
		((Array *)this->value)->ast = new JsonArrayAst;
		this->type = ARRAY;
	}
}


void JsonValue::createObject()
{
	if (this->value == nullptr)
	{
		this->value = new Object;
		((Object *)this->value)->obj = new JsonObject;
		((Object *)this->value)->ast = new JsonObjectAst;
		this->type = OBJECT;
	}
}


void JsonValue::release()
{
	if (--(*this->refCount) == 0)
	{
		delete this->refCount;

		switch (this->type)
		{
			case STRING:
				delete (std::string *)this->value;
				break;
			case NUMBER:
				delete (double *)this->value;
				break;
			case OBJECT:
				delete ((Object *)this->value)->obj;
				delete ((Object *)this->value)->ast;
				delete (Object *)this->value;
				break;
			case ARRAY:
				delete ((Array *)this->value)->arr;
				delete ((Array *)this->value)->ast;
				delete (Array *)this->value;
				break;
			default: break;
		}
	}
}


std::string JsonValue::toString() const
{
	switch (type)
	{
		case STRING: return *(std::string *)this->value;
		case BOOLEAN: return this->value == nullptr ? "false" : "true";
		case NUMBER:
#ifdef _MSC_VER
			return std::to_string((long double)*(double *)this->value);
#else
			return std::to_string(*(double *)this->value);
#endif
		case OBJECT: return "{object}";
		case ARRAY: return "{array}";
		default: return "null";
	}
}


void JsonValue::assign(const JsonValue &v)
{
	if (this != &v)
	{
		this->release();
		this->copy(v);
	}
}


void JsonValue::copy(const JsonValue &v)
{
	this->value = v.value;
	this->refCount = v.refCount;
	this->type = v.type;
	++(*this->refCount);
}


JsonObject::PairMap JsonObject::buildMap()
{
	PairMap map;

	for (Pairs::iterator it = this->begin(); it != this->end(); ++it)
		map[it->getName()] = it->getReferenceValue();

	return map;
}


void JsonObject::removePair(const std::string &name)
{
	for (Pairs::iterator it = pairs.begin(); it != pairs.end(); ++it)
	{
		if ((*it).equals(name) == true)
		{
			this->pairs.erase(it);
			break;
		}
	}
}


void Json::newLineToString(std::string &buffer, int tabDepth)
{
	buffer.push_back('\n');
	buffer.append((size_t)tabDepth, '\t');
}


void Json::appendSpace(std::string &buffer)
{
	if (buffer.back() != '{' && buffer.back() != '[')
		buffer.push_back(' ');
}


void Json::valueToString(std::string &buffer, const JsonValue &value, int tabDepth)
{
	JsonValue::Type type = value.getType();

	if (type == JsonValue::OBJECT)
	{
		Json::newLineToString(buffer, tabDepth);
		Json::objectToString(buffer, *value.getObject(), tabDepth);
		buffer.push_back(',');
	}
	else if (type == JsonValue::STRING)
	{
		appendSpace(buffer);
		buffer.push_back('\"');
		buffer.append(JsonLexer::resolveEscaping(value.toString()));
		buffer.append("\",");
	}
	else if (type == JsonValue::BOOLEAN)
	{
		appendSpace(buffer);

		if (value.getBoolean() == true)
			buffer.append("true,");
		else
			buffer.append("false,");
	}
	else if (type == JsonValue::NUMBER)
	{
		appendSpace(buffer);
#ifdef _MSC_VER
		buffer.append(std::to_string((long double)value.getNumber()));
#else
		buffer.append(std::to_string(value.getNumber()));
#endif
		buffer.push_back(',');
	}
	else if (type == JsonValue::ARRAY)
	{
		arrayToString(buffer, *value.getArray(), tabDepth);
		buffer.push_back(',');
	}
	else if (type == JsonValue::NULL_VALUE)
	{
		appendSpace(buffer);
		buffer.append("null,");
	}
}


void Json::arrayToString(std::string &buffer, const JsonArray &array, int tabDepth)
{
	Json::newLineToString(buffer, tabDepth);
	buffer.push_back('[');
	tabDepth += 1;
	Json::newLineToString(buffer, tabDepth);

	for (int i = 0; i < (int)array.getSize(); ++i)
		Json::valueToString(buffer, array[i], tabDepth);

	if (buffer.back() == ',')
		buffer.pop_back();

	Json::newLineToString(buffer, tabDepth - 1);
	buffer.push_back(']');
}


void Json::objectToString(std::string &buffer, const JsonObject &object, int tabDepth)
{
	buffer.push_back('{');
	tabDepth += 1;

	for (Pairs::const_iterator it = object.begin(); it != object.end(); ++it)
	{
		Json::newLineToString(buffer, tabDepth);
		buffer.push_back('\"');
		buffer.append(it->getName());
		buffer.append("\":");
		Json::valueToString(buffer, it->getValue(), tabDepth);
	}

	if (buffer.back() == ',')
		buffer.pop_back();

	Json::newLineToString(buffer, tabDepth - 1);
	buffer.push_back('}');
}


bool Json::open(const char *fileName)
{
	JsonContext c(fileName);
	JsonParser p;

	if (p.parse(c) == true)
	{
		this->object = p.generate();
		return true;
	}
	else
	{
		return false;
	}
}


bool Json::openFromMemory(const void *data, size_t length)
{
	JsonContext c(data, length);
	JsonParser p;

	if (p.parse(c) == true)
	{
		this->object = p.generate();
		return true;
	}
	else
	{
		return false;
	}
}


void Json::save(const char *fileName)
{
	File(fileName, File::WRITE) << this->toString();
}


std::string Json::toString() const
{
	std::string buffer;
	Json::objectToString(buffer, this->object, 0);
	return buffer;
}
