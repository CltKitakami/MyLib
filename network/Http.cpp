#include "Http.hpp"
#include "common/Convert.hpp"
#include "common/ConstString.hpp"
#include "common/Exception.hpp"
#include "common/Log.hpp"
#include "common/String.hpp"
#include "encoding/Gzip.hpp"
#include "thread/Thread.hpp"


// #define HTTP_DEBUG


#ifdef _WIN32
#define SYSTEM_AGENT "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0"
#else
#define SYSTEM_AGENT "Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:32.0) Gecko/20100101 Firefox/32.0"
#endif


const int Http::recvBlockSize = 8192;


Http::Http() :
	isConnected(false),
	isKeepAlive(false)
{
	this->setDefaultHeaders();
}


Http::Http(const std::string &uri) :
	isConnected(false),
	isKeepAlive(false)
{
	this->setDefaultHeaders();
	this->setUri(uri);
}


Http::Http(const Http &http) :
	uri(http.uri),
	cookies(http.cookies),
	headers(http.headers),
	data(http.data),
	isConnected(false),
	isKeepAlive(false) {}


Http::Http(Http &&http) :
	uri(std::move(http.uri)),
	cookies(std::move(http.cookies)),
	headers(std::move(http.headers)),
	data(std::move(http.data)),
	isConnected(false),
	isKeepAlive(false) {}


Http::~Http()
{
	this->close();
}


Http & Http::operator = (const Http &http)
{
	this->uri = http.uri;
	this->cookies = http.cookies;
	this->headers = http.headers;
	this->data = http.data;
	this->isConnected = http.isConnected;
	this->isKeepAlive = http.isKeepAlive;
	return *this;
}

Http & Http::operator = (Http &&http)
{
	this->uri = std::move(http.uri);
	this->cookies = std::move(http.cookies);
	this->headers = std::move(http.headers);
	this->data = std::move(http.data);
	this->isConnected = http.isConnected;
	this->isKeepAlive = http.isKeepAlive;
	return *this;
}


Http Http::clone() const
{
	Http http;
	http.uri = this->uri;
	http.cookies = this->cookies;
	http.headers = this->headers;
	http.data = this->data;
	return http;
}


void Http::close()
{
#ifdef ENABLE_OPENSSL
	if (this->uri.getType() == Uri::HTTPS)
		this->ssl.close();
#endif

	this->tcp.close();
	this->isConnected = false;
}


void Http::setNetMethods()
{
	if (this->uri.getType() == Uri::HTTPS)
	{
#ifdef ENABLE_OPENSSL
		this->netSend = &Http::sslSend;
		this->netReceive = &Http::sslReceive;
#else
		throw Exception(TRACE, "Not support HTTPS");
#endif
	}
	else
	{
		this->netSend = &Http::tcpSend;
		this->netReceive = &Http::tcpReceive;
	}
}


void Http::setUri(const std::string &uri)
{
	this->uri.set(uri);
	this->setNetMethods();
	this->setHeader(HeaderContainer::HOST, this->uri.getHostName());
}


void Http::setSsl(bool isEnable)
{
	this->uri.setType(isEnable == true ? Uri::HTTPS : Uri::HTTP);
	this->setNetMethods();
}


void Http::get(const std::string &uri)
{
	this->setUri(uri);
	this->get();
}


void Http::get()
{
	this->request();
	this->sendHeader();
	this->receive();
	this->saveCookie();
}


void Http::post(const std::string &uri, const std::string &data)
{
	this->setUri(uri);
	this->post(data);
}


void Http::post(const std::string &data)
{
	this->request();
	this->sendHeader(data);
	this->receive();
	this->saveCookie();
}


void Http::setDefaultHeaders()
{
	this->setHeader(HeaderContainer::HOST, this->uri.getHostName());
	this->setHeader(HeaderContainer::USER_AGENT, SYSTEM_AGENT);
	this->setHeader(HeaderContainer::ACCEPT, "*/*");
	this->setHeader(HeaderContainer::ACCEPT_LANGUAGE, "zh-tw,zh;q=0.8,en-us;q=0.5,en;q=0.3");
	this->setHeader(HeaderContainer::ACCEPT_ENCODING, "gzip");
	this->setHeader(HeaderContainer::CONNECTION, isKeepAlive ? "keep-alive" : "close");
}


