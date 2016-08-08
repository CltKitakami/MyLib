#include "Openssl.hpp"
#include "common/Exception.hpp"


OpensslContext::~OpensslContext()
{
	this->destory();
}


void OpensslContext::create(Method method)
{
	if (this->sslCtx == nullptr || this->method != method)
	{
		this->destory();

		this->sslCtx = SSL_CTX_new(method == CLIENT ?
			SSLv23_client_method() : SSLv23_server_method());

		if (this->sslCtx == nullptr)
		{
			ERR_print_errors_fp(stderr);
			throw Exception(TRACE, "SSL_CTX_new");
		}

		this->method = method;
	}
}

void OpensslContext::destory()
{
	if (this->sslCtx != nullptr)
	{
		SSL_CTX_free(this->sslCtx);
		this->sslCtx = nullptr;
	}
}


void OpensslContext::initialize()
{
	static bool isInitialized = false;

	if (isInitialized == false)
	{
		isInitialized = true;
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
	}
}


OpensslSocket::OpensslSocket() : ssl(nullptr)
{
	OpensslContext::initialize();
}


OpensslSocket::OpensslSocket(Socket::Id id,
	OpensslContext::Method method) : ssl(nullptr)
{
	OpensslContext::initialize();
	this->open(id, method);
}


OpensslSocket::~OpensslSocket()
{
	this->close();
}


void OpensslSocket::open(Socket::Id id, OpensslContext::Method method)
{
	if (this->ssl == nullptr)
	{
		this->ctx.create(method);
		this->ssl = SSL_new(this->ctx.get());
		SSL_set_fd(this->ssl, id);
	}
}


void OpensslSocket::close()
{
	if (this->ssl != nullptr)
	{
		SSL_free(this->ssl);
		this->ssl = nullptr;
	}
}


void OpensslSocket::accept()
{
	int ret = SSL_accept(this->ssl);

	if (ret <= 0)
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));
}


void OpensslSocket::connect()
{
	int ret = SSL_connect(this->ssl);

	if (ret <= 0)
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));
}


int OpensslSocket::receive(void *buffer, int bufferSize)
{
	int ret = SSL_read(this->ssl, buffer, bufferSize);

	if (ret < 0)	// Do not throw receiving zero
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));

	return ret;
}


void OpensslSocket::send(const void *buffer, int bufferSize)
{
	int ret = SSL_write(this->ssl, buffer, bufferSize);

	if (ret <= 0)
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));
}

void OpensslSocket::send(const char *buffer)
{
	return this->send(buffer, (int)::strlen(buffer));
}


void OpensslSocket::send(const std::string &buffer)
{
	return this->send(buffer.data(), (int)buffer.length());
}


void OpensslContext::loadCertificates(const char *certFile, const char *keyFile)
{
	// openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 30 -nodes

	this->create(SERVER);

	int ret = SSL_CTX_use_certificate_file(this->sslCtx, certFile, SSL_FILETYPE_PEM);

	if (ret <= 0)
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));

	ret = SSL_CTX_use_PrivateKey_file(this->sslCtx, keyFile, SSL_FILETYPE_PEM);

	if (ret <= 0)
		throw Exception(TRACE, ERR_error_string((unsigned long)ret, nullptr));

	ret = SSL_CTX_check_private_key(this->sslCtx);

	if (ret == 0)
		throw Exception(TRACE, "Private key does not match the public certificate");
}


void OpensslSocket::loadCertificates(const char *certFile,
	const char *keyFile)
{
	this->ctx.loadCertificates(certFile, keyFile);
}


std::string OpensslSocket::getCertificate() const
{
	return this->getCertificate(this->get());
}


std::string OpensslSocket::getCertificate(const SSL *ssl)
{
	X509 *cert = SSL_get_peer_certificate(ssl);

	if (cert != nullptr)
	{
		std::string str;
		str.reserve(64);

		str.append("Server certificates:\nSubject: ");
		str.append(X509_NAME_oneline(X509_get_subject_name(cert), 0, 0));
		str.append("\nIssuer: ");
		str.append(X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0));

		X509_free(cert);

		return str;
	}
	else
		return std::string("No certificates.");
}


std::string OpensslSocket::getCertificate(const OpensslSocket &ssl)
{
	return OpensslSocket::getCertificate(ssl.get());
}

