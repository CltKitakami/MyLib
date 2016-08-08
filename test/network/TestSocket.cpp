#include <cstdlib>

#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "network/Tcp.hpp"
#include "network/Udp.hpp"


// #define UDP
#ifdef UDP
#define SOCKET_CLASS Udp
#else
// #define WEB
#define SOCKET_CLASS Tcp
#endif


void createClient();
void createServer();


int main()
{
	Wsa wsa; (void)wsa;

#ifdef WEB
	createClient();
#else
	LOGT("Are you server? (y or n):");

	char yesNo = 'n';
	std::cin >> yesNo;

	if (yesNo == 'y')
	{
		LOGT("I'm server.");
		createServer();
	}
	else
	{
		LOGT("I'm client.");
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
		SOCKET_CLASS client;
		client.open();
#ifdef WEB
		unsigned long addr = Socket::getHostByName("www.google.com.tw");
		LOGT("IP: " << std::hex << addr << std::dec);
		client.setAddress(addr);
		client.setPort(80);
#else
		client.setAddress("127.0.0.1");
		client.setPort(8888);
#endif

#ifndef UDP
		client.connect();
#endif

#ifdef WEB
		client.send("GET / HTTP/1.1\r\n\r\n");
#else
		while (client.send("Hi, I'm client.") == false);
#endif

		int recvBytes = 0;
		do {
			recvBytes = client.receive(recvBuf, 1024);
			if (recvBytes > 0)
			{
				recvBuf[recvBytes] = '\0';
				LOGT("recvBytes = " << recvBytes);
				LOGT("[from server]: " << recvBuf);
			}
		} while (recvBytes <= 0);

		client.close();
	}
	catch (Exception ex)
	{
		LOGT(ex.getMessage());
	}
}


void createServer()
{
	try
	{
		char receBuf[1025];

		SOCKET_CLASS server;
		server.open();
		server.setPort(8888);
		server.bind();
#ifndef UDP
		server.listen();
		SOCKET_CLASS client(server.accept());
		server.close();	// No longer accept
#else
		SOCKET_CLASS client(server.getId());
#endif

		if ((bool)client)
		{
			int recvBytes = 0;
			do {
				recvBytes = client.receive(receBuf, 1024);
				if (recvBytes > 0)
				{
					receBuf[recvBytes] = '\0';
					LOGT("recvBytes = " << recvBytes);
					LOGT("[from client]: " << receBuf);
				}
			} while (recvBytes <= 0);

			while (client.send("Hello! I'm server.") == false);
		}
	}
	catch (Exception ex)
	{
		LOGT(ex.getMessage());
	}
}
