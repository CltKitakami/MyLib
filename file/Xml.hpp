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

	inline void set(const std::string &key, const std::string &value)
	{ this->key = key; this->value = value; }

	inline void set(std::string &&key, std::string &&value)
	{ this->key = std::move(key); this->value = std::move(value); }

	inline std::string getKey() const { return this->key; }
	inline std::string getValue() const { return this->value; }

	inline bool equals(const std::string &key) const
	{ return this->key == key; }

	inline XmlAttribute & operator = (const XmlAttribute &a) = default;
	inline XmlAttribute & operator = (XmlAttribute &&a) = default;
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

	inline std::string getName() const { return this->name; }
	inline void setName(const std::string &name) { this->name = name; }
	inline void setName(std::string &&name)
	{ this->name = std::move(name); }

	inline std::string getValue() const { return this->value; }
	inline void setValue(const std::string &value) { this->value = value; }
	inline void setValue(std::string &&value)
	{ this->value = std::move(value); }

	inline bool hasValue() const { return this->value.length() != 0; }

	std::string getAttribute(const std::string &key);

	inline void addAttribute(const XmlAttribute &a)
	{ this->attributes.push_back(a); }

	inline void addAttribute(XmlAttribute &&a)
	{ this->attributes.push_back(std::move(a)); }

	void removeAttribute(const std::string &key);

	inline void addElement(const XmlElement &e)
	{ this->elements.push_back(e); }

	inline void addElement(XmlElement &&e)
	{ this->elements.push_back(std::move(e)); }

	void removeElement(const std::string &name);

	inline bool isCdata() const { return this->cdata.size() != 0; }
	inline std::string getCdata() const { return this->cdata; }
	inline void setCdata(const std::string &cdata) { this->cdata = cdata; }

	XmlElementMap buildElementMap();
	XmlAttributeMap buildAttributeMap();

	inline Elements::iterator elementsBegin()
	{ return this->elements.begin(); }

	inline Elements::iterator elementsEnd()
	{ return this->elements.end(); }

	inline Elements::const_iterator elementsBegin() const
	{ return this->elements.begin(); }

	inline Elements::const_iterator elementsEnd() const
	{ return this->elements.end(); }

	inline XmlElement & elementsFront()
	{ return this->elements.front(); }

	inline const XmlElement & elementsFront() const
	{ return this->elements.front(); }

	inline XmlElement & elementsBack()
	{ return this->elements.back(); }

	inline const XmlElement & elementsBack() const
	{ return this->elements.back(); }

	inline size_t getElementsSize() const
	{ return this->elements.size(); }

	inline Attributes::iterator attributesBegin()
	{ return this->attributes.begin(); }

	inline Attributes::iterator attributesEnd()
	{ return this->attributes.end(); }

	inline Attributes::const_iterator attributesBegin() const
	{ return this->attributes.begin(); }

	inline Attributes::const_iterator attributesEnd() const
	{ return this->attributes.end(); }

	inline XmlAttribute & attributesFront()
	{ return this->attributes.front(); }

	inline const XmlAttribute & attributesFront() const
	{ return this->attributes.front(); }

	inline XmlAttribute & attributesBack()
	{ return this->attributes.back(); }

	inline const XmlAttribute & attributesBack() const
	{ return this->attributes.back(); }

	inline size_t getAttributesSize() const
	{ return this->attributes.size(); }

	inline bool equals(const std::string &name) const
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

	inline void addElement(const XmlElement &e)
	{ this->element.addElement(e); }

	inline void addElement(XmlElement &&e)
	{ this->element.addElement(std::move(e)); }

	inline void removeElement(const std::string &name)
	{ this->element.removeElement(name); }

	inline Elements::iterator begin()
	{ return this->element.elementsBegin(); }

	inline Elements::iterator end()
	{ return this->element.elementsEnd(); }

	inline Elements::const_iterator begin() const
	{ return this->element.elementsBegin(); }

	inline Elements::const_iterator end() const
	{ return this->element.elementsEnd(); }

	inline XmlElement & front()
	{ return this->element.elementsFront(); }

	inline const XmlElement & front() const
	{ return this->element.elementsFront(); }

	inline XmlElement & back()
	{ return this->element.elementsBack(); }

	inline const XmlElement & back() const
	{ return this->element.elementsBack(); }

	Xml & operator = (const Xml &xml) = default;
	Xml & operator = (Xml &&xml) = default;

	Xml & operator = (const XmlElement &element)
	{ this->element = element; return *this; }

	Xml & operator = (XmlElement &&element)
	{ this->element = std::move(element); return *this; }
};

#endif
