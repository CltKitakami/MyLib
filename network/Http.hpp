#ifndef _HTTP_HPP_
#define _HTTP_HPP_

#include <string>

#ifdef ENABLE_OPENSSL
#include "network/Openssl.hpp"
#endif
#include "network/Tcp.hpp"
#include "network/Uri.hpp"
#include "network/CookieContainer.hpp"
#include "network/HeaderContainer.hpp"


class Http
{
public:
	typedef enum Method { GET, POST } Method;

	typedef struct Data
	{
		Data() = default;
		Data(const struct Data &data) = default;
		Data(struct Data &&data) = default;

		Data & operator = (const Data &data) = default;
		Data & operator = (Data &&data) = default;

		std::string header;
		std::string html;
	} Data;

	Uri uri;
	CookieContainer cookies;
	HeaderContainer headers;
	Data data;

	Http();
	Http(const std::string &uri);
	Http(const Http &http);
	Http(Http &&http);
	~Http();

	Http & operator = (const Http &http);
	Http & operator = (Http &&http);

	Http clone() const;

	void close();

	void get(const std::string &uri);
	void get();

	void post(const std::string &uri, const std::string &data);
	void post(const std::string &data);

	void setUri(const std::string &uri);
	void setDefaultHeaders();

	bool getSsl() const { return this->uri.getType() == Uri::HTTPS; }
	void setSsl(bool isEnable);

	uint16_t getPort() const { return this->uri.getPort(); }
	void setPort(uint16_t port) { this->uri.setPort(port); }

	void setHeader(HeaderContainer::DefaultField field, const std::string &value)
	{ this->headers.defaultFields[field] = value; }

	void setHeader(const std::string &field, const std::string &value)
	{ this->headers.customFields[field] = value; }

	std::string getHeader() const { return this->data.header; }
	std::string getHtml() const { return this->data.html; }

	void setKeepAlive(bool value) { this->isKeepAlive = value; }
	bool getKeepAlive() { return this->isKeepAlive; }

private:
	void setNetMethods();
	void request();
	void sendHeader();
	void sendHeader(const std::string &data);
	void receive();
	size_t receiveHeader();
	size_t receiveHtml(size_t htmlLength);
	size_t receiveHtml();
	void receiveChunkedHtml();
	size_t parseChunkSizeHex(size_t start, size_t &outChunkBoundary);
	size_t appendChunk(std::string &buffer, size_t processedSize, size_t chunkSize);
	void recevieEndOfChunkedEncoding();
	size_t seekChunkString(size_t cursor);
	size_t findContentLength();
	int getStatusCode();
	std::string getStatusString();
	size_t getRemainHtmlLength();
	void htmlDecompress();
	void saveCookie();
	bool isChunkedHtml();
	bool isReceiveHtmlEnd();
	bool isGzipHtml();
	std::string getRequestLine(Http::Method method);

	typedef void (Http::*NetSend)(const std::string &data);
	typedef size_t (Http::*NetReceive)(char *data, size_t length);

	void tcpSend(const std::string &data);
	size_t tcpReceive(char *data, size_t length);

#ifdef ENABLE_OPENSSL
	void sslSend(const std::string &data);
	size_t sslReceive(char *data, size_t length);
#endif

	static const int recvBlockSize;

	bool isConnected;
	bool isKeepAlive;

	Tcp tcp;
#ifdef ENABLE_OPENSSL
	OpensslSocket ssl;
#endif
	NetSend netSend;
	NetReceive netReceive;
};

#endif
