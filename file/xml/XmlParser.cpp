#include "XmlParser.hpp"
#include "XmlLexer.hpp"
#include "common/Log.hpp"

/*
{XML} := {ELEMENT_LIST}
{ELEMENT_LIST} := {TAB_BEGIN}*
{TAB_BEGIN} := <{TAG_NAME} {ATTRIBUTE_LIST} | < !-- {COMMENT} | <? {INTERNAL} | < ![CDATA[ {CDATA}
{ATTRIBUTE_LIST} := {ATTRIBUTE}* {TAG_END}
{ATTRIBUTE} := {KEY}="{VALUE}"
{TAG_END} := >{TAG_VALUE}{TAG_NAME}> | />
{TAG_VALUE} := {STRING} | {ELEMENT_LIST} | </
{COMMENT} := {} -->
{INTERNAL} := {} ?>
{CDATA} := {} ]]>

<layout name="main">
	<!-- comment -->
	<widget name="button" x="0" y="0" width="0" height="0"
		event="onMousePress:foo"/>
</layout>
*/


bool XmlTagValueAst::parse(XmlContext &context)
{
	if (context.hasToken() == true)
	{
		const Token &token = context.getNextToken();

		if (token.value[0] != '<')
			this->value = token.value;
		else if (token.value == "<![CDATA[" && context.hasToken() == true)
		{
			if (this->elements.parse(context) == false)
				return false;
			else
				return true;
		}
		else
			context.backToken();
	}

	if (context.hasToken() == true)
	{
		const Token &token = context.getNextToken();

		if (token.value == "<" && context.hasToken() == true)
		{
			const Token &token = context.getNextToken();

			if (token.value == "/")
				return true;
			else
			{
				context.backToken();
				return this->elements.parse(context);
			}
		}
		else
		{
			XmlParser::reportError(token.location, "near \"" + this->value +
				"\" \"" + token.value + "\" expected \'</TagName>\'.");
			return false;
		}
	}
	else
	{
		XmlParser::reportError("expected \'</TagName>\' at end of input.");
		return false;
	}
}


bool XmlTagBeginAst::parse(XmlContext &context)
{
	if (context.hasToken() == true)
	{
		const Token &token = context.getToken();

		if (token.value == "<" && context.hasToken() == true)
		{
			const Token &token = context.getNextToken();

			if (XmlLexer::isSymbolChar(token.value[0]) == true)
			{
				this->name = token.value;
				context.nextToken();
				return this->attributes.parse(context);
			}
			else
			{
				XmlParser::reportError(token.location, "expected \'TagName>\'.");
				return false;
			}
		}
		else if (token.value == "<![CDATA[" && context.hasToken() == true)
		{
			{	// take cdata value
				const Token &token = context.getNextToken();

				if (token.value != "]]>")
					this->cdata = token.value;
				else
					context.backToken();
			}
			{	// take cdata end symbol
				const Token &token = context.getNextToken();

				if (token.value == "]]>")
					return true;
				else
				{
					XmlParser::reportError(token.location, "expected \']]>\'.");
					return false;
				}
			}
		}
		else
		{
			XmlParser::reportError(token.location, "expected \'<TagName>\'.");
			return false;
		}
	}
	else
		return false;
}


bool XmlTagEndAst::parse(XmlContext &context)
{
	if (context.hasToken() == true)
	{
		const Token &token = context.getToken();

		if (token.value == ">")
		{
			if (this->value.parse(context) == true)
			{
				if (context.hasToken() == true)
				{
					const Token &token = context.getNextToken();

					if (isalnum(token.value[0] & 0xff) != 0)
					{
						this->name = token.value;

						if (context.hasToken() == true &&
							context.getNextToken().value == ">")
							return true;
						else
						{
							XmlParser::reportError(token.location,
								"expected \'>\' after \'</\'" + this->name + ".");
							return false;
						}
					}
					else
					{
						XmlParser::reportError(token.location,
							"expected \'TagName>\' after \'</\'.");
						return false;
					}
				}
				else
				{
					XmlParser::reportError("expected \'TagName>\' after \'</\'.");
					return false;
				}
			}
			else
				return false;
		}
		else if (token.value == "/")
		{
			if (context.hasToken() == true && context.getNextToken().value == ">")
			{
				this->isEmptyTag = true;
				return true;
			}
			else
			{
				XmlParser::reportError(token.location, "expected \'>\'.");
				return false;
			}
		}
		else
		{
			XmlParser::reportError(token.location, "expected \'>\'.");
			return false;
		}
	}
	else
	{
		XmlParser::reportError("expected \'/>\' at end of input.");
		return false;
	}
}


