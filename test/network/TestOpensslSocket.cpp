#include <cstdlib>

#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "network/Tcp.hpp"
#include "network/Udp.hpp"
#include "network/Openssl.hpp"


#define WEB


void createClient();
void createServer();


int main()
{
	Wsa wsa; (void)wsa;

#ifdef WEB
	createClient();
#else
	LOGP("Are you server? (y or n):");

	char yesNo = 'n';
	std::cin >> yesNo;

	if (yesNo == 'y')
	{
		LOGP("I'm server.");
		createServer();
	}
	else
	{
		LOGP("I'm client.");
		createClient();
	}
#endif

	return 0;
}


void createClient()
{
	try
	{
		char recvBuf[1025];
		Tcp client;
		OpensslSocket ssl;
		client.open();
		ssl.open(client.getId(), OpensslContext::CLIENT);
#ifdef WEB
		unsigned long addr = Socket::getHostByName("www.google.com.tw");
		LOGF("IP: 0x%x", addr);
		client.setAddress(addr);
		client.setPort(443);
#else
		client.setAddress("127.0.0.1");
		client.setPort(8888);
#endif

		client.connect();
		ssl.connect();
		LOGP(ssl.getCertificate());

#ifdef WEB
		ssl.send("GET / HTTP/1.1\r\n\r\n");
#else
		ssl.send("Hi, I'm client.");
#endif

		int recvBytes = ssl.receive(recvBuf, 1024);
		if (recvBytes > 0)
		{
			recvBuf[recvBytes] = '\0';
			LOGP("recvBytes = " << recvBytes);
			LOGP("[from server]: " << recvBuf);
		}

		ssl.close();
		client.close();
	}
	catch (Exception ex)
	{
		LOGP(ex.getMessage());
		ERR_print_errors_fp(stderr);
	}
}


void createServer()
{
	try
	{
		char receBuf[1025];

		Tcp server;
		server.open();
		server.setPort(8888);
		server.bind();
		server.listen();
		Tcp client(server.accept());
		server.close();	// No longer accept

		if ((bool)client)
		{
			OpensslSocket ssl;
			ssl.loadCertificates("cert.pem", "key.pem");
			ssl.open(client.getId(), OpensslContext::SERVER);
			ssl.accept();
			LOGP(ssl.getCertificate());

			int recvBytes = ssl.receive(receBuf, 1024);
			if (recvBytes > 0)
			{
				receBuf[recvBytes] = '\0';
				LOGP("recvBytes = " << recvBytes);
				LOGP("[from client]: " << receBuf);
			}

			ssl.send("Hello! I'm server.");

			ssl.close();
			client.close();
		}
	}
	catch (Exception ex)
	{
		LOGP(ex.getMessage());
		ERR_print_errors_fp(stderr);
	}
}
