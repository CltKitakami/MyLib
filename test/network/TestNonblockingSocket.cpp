#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "network/Tcp.hpp"
#include "network/SocketIdSet.hpp"

void createClient();

int main()
{
	Wsa wsa;
	createClient();

	return 0;
}

void createClient()
{
	char *recvBuf = new char[1025];

	try
	{
		Tcp client;
		SocketIdSet set(1);

		client.open();
		client.setBlocking(true);

		unsigned long addr = Socket::getHostByName("www.google.com.tw");
		LOGF("IP: 0x%x", addr);
		client.setAddress(addr);
		client.setPort(80);

		if (client.connect() != true)
		{
			set.reset();
			set.writeAdd(client.id());
			set.select();
			if (client.connect() != true)
				throw Exception(TRACE, "Can't connect.");
		}

		set.reset();
		set.writeAdd(client.id());
		set.select();
		if (set.writeAvailable(client.id()))
			client.send("GET / HTTP/1.1\r\n\r\n");

		set.reset();
		set.readAdd(client.id());
		set.select();

		int recvBytes = client.receive(recvBuf, 1024);

		if (recvBytes > 0)
		{
			recvBuf[recvBytes] = '\0';
			LOG.d("recvBytes = " << recvBytes);
			LOG.d("[from server]: " << recvBuf);
		}

		client.close();
	}
	catch (Exception &ex)
	{
		LOG.d(ex.getMessage());
	}

	delete [] recvBuf;
}