bool XmlAttributeAst::parse(XmlContext &context)
{
	size_t available = context.getAvailableToken();

	if (available >= 3)
	{
		const Token &key = context.getToken();
		const Token &equal = context.getNextToken();
		const Token &value = context.getNextToken();

		if (equal.value != "=")
		{
			XmlParser::reportError(equal.location, "expected \'=\'.");
			return false;
		}


		if (value.value.length() < 2 ||
			value.value.front() != value.value.back() ||
			(value.value.front() != '\"' && value.value.front() != '\''))
		{
			XmlParser::reportError(value.location, "expected \'\"\'.");
			return false;
		}

		this->key = key.value;
		this->value = value.value.substr(1, value.value.length() - 2);

		return true;
	}
	else
	{
		if (available > 1)
			XmlParser::reportError(context.getNextToken().location, "expected attribute at end of input.");
		else
			XmlParser::reportError("expected attribute at end of input.");

		return false;
	}
}


bool XmlAttributeListAst::parse(XmlContext &context)
{
	while (context.hasToken() == true)
	{
		const Token &token = context.getToken();

		if (token.value[0] == '>' || token.value[0] == '/')
			return this->tagEnd.parse(context);
		else
		{
			this->list.push_back(XmlAttributeAst());

			if (this->list.back().parse(context) == false)
				return false;
		}

		context.nextToken();
	}

	XmlParser::reportError("expected \'/>\' at end of input.");

	return false;
}


XmlElementListAst::~XmlElementListAst()
{
	for (std::vector<XmlTagBeginAst *>::iterator it = elements.begin();
		it != elements.end(); ++it)
	{
		delete *it;
	}
}


bool XmlElementListAst::parse(XmlContext &context)
{
	while (context.hasToken() == true)
	{
		XmlTagBeginAst *XmltagBeginAst = new XmlTagBeginAst();
		this->elements.push_back(XmltagBeginAst);

		if (XmltagBeginAst->parse(context) == false)
			return false;

		context.nextToken();

		if (context.getAvailableToken() >= 2)
		{
			if (context.getToken().value == "<" &&
				context.getNextToken().value == "/")
				return true;
			else
				context.backToken();
		}
	}

	return true;
}


void XmlElementListAst::generate(XmlBuilder &builder)
{
	for (std::vector<XmlTagBeginAst *>::iterator it = elements.begin();
		it != elements.end(); ++it)
	{
		(*it)->generate(builder);
	}
}


void XmlTagBeginAst::generate(XmlBuilder &builder)
{
	if (this->isCdata() == false)
	{
		builder.stack.back()->addElement(XmlElement(this->name));
		this->attributes.generate(builder);
	}
	else
	{
		XmlElement e;
		e.setCdata(this->cdata);
		builder.stack.back()->addElement(e);
	}
}


void XmlTagEndAst::generate(XmlBuilder &builder)
{
	if (this->isEmptyTag == true ||
		builder.stack.back()->elementsBack().equals(this->name) == true)
		this->value.generate(builder);
	else
		XmlParser::reportError("TagName miss: " +
			builder.stack.back()->elementsBack().getName() + " != " + this->name);
}


void XmlTagValueAst::generate(XmlBuilder &builder)
{
	XmlElement &e = builder.stack.back()->elementsBack();
	e.setValue(XmlLexer::resolveEscaping(this->value.data(), this->value.size()));
	builder.stack.push_back(&e);
	this->elements.generate(builder);
	builder.stack.pop_back();
}


void XmlAttributeListAst::generate(XmlBuilder &builder)
{
	for (Attributes::iterator it = list.begin(); it != list.end(); ++it)
		it->generate(builder);
	this->tagEnd.generate(builder);
}


void XmlAttributeAst::generate(XmlBuilder &builder)
{
	builder.stack.back()->elementsBack().addAttribute(XmlAttribute(this->key, this->value));
}


bool XmlParser::parse(XmlContext &context)
{
	return this->elementList.parse(context);
}


XmlElement XmlParser::generate()
{
	XmlBuilder builder;
	builder.stack.push_back(&builder.xml);
	this->elementList.generate(builder);
	return builder.xml;
}


void XmlParser::reportError(const CodeLocation &location, const std::string &message)
{
	LOG << location.line << ':' << location.column <<
		": " << message << '\n';
}


void XmlParser::reportError(const std::string &message)
{
	LOG << message << '\n';
}
