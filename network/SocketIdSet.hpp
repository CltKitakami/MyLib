#ifndef _SOCKETIDSET_HPP_
#define _SOCKETIDSET_HPP_

#include <cstdlib>

#include "network/Socket.hpp"
#include "common/Array.hpp"


class SocketIdSet
{
	class Set
	{
	public:
		fd_set set;
		size_t size;
		Array<Socket::Id> ids;
		Socket::Id max;

		Set(size_t maxSize) : ids(maxSize) {}

		void add(Socket::Id id);
		void remove(Socket::Id id);
		void restore();
		void reset();
		bool available(Socket::Id id);
	};

public:
	SocketIdSet(size_t maxSize);

	int select();
	int timedSelect(int millionSecond);

	void restore();
	void reset();

	Set read, write;
};

#endif
