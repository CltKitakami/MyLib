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
		LOGP("name = " << name);
		LOGP("name.a = " << nameAttr["a"]->getValue());
		LOGP("name.b = " << nameAttr["b"]->getValue());
	}
	catch (Exception &ex)
	{
		LOGP(ex.getMessage());
	}
	return 0;
}
