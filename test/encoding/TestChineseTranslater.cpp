#include "common/Log.hpp"
#include "encoding/ChineseTranslater.hpp"
#include "encoding/Iconv.hpp"
#include "encoding/Iconv.cpp"


int main()
{
	ChineseTranslater::initialize();

	std::string buffer("\xb2\xe2\xca\xd4");	// GBK encoding "ด๚ธี"

	std::string utf8 = ChineseTranslater::translate(buffer);
	LOG_VAR(utf8.length());
	LOG_VAR(utf8);

	std::string big5 = Iconv::translate(utf8, "UTF-8", "BIG5");
	LOG_VAR(big5.length());
	LOG_VAR(big5);

	return 0;
}

