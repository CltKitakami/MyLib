#ifndef _OPENSSL_HPP_
#define _OPENSSL_HPP_

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "Socket.hpp"


class OpensslContext
{
public:
	enum Method { CLIENT, SERVER, UNKNOWN };

	OpensslContext() : sslCtx(nullptr), method(UNKNOWN) {}
	~OpensslContext();

	void create(Method method);
	void destory();

	void loadCertificates(const char *certFile, const char *keyFile);

	SSL_CTX * get() { return sslCtx; }

	static void initialize();

private:

	SSL_CTX *sslCtx;
	Method method;
};


class OpensslSocket
{
public:
	OpensslSocket();
	OpensslSocket(Socket::Id id, OpensslContext::Method method);
	~OpensslSocket();

	const SSL * get() const { return this->ssl; }

	void open(Socket::Id id, OpensslContext::Method method);
	void close();

	void accept();

	void connect();

	int receive(void *buffer, int bufferSize);

	void send(const void *buffer, int bufferSize);
	void send(const char *buffer);
	void send(const std::string &buffer);

	void loadCertificates(const char *certFile, const char *keyFile);

	std::string getCertificate() const;

	static std::string getCertificate(const SSL *ssl);
	static std::string getCertificate(const OpensslSocket &ssl);

protected:
	OpensslContext ctx;
	SSL *ssl;
};


#endif
