#ifndef _XMLPARSER_HPP_
#define _XMLPARSER_HPP_

#include "XmlContext.hpp"
#include "../Xml.hpp"


typedef struct XmlBuilder
{
	std::vector<XmlElement *> stack;
	XmlElement xml;
} XmlBuilder;


class XmlExprAst
{
public:
	virtual bool parse(XmlContext &context) = 0;
	virtual void generate(XmlBuilder &builder) = 0;
	virtual ~XmlExprAst() {}
};


class XmlTagBeginAst;


class XmlElementListAst : public XmlExprAst
{
	std::vector<XmlTagBeginAst *> elements;
	XmlElementListAst(const XmlElementListAst &);
	XmlElementListAst & operator = (const XmlElementListAst &);
public:
	XmlElementListAst() {}
	virtual ~XmlElementListAst();
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);
};


class XmlTagValueAst : public XmlExprAst
{
	std::string value;
	XmlElementListAst elements;
public:
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);
	inline std::string get() { return value; }
};


class XmlTagEndAst : public XmlExprAst
{
	XmlTagValueAst value;
	std::string name;
	bool isEmptyTag;
public:
	XmlTagEndAst() : isEmptyTag(false) {}
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);
};


class XmlAttributeAst : public XmlExprAst
{
	std::string key, value;
public:
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);
};


class XmlAttributeListAst : public XmlExprAst
{
public:
	typedef std::vector<XmlAttributeAst> Attributes;
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);

	Attributes::iterator begin() { return this->list.begin(); }
	Attributes::iterator end() { return this->list.end(); }
	Attributes::const_iterator begin() const { return this->list.begin(); }
	Attributes::const_iterator end() const { return this->list.end(); }

private:
	Attributes list;
	XmlTagEndAst tagEnd;
};


class XmlTagBeginAst : public XmlExprAst
{
	XmlAttributeListAst attributes;
	std::string cdata;
	std::string name;
public:
	virtual bool parse(XmlContext &context);
	virtual void generate(XmlBuilder &builder);
	inline bool isCdata() const { return this->cdata.size() != 0; }
};


class XmlParser
{
	XmlElementListAst elementList;
public:
	virtual ~XmlParser() {}
	virtual bool parse(XmlContext &context);
	virtual XmlElement generate();

	static void reportError(const CodeLocation &location, const std::string &message);
	static void reportError(const std::string &message);
};

#endif
