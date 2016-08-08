#include "SocketIdSet.hpp"


void SocketIdSet::Set::add(Socket::Id id)
{
	if (size < ids.getSize())
	{
		FD_SET(id, &set);
		ids[size++] = id;

		if (id > max)
			max = id;
	}
}


void SocketIdSet::Set::remove(Socket::Id id)
{
	size_t find = ~0u;

	for (size_t i = 0; i < ids.getSize(); ++i)
	{
		if (ids[i] == id)
			find = i;
	}

	if (find != ~0u)
	{
		size -= 1;
		ids[find] = ids[size];
		FD_CLR(id, &set);
	}
}


void SocketIdSet::Set::restore()
{
	FD_ZERO(&set);

	for (size_t i = 0; i < size; ++i)
		FD_SET(ids[i], &set);
}


void SocketIdSet::Set::reset()
{
	FD_ZERO(&set);
	size = 0;
	max = 0;
}


bool SocketIdSet::Set::available(Socket::Id id)
{
	return FD_ISSET(id, &set) != 0;
}


SocketIdSet::SocketIdSet(size_t maxSize) :
	read(maxSize), write(maxSize)
{
	this->reset();
}


int SocketIdSet::select()
{
	int max = (int)(read.max > write.max ? read.max : write.max);
	int available = ::select(max + 1, &read.set, &write.set, 0, 0);

	if (available < 0)
		SOCKET_THROW(SOCKET_EX_SELECT);

	return available;
}


int SocketIdSet::timedSelect(int millionSecond)
{
	struct timeval tv;
	tv.tv_sec = millionSecond / 1000;
	tv.tv_usec = (millionSecond % 1000) * 1000;

	int max = (int)(read.max > write.max ? read.max : write.max);
	int available = ::select(max + 1, &read.set, &write.set, 0, &tv);

	if (available < 0)
		SOCKET_THROW(SOCKET_EX_SELECT);

	return available;
}


void SocketIdSet::restore()
{
	this->read.restore();
	this->write.restore();
}


void SocketIdSet::reset()
{
	this->read.reset();
	this->write.reset();
}
