#include "JsonContext.hpp"
#include "common/Exception.hpp"
#include "file/File.hpp"


void JsonContext::open(const char *path)
{
	File file(path, File::READ);

	if (file.isOpened() == false)
		throw Exception(TRACE, "Can't open " + std::string(path));

	std::string buffer;
	file >> buffer;

	this->openFromMemory(buffer.data(), buffer.length());
}


void JsonContext::openFromMemory(const void *data, size_t length)
{
	const char *p = reinterpret_cast<const char *>(data);

	if (p[0] == '\xef' && p[1] == '\xbb' && p[2] == '\xbf')
		p += 3, length -= 3;

	this->tokens = JsonLexer::splitTokens(p, length);
	this->currentToken = 0;
}