void Http::request()
{
	if (this->isConnected == false)
	{
		this->tcp.open();
		unsigned long addr = Socket::getHostByName(this->uri.getHostName().data());
		this->tcp.setAddress(addr);
		this->tcp.setPort(this->uri.getPort());
		this->tcp.connect();

#ifdef ENABLE_OPENSSL
		if (this->uri.getType() == Uri::HTTPS)
		{
			this->ssl.open(this->tcp.getId(), OpensslContext::CLIENT);
			this->ssl.connect();
		}
#endif

		this->isConnected = true;
	}
}


void Http::sendHeader()
{
	this->setHeader(HeaderContainer::CONTENT_TYPE, "");
	this->setHeader(HeaderContainer::CONTENT_LENGTH, "");

	std::string headersStr = this->headers.toString(
		getRequestLine(GET), cookies.toString());

#ifdef HTTP_DEBUG
	LOG.d(getRequestLine(GET));
	LOG.d("<send header>");
	LOG.d(headersStr);
	LOG.d("</send header>");
#endif

	(this->*netSend)(headersStr);
}


void Http::sendHeader(const std::string &data)
{
	this->setHeader(HeaderContainer::CONTENT_TYPE,
		"application/x-www-form-urlencoded; charset=UFT-8");
	this->setHeader(HeaderContainer::CONTENT_LENGTH,
		Convert::toString((uint32_t)data.length()));

	std::string headersStr = headers.toString(getRequestLine(POST), cookies.toString());
	headersStr += data;

#ifdef HTTP_DEBUG
	LOG.d(getRequestLine(POST));
	LOG.d("<send header>");
	LOG.d(headersStr);
	LOG.d("</send header>");
#endif

	(this->*netSend)(headersStr);
}


void Http::receive()
{
	this->data.header.clear();
	this->data.html.clear();

	size_t htmlRemainLength = this->receiveHeader();
	int status = this->getStatusCode();

#ifdef HTTP_DEBUG
	LOG.d("<recevie header>");
	LOG.d(this->data.header);
	LOG.d("</recevie header>");
#endif

	if (status < 400)
	{
		if (htmlRemainLength != ~0u)
			this->receiveHtml(htmlRemainLength);
		else if (this->isChunkedHtml())
			this->receiveChunkedHtml();
		else
			this->receiveHtml();

		if (this->data.html.length() == 0)
		{
			LOG.w(TRACE, "recv zero");
			this->receiveHtml();
		}

#ifdef HTTP_DEBUG
		LOG.d("<html>");
		LOG.d(this->data.html);
		LOG.d("</html>");
#endif

		if (this->isGzipHtml() == true)
			this->htmlDecompress();
	}
	else
	{
		std::string statusStr = this->getStatusString();
		LOG.d(TRACE, Log::format("receive: %d %s", status, statusStr.data()));
	}

	if (this->isKeepAlive == false)
		this->close();
}


size_t Http::receiveHeader()
{
	const std::string recvEnd("\r\n\r\n");
	size_t recvSize = 0, totalSize = 0;

	do
	{
		char *ptr = String::getSpace(data.header, totalSize, recvBlockSize);
		recvSize = (this->*netReceive)(ptr, recvBlockSize);
		size_t endAt = String::extract(data.header, totalSize, recvEnd);
		totalSize += recvSize;

		if (endAt != ~0u)
		{
			this->data.html.assign(data.header.data() + endAt, totalSize - endAt);
			this->data.header.resize(endAt);
			break;
		}
	} while (recvSize > 0);

	if (recvSize <= 0)
		this->data.html.resize(totalSize);

	return this->getRemainHtmlLength();
}


