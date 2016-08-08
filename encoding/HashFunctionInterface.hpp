#ifndef _HASHFUNCTIONINTERFACE_HPP_
#define _HASHFUNCTIONINTERFACE_HPP_

#include <cstdint>
#include <string>
#include "common/Convert.hpp"


template <int BLOCK_SIZE, int LENGTH_SIZE>
class HashFunctionInterface
{
public:
	HashFunctionInterface() = default;
	virtual ~HashFunctionInterface() = default;

	virtual void reset() = 0;

	virtual void transform(const uint8_t *data, size_t length);

	void transform(const char *data, size_t length)
	{ transform(reinterpret_cast<const uint8_t *>(data), length); }

	void transform(const std::string &data)
	{ transform(data.data(), data.length()); }

	virtual void finalize();

	virtual std::string toString() = 0;

protected:
	enum { BLOCK_LENGTH = BLOCK_SIZE, LENGTH_MESSAGE = LENGTH_SIZE };

	virtual size_t transformPreviousBlock(const uint8_t *data, size_t length);
	virtual void transformBlock(const uint8_t *) = 0;
	virtual void addLengthMessage() = 0;

	static void addLengthMessageLittleEndian(uint64_t length, uint8_t *block);
	static void addLengthMessageBigEndian(uint64_t length, uint8_t *block);

	uint64_t length;
	uint8_t lastBlock[BLOCK_LENGTH];
	size_t lastBlockOffset;
};


template <int BLOCK_SIZE, int LENGTH_SIZE>
void HashFunctionInterface<BLOCK_SIZE, LENGTH_SIZE>::transform(
	const uint8_t *data, size_t length)
{
	size_t skipLength = this->transformPreviousBlock(data, length);
	const uint8_t *p = data + skipLength;
	size_t blockCount = (length + skipLength) / BLOCK_LENGTH;
	size_t remainCount = (length + skipLength) % BLOCK_LENGTH;

	for (size_t i = 0; i < blockCount; i += 1, p += (size_t)BLOCK_LENGTH)
		this->transformBlock(p);

	if (remainCount != 0)
	{
		::memcpy(this->lastBlock, p, remainCount);
		this->lastBlockOffset = remainCount;
	}

	this->length += (uint64_t)length;
}


template <int BLOCK_SIZE, int LENGTH_SIZE>
void HashFunctionInterface<BLOCK_SIZE, LENGTH_SIZE>::finalize()
{
	this->lastBlock[this->lastBlockOffset] = 0x80;
	this->lastBlockOffset += 1;

	if (this->lastBlockOffset > BLOCK_LENGTH - LENGTH_MESSAGE - 1)
	{
		::memset(this->lastBlock + this->lastBlockOffset,
			0, BLOCK_LENGTH - this->lastBlockOffset);
		this->transformBlock(this->lastBlock);
		this->lastBlockOffset = 0;
	}

	::memset(this->lastBlock + this->lastBlockOffset,
		0, BLOCK_LENGTH - LENGTH_MESSAGE - this->lastBlockOffset);

	this->addLengthMessage();
	this->transformBlock(this->lastBlock);
}


template <int BLOCK_SIZE, int LENGTH_SIZE>
size_t HashFunctionInterface<BLOCK_SIZE, LENGTH_SIZE>::
	transformPreviousBlock(const uint8_t *data, size_t length)
{
	if (this->lastBlockOffset != 0)
	{
		size_t fillPreviousBlockLength =
			(size_t)BLOCK_LENGTH - this->lastBlockOffset;

		if (length - fillPreviousBlockLength >= 0)
		{
			::memcpy(this->lastBlock + this->lastBlockOffset,
				data, fillPreviousBlockLength);
			this->transformBlock(this->lastBlock);
			this->lastBlockOffset = 0;
			return fillPreviousBlockLength;
		}
		else
		{
			::memcpy(this->lastBlock + this->lastBlockOffset, data, length);
			this->lastBlockOffset += length;
			return length;
		}
	}

	return 0;
}


template <int BLOCK_SIZE, int LENGTH_SIZE>
void HashFunctionInterface<BLOCK_SIZE, LENGTH_SIZE>::
	addLengthMessageLittleEndian(uint64_t length, uint8_t *block)
{
	uint64_t lengthBits = length * 8;
	uint8_t *p = block + BLOCK_LENGTH - LENGTH_MESSAGE;

	for (int i = LENGTH_MESSAGE; i > 0; --i)
	{
		*p++ = (uint8_t)lengthBits;
		lengthBits >>= 8;
	}
}


template <int BLOCK_SIZE, int LENGTH_SIZE>
void HashFunctionInterface<BLOCK_SIZE, LENGTH_SIZE>::
	addLengthMessageBigEndian(uint64_t length, uint8_t *block)
{
	uint64_t lengthBits = length * 8;
	uint8_t *p = block + BLOCK_LENGTH - 1;

	for (int i = LENGTH_MESSAGE; i > 0; --i)
	{
		*p-- = (uint8_t)lengthBits;
		lengthBits >>= 8;
	}
}

#endif
