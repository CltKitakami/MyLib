#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "file/Xml.hpp"

const std::string xmlFile =
"<settings>\n"
"	<name a=\"a\" b=\"b\">test</name>\n"
"</settings>";

int main()
{
	try
	{
		Xml xml(xmlFile.data(), xmlFile.length());

		XmlElementMap xmlElem = xml.begin()->buildElementMap();
		XmlAttributeMap nameAttr = xmlElem["name"]->buildAttributeMap();

		std::string name = xmlElem["name"]->getValue();

		LOGP(xml.toString());
		LOGP("===========");
		LOGP("name = " << name);
		LOGP("name.a = " << nameAttr["a"]->getValue());
		LOGP("name.b = " << nameAttr["b"]->getValue());
		LOGP("===========");

		XmlElement elem("name2");
		elem.addAttribute(XmlAttribute("width", "100"));
		elem.addAttribute(XmlAttribute("height", "120"));
		elem.addElement(XmlElement("name3"));
		elem.setValue("test2");
		xml.back().addElement(elem);

		LOGP(xml.toString());
	}
	catch (Exception &ex)
	{
		LOGP(ex.getMessage());
	}
	return 0;
}