size_t Http::receiveHtml(size_t htmlRemainLength)
{
	size_t htmlLength = this->data.html.length();
	size_t recvSize = 0, remainSize = htmlRemainLength;
	char *ptr = String::getSpace(data.html, htmlLength, htmlRemainLength);

	do
	{
		size_t blockSize = remainSize > recvBlockSize ? recvBlockSize : remainSize;

		recvSize = (this->*netReceive)(ptr, blockSize);

		if (recvSize > 0)
		{
			ptr += recvSize;
			remainSize -= recvSize;
		}
	} while (remainSize > 0 && recvSize != 0);

	if (remainSize > 0)
		this->data.html.resize((size_t)(htmlLength + htmlRemainLength - remainSize));

	return remainSize;
}


size_t Http::receiveHtml()
{
	size_t recvSize = 0;
	size_t totalSize = this->data.html.length();
	bool endAt = this->isReceiveHtmlEnd();

	if (endAt == false)
		this->data.html.reserve(recvBlockSize * 16);
	else
		return totalSize;

	do
	{
		char *ptr = String::getSpace(data.html, totalSize, recvBlockSize);
		recvSize = (this->*netReceive)(ptr, recvBlockSize);
		totalSize += recvSize;
		endAt = this->isReceiveHtmlEnd();
	} while (endAt == false && recvSize > 0);

	this->data.html.resize(totalSize);

	return totalSize;
}


size_t Http::parseChunkSizeHex(size_t start, size_t &outChunkBoundary)
{
	const char *p = this->data.html.data() + start;
	size_t i = 0, chunkSize = 0;

	for (; i < 10 && Convert::isHexChar(p[i]) == true; ++i)
	{
		chunkSize <<= 4;
		chunkSize += Convert::hexToByte((uint8_t)p[i]);
	}

	outChunkBoundary = start + i + 2;

	if (p[i] != '\0' && p[i] == '\r' &&
		p[i + 1] != '\0' && p[i + 1] == '\n')
	{
		return chunkSize;
	}
	else
	{
		return ~0u;
	}
}


size_t Http::appendChunk(std::string &buffer, size_t processedSize, size_t chunkSize)
{
	const char *start = &this->data.html[processedSize];
	const char *end = start + chunkSize;

	if (end[0] == '\r' && end[1] == '\n')
	{
		buffer.append(start, chunkSize);
		this->data.html.erase(0, processedSize + chunkSize + 2);
		return 0;
	}
	else
	{
		return ~0u;
	}
}


void Http::recevieEndOfChunkedEncoding()
{
	if (this->isReceiveHtmlEnd() == false)
	{
		char dump[4], *ptr = dump;
		(this->*netReceive)(ptr, 4);	// get rid of remain bytes of end
	}
}


void Http::receiveChunkedHtml()
{
	std::string buffer;
	buffer.reserve(recvBlockSize * 16);
	size_t cursor = 0;

	while (true)
	{
		cursor = this->seekChunkString(cursor);

		if (cursor == ~0u)
			throw Exception(TRACE, "recevie chunk data error.");

		size_t chunkBoundary = 0;
		size_t chunkSize = this->parseChunkSizeHex(cursor, chunkBoundary);

		if (chunkSize == ~0u)
		{
			throw Exception(TRACE, "parse chunk hex error.");
		}
		else if (chunkSize == 0)
		{
			this->recevieEndOfChunkedEncoding();
			break;
		}

		ssize_t remainSize = (ssize_t)(chunkBoundary +
			chunkSize + 2 -data.html.length());

		if (remainSize > 0)
		{
			// a little more than remainSize
			size_t remainAndNextChunkSize = (size_t)remainSize + 10;

			if (this->receiveHtml(remainAndNextChunkSize) ==
				remainAndNextChunkSize)
			{
				LOG.w(TRACE, "can not recevie chunk data.");
				break;	// receive empty data
			}
		}

		cursor = this->appendChunk(buffer, chunkBoundary, chunkSize);
	}

	this->data.html.swap(buffer);
}


size_t Http::seekChunkString(size_t cursor)
{
	if (cursor == this->data.html.length())
		this->receiveHtml(recvBlockSize);	// recevie more data

	if (Convert::isHexChar(this->data.html[cursor]) == false)
	{
		size_t chunkBegin = this->data.html.find("\r\n", cursor);

		if (chunkBegin == std::string::npos)
		{
			return ~0u;
		}
		else if (chunkBegin + 2 < this->data.html.length() ||
			Convert::isHexChar(this->data.html[chunkBegin + 2]) == true)
		{
			return chunkBegin + 2;
		}
		else
		{
			return ~0u;
		}
	}

	return cursor;
}


