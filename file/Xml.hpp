#ifndef _XML_HPP_
#define _XML_HPP_

#include <string>
#include <vector>
#include <list>
#include <map>


class XmlAttribute
{
	std::string key, value;

public:
	XmlAttribute() = default;
	XmlAttribute(const XmlAttribute &a) = default;
	XmlAttribute(XmlAttribute &&a) = default;

	XmlAttribute(const std::string &key, const std::string &value) :
		key(key), value(value) {}

	XmlAttribute(std::string &&key, std::string &&value) :
		key(std::move(key)), value(std::move(value)) {}

	void set(const std::string &key, const std::string &value)
	{ this->key = key; this->value = value; }

	void set(std::string &&key, std::string &&value)
	{ this->key = std::move(key); this->value = std::move(value); }

	std::string getKey() const { return this->key; }
	std::string getValue() const { return this->value; }

	bool equals(const std::string &key) const
	{ return this->key == key; }

	XmlAttribute & operator = (const XmlAttribute &a) = default;
	XmlAttribute & operator = (XmlAttribute &&a) = default;
};


class XmlElement;
typedef std::map<std::string, XmlElement *> XmlElementMap;
typedef std::map<std::string, XmlAttribute *> XmlAttributeMap;


class XmlElement
{
public:
	typedef std::list<XmlElement> Elements;
	typedef std::list<XmlAttribute> Attributes;

	XmlElement() = default;
	XmlElement(const std::string &name) : name(name) {}
	XmlElement(std::string &&name) : name(std::move(name)) {}
	XmlElement(const XmlElement &e) = default;
	XmlElement(XmlElement &&e) = default;

	std::string getName() const { return this->name; }
	void setName(const std::string &name) { this->name = name; }
	void setName(std::string &&name)
	{ this->name = std::move(name); }

	std::string getValue() const { return this->value; }
	void setValue(const std::string &value) { this->value = value; }
	void setValue(std::string &&value)
	{ this->value = std::move(value); }

	bool hasValue() const { return this->value.length() != 0; }

	std::string getAttribute(const std::string &key);

	void addAttribute(const XmlAttribute &a)
	{ this->attributes.push_back(a); }

	void addAttribute(XmlAttribute &&a)
	{ this->attributes.push_back(std::move(a)); }

	void removeAttribute(const std::string &key);

	void addElement(const XmlElement &e)
	{ this->elements.push_back(e); }

	void addElement(XmlElement &&e)
	{ this->elements.push_back(std::move(e)); }

	void removeElement(const std::string &name);

	bool isCdata() const { return this->cdata.size() != 0; }
	std::string getCdata() const { return this->cdata; }
	void setCdata(const std::string &cdata) { this->cdata = cdata; }

	XmlElementMap buildElementMap();
	XmlAttributeMap buildAttributeMap();

	Elements::iterator elementsBegin()
	{ return this->elements.begin(); }

	Elements::iterator elementsEnd()
	{ return this->elements.end(); }

	Elements::const_iterator elementsBegin() const
	{ return this->elements.begin(); }

	Elements::const_iterator elementsEnd() const
	{ return this->elements.end(); }

	XmlElement & elementsFront()
	{ return this->elements.front(); }

	const XmlElement & elementsFront() const
	{ return this->elements.front(); }

	XmlElement & elementsBack()
	{ return this->elements.back(); }

	const XmlElement & elementsBack() const
	{ return this->elements.back(); }

	size_t getElementsSize() const
	{ return this->elements.size(); }

	Attributes::iterator attributesBegin()
	{ return this->attributes.begin(); }

	Attributes::iterator attributesEnd()
	{ return this->attributes.end(); }

	Attributes::const_iterator attributesBegin() const
	{ return this->attributes.begin(); }

	Attributes::const_iterator attributesEnd() const
	{ return this->attributes.end(); }

	XmlAttribute & attributesFront()
	{ return this->attributes.front(); }

	const XmlAttribute & attributesFront() const
	{ return this->attributes.front(); }

	XmlAttribute & attributesBack()
	{ return this->attributes.back(); }

	const XmlAttribute & attributesBack() const
	{ return this->attributes.back(); }

	size_t getAttributesSize() const
	{ return this->attributes.size(); }

	bool equals(const std::string &name) const
	{ return this->name == name; }

	XmlElement & operator = (const XmlElement &e) = default;
	XmlElement & operator = (XmlElement &&e) = default;

private:
	Elements elements;
	Attributes attributes;
	std::string name, value, cdata;
};


class Xml
{
	XmlElement element;

	static void newLineToString(std::string &buffer, int tabDepth);

	static void attributeToString(std::string &buffer,
		const XmlElement &elements);

	static void elementToString(std::string &buffer,
		const XmlElement &elements, int tabDepth);

public:
	typedef XmlElement::Elements Elements;
	typedef XmlElement::Attributes Attributes;

	Xml() = default;
	Xml(const char *fileName) { (void)open(fileName); }
	Xml(const std::string &fileName) { (void)open(fileName.data()); }
	Xml(const void *data, size_t length)
	{ (void)openFromMemory(data, length); }

	Xml(const Xml &xml) = default;
	Xml(Xml &&xml) = default;

	Xml(const XmlElement &element) : element(element) {}
	Xml(XmlElement &&element) : element(std::move(element)) {}

	bool open(const char *fileName);
	bool openFromMemory(const void *data, size_t length);
	void save(const char *fileName);

	std::string toString() const;

	void addElement(const XmlElement &e)
	{ this->element.addElement(e); }

	void addElement(XmlElement &&e)
	{ this->element.addElement(std::move(e)); }

	void removeElement(const std::string &name)
	{ this->element.removeElement(name); }

	Elements::iterator begin()
	{ return this->element.elementsBegin(); }

	Elements::iterator end()
	{ return this->element.elementsEnd(); }

	Elements::const_iterator begin() const
	{ return this->element.elementsBegin(); }

	Elements::const_iterator end() const
	{ return this->element.elementsEnd(); }

	XmlElement & front()
	{ return this->element.elementsFront(); }

	const XmlElement & front() const
	{ return this->element.elementsFront(); }

	XmlElement & back()
	{ return this->element.elementsBack(); }

	const XmlElement & back() const
	{ return this->element.elementsBack(); }

	Xml & operator = (const Xml &xml) = default;
	Xml & operator = (Xml &&xml) = default;

	Xml & operator = (const XmlElement &element)
	{ this->element = element; return *this; }

	Xml & operator = (XmlElement &&element)
	{ this->element = std::move(element); return *this; }
};

#endif
