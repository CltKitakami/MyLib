#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "network/Tcp.hpp"
#include "network/SocketIdSet.hpp"

void createClient();

int main()
{
	Wsa wsa; (void)wsa;
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
		client.setBlocking(false);

		unsigned long addr = Socket::getHostByName("www.google.com.tw");
		LOGP("IP: " << Socket::ipv4AddressToString(addr));

		client.setAddress(addr);
		client.setPort(80);

		if (client.connect() != true)
		{
			LOGP("Wait to write");
			set.reset();
			set.write.add(client.getId());
			set.select();

			if (client.connect() != true)
				throw Exception(TRACE, "Can't connect.");
		}

		LOGP("Wait to write");
		set.reset();
		set.write.add(client.getId());
		set.select();

		if (set.write.available(client.getId()))
			client.send("GET / HTTP/1.1\r\n\r\n");
		else
			throw Exception(TRACE, "Not available to send.");

		LOGP("Wait to read");
		set.reset();
		set.read.add(client.getId());
		set.timedSelect(5000);

		if (set.read.available(client.getId()))
		{
			int recvBytes = client.receive(recvBuf, 1024);

			if (recvBytes > 0)
			{
				recvBuf[recvBytes] = '\0';
				LOGP("recvBytes = " << recvBytes);
				LOGP("[from server]: " << recvBuf);
			}
		}
		else
		{
			throw Exception(TRACE, "Receive timeouted.");
		}
	}
	catch (Exception &ex)
	{
		LOGP(ex.getMessage());
	}

	delete [] recvBuf;
}