size_t Http::findContentLength()
{
	ConstString lengthStr("Content-Length: ");
	size_t at = this->data.header.find(
		lengthStr.getCharArray(), 0, lengthStr.getLength());

	if (at != std::string::npos)
	{
		return Convert::stringToUint<uint32_t>
			(&this->data.header[at + lengthStr.getLength()]);
	}
	else
	{
		return ~0u;
	}
}


int Http::getStatusCode()
{
	if (this->data.header.length() > 13)	// strlen("HTTP/1.1 xxx ")
	{
		ConstString prefix("HTTP/1.1 ");
		const char *p = this->data.header.data() + prefix.getLength();

		if (Convert::isInRange(p[0], '0', '9') == true &&
			Convert::isInRange(p[1], '0', '9') == true &&
			Convert::isInRange(p[2], '0', '9') == true)
		{
			return ((int)p[0] - (int)'0') * 100 +
				((int)p[1] - (int)'0') * 10 +
				(int)p[2] - (int)'0';
		}
		else
			return 0;
	}
	else
		return 0;
}


std::string Http::getStatusString()
{
	std::string status;
	status.reserve(32);

	const char *p = this->data.header.data();
	ConstString prefix("HTTP/1.1 xxx ");

	for (size_t i = prefix.getLength();
		i < this->data.header.length(); ++i)
	{
		if (p[i] == '\r')
			break;
		else
			status.push_back(p[i]);
	}

	return status;
}


size_t Http::getRemainHtmlLength()
{
	size_t htmlLength = this->findContentLength();

	if (htmlLength != ~0u)
	{
		size_t remain = htmlLength - this->data.html.length();
		return remain > 0 ? remain : ~0u;
	}
	else
		return ~0u;
}


void Http::htmlDecompress()
{
	this->data.html = Gzip::decompress(&this->data.html[0], this->data.html.length());
}


void Http::saveCookie()
{
	ConstString setCookie("Set-Cookie: ");
	size_t cursor = 0;

	while (true)
	{
		size_t at = data.header.find(
			setCookie.getCharArray(), cursor, setCookie.getLength());

		if (at == std::string::npos)
			break;

		at += setCookie.getLength();

		size_t end = data.header.find('\r', at);

		if (end == std::string::npos)
			end = data.header.length();

		this->cookies.addFromCookieString(data.header.substr(at, end - at));
		cursor = end;
	}

	this->cookies.remove("expires");
	this->cookies.remove("path");
}


bool Http::isChunkedHtml()
{
	ConstString encoding("Transfer-Encoding");
	size_t at = this->data.header.find(
		encoding.getCharArray(), 0, encoding.getLength());
	return at != std::string::npos;
}


bool Http::isReceiveHtmlEnd()
{
	int len = (int)this->data.html.length();
	const char *ptr = this->data.html.data() + len - 1;
	return *(ptr - 3) == '\r' && *(ptr - 2) == '\n' &&
		*(ptr - 1) == '\r' && *ptr == '\n';
}


bool Http::isGzipHtml()
{
	std::string value = HeaderContainer::findHeader(this->data.header, "Content-Encoding");
	return value.length() > 0 && value == "gzip";
}


std::string Http::getRequestLine(Http::Method method)
{
	std::string requestLine;
	requestLine.reserve(10 + this->uri.getHostPath().length());
	requestLine += method == POST ? "POST " : "GET ";
	requestLine += this->uri.getHostPath();
	requestLine += " HTTP/1.1\r\n";
	return requestLine;
}


void Http::tcpSend(const std::string &data)
{
	tcp.send(data);
}


size_t Http::tcpReceive(char *data, size_t length)
{
	return (size_t)tcp.receive(data, (int)length);
}


#ifdef ENABLE_OPENSSL
void Http::sslSend(const std::string &data)
{
	ssl.send(data);
}


size_t Http::sslReceive(char *data, size_t length)
{
	return (size_t)ssl.receive(data, (int)length);
}
#endif
