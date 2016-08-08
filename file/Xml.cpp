#include "Xml.hpp"
#include "xml/XmlParser.hpp"
#include "xml/XmlContext.hpp"
#include "xml/XmlLexer.hpp"
#include "file/File.hpp"


std::string XmlElement::getAttribute(const std::string &key)
{
	for (Attributes::iterator it = attributesBegin(); it != attributesEnd(); ++it)
	{
		if ((*it).equals(key) == true)
			return it->getValue();
	}

	return std::string();
}


void XmlElement::removeAttribute(const std::string &key)
{
	for (Attributes::iterator it = attributesBegin(); it != attributesEnd(); ++it)
	{
		if ((*it).equals(key) == true)
		{
			this->attributes.erase(it);
			break;
		}
	}
}


XmlElementMap XmlElement::buildElementMap()
{
	XmlElementMap map;

	for (Elements::iterator it = elementsBegin(); it != elementsEnd(); ++it)
		map[it->getName()] = &*it;

	return map;
}


XmlAttributeMap XmlElement::buildAttributeMap()
{
	XmlAttributeMap map;

	for (Attributes::iterator it = attributesBegin(); it != attributesEnd(); ++it)
		map[it->getKey()] = &*it;

	return map;
}


void XmlElement::removeElement(const std::string &name)
{
	for (Elements::iterator it = elementsBegin(); it != elementsEnd(); ++it)
	{
		if ((*it).equals(name) == true)
		{
			this->elements.erase(it);
			break;
		}
	}
}


void Xml::newLineToString(std::string &buffer, int tabDepth)
{
	buffer.push_back('\n');
	buffer.append((size_t)tabDepth, '\t');
}


void Xml::attributeToString(std::string &buffer, const XmlElement &elements)
{
	for (Attributes::const_iterator it = elements.attributesBegin();
		it != elements.attributesEnd(); ++it)
	{
		buffer.push_back(' ');
		buffer.append(it->getKey());
		buffer.append("=\"");
		buffer.append(it->getValue());
		buffer.push_back('\"');
	}
}


void Xml::elementToString(std::string &buffer, const XmlElement &elements, int tabDepth)
{
	for (Elements::const_iterator it = elements.elementsBegin();
		it != elements.elementsEnd(); ++it)
	{
		buffer.push_back('<');
		buffer.append(it->getName());
		Xml::attributeToString(buffer, *it);

		if (it->isCdata() == true)
		{
			buffer.append("<![CDATA[");
			buffer.append(it->getCdata());
			buffer.append("]]></");
			buffer.append(it->getName());
			buffer.push_back('>');
			Xml::newLineToString(buffer, tabDepth);
		}
		else if (it->getElementsSize() == 0 && it->hasValue() == false)
		{
			buffer.append("/>");
			Xml::newLineToString(buffer, tabDepth);
		}
		else
		{
			buffer.push_back('>');
			buffer.append(it->getValue());

			if (it->getElementsSize() != 0)
			{
				Xml::newLineToString(buffer, tabDepth + 1);
				Xml::elementToString(buffer, *it, tabDepth + 1);
			}

			buffer.append("</");
			buffer.append(it->getName());
			buffer.push_back('>');
			Xml::newLineToString(buffer, tabDepth);
		}
	}

	if (buffer.back() == '\t')
		buffer.pop_back();
}


bool Xml::open(const char *fileName)
{
	XmlContext c(fileName);
	XmlParser p;

	if (p.parse(c) == true)
	{
		this->element = p.generate();
		return true;
	}
	else
	{
		return false;
	}
}


bool Xml::openFromMemory(const void *data, size_t length)
{
	XmlContext c(data, length);
	XmlParser p;

	if (p.parse(c) == true)
	{
		this->element = p.generate();
		return true;
	}
	else
	{
		return false;
	}
}


void Xml::save(const char *fileName)
{
	File(fileName, File::WRITE) << this->toString();
}


std::string Xml::toString() const
{
	std::string buffer;
	Xml::elementToString(buffer, this->element, 0);
	return buffer;
}
