#ifndef _OBJECTPOOL_HPP_
#define _OBJECTPOOL_HPP_

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <new>

#include "pattern/NonCopyable.hpp"


#ifdef max
#undef max
#endif


template <typename T>
class ObjectPool : private NonCopyable
{
	typedef struct BlockHeader
	{
		BlockHeader() : next(nullptr) {}
		struct BlockHeader *next;
	} BlockHeader;

	typedef struct TopHeader
	{
		TopHeader(size_t chunkNum) :
			chunkNum(chunkNum),
			chunkSize(sizeof(T) >= sizeof(char *) ? sizeof(T) : sizeof(char *)),
			blockSize(chunkNum * chunkSize + sizeof(BlockHeader)),
			firstFreeAddr(nullptr), lastFreeAddr(nullptr),
			block(nullptr) {}

		size_t chunkNum, chunkSize;
		size_t blockSize;
		void *firstFreeAddr, *lastFreeAddr;
		BlockHeader *block;
	} TopHeader;

public:
	typedef T value_type;
	typedef T * pointer;
	typedef pointer Ptr;
	typedef const T * const_pointer;
	typedef const_pointer ConstPtr;
	typedef T & reference;
	typedef reference Ref;
	typedef const T & const_reference;
	typedef const_reference ConstRef;
	typedef size_t size_type;
	typedef size_type SizeType;
	typedef std::ptrdiff_t difference_type;

	template <class U>
	struct rebind { typedef ObjectPool<U> other; };

	ObjectPool(size_t chunkNum = 32) : topHeader(chunkNum) {}
	ObjectPool(const ObjectPool &o) : topHeader(o.getChunks()) {}
	template <class U>
	ObjectPool(const ObjectPool<U> &o) : topHeader(o.getChunks()) {}

	~ObjectPool() { deleteAllBlocks(); }

	Ptr address(Ref value) const { return &value; }
	ConstPtr address (ConstRef value) const { return &value; }

	SizeType max_size() const
	{ return std::numeric_limits<size_t>::max() / sizeof(T); }

	void construct(Ptr ptr, const T &value)
	{
		// LOGP("construct " << ptr);
		new ((void *)ptr) T(value);
	}

	Ptr allocate(SizeType num, const void * = 0)
	{
		if (num == 1)
		{
			// All blocks are full or hasn't allocate
			if (topHeader.firstFreeAddr == nullptr)
				initializeNewBlock();

			Ptr ptr = (T *)topHeader.firstFreeAddr;
			topHeader.firstFreeAddr = getNextFreePointer();
			// LOGP("allocate[1]: " << ptr);

			return ptr;
		}
		else
		{
			Ptr ptr = initializeNewBlock(num);
			// LOGP("allocate[" << (int)num << "]: " << ptr);
			return ptr;
		}
	}

	void destroy(Ptr ptr)
	{
		// LOGP("destory: " << ptr);
		ptr->~T();
	}

	void deallocate(Ptr ptr, SizeType num)
	{
		// LOGP("free[" << num << "]: " << ptr);
		for (SizeType i = 0; i < num; ++i)
			appendFreePointer((void *)(ptr + i));
	}

	size_t getChunks() const { return this->topHeader.chunkNum; }
	size_t getChunkSize() const { return this->topHeader.chunkSize; }

private:
	void appendFreePointer(void *freePtr)
	{
		setEmbeddedPointer(topHeader.lastFreeAddr, freePtr);
		setEmbeddedPointer(freePtr, nullptr);
		topHeader.lastFreeAddr = freePtr;
	}

	char * allocateBlock(SizeType size) const
	{ return new (std::nothrow) char[size]; }

	void deallocateBlock(char *block) const
	{ delete [] block; }

	void initializeNewBlockFreeList()
	{
		void *ptr = topHeader.firstFreeAddr;

		for (size_t i = topHeader.chunkNum; i != 0; --i)
		{
			void *prevPtr = ptr;
			ptr = getOffsetPointer(ptr, topHeader.chunkSize);
			setEmbeddedPointer(prevPtr, ptr);
		}

		setEmbeddedPointer(ptr, nullptr);
		topHeader.lastFreeAddr = ptr;
	}

	void initializeNewBlock()
	{
		BlockHeader *oldBlock = topHeader.block;
		char *ptr = allocateBlock((SizeType)topHeader.blockSize);
		topHeader.block = reinterpret_cast<BlockHeader *>(ptr);
		topHeader.firstFreeAddr = (void *)(ptr + sizeof(BlockHeader));
		topHeader.block->next = oldBlock;
		initializeNewBlockFreeList();
	}

	Ptr initializeNewBlock(SizeType chunkNum)
	{
		BlockHeader *oldBlock = topHeader.block;
		size_t blockSize = chunkNum * topHeader.chunkSize + sizeof(BlockHeader);
		char *ptr = allocateBlock(blockSize);
		topHeader.block = reinterpret_cast<BlockHeader *>(ptr);
		topHeader.block->next = oldBlock;
		return reinterpret_cast<Ptr>(ptr + sizeof(BlockHeader));
	}

	void deleteAllBlocks()
	{
		BlockHeader *ptr = topHeader.block;

		while (ptr != nullptr)
		{
			char *deletePtr = reinterpret_cast<char *>(ptr);
			ptr = ptr->next;
			deallocateBlock(deletePtr);
		}
	}

	void * getNextFreePointer() const
	{ return *(void **)(topHeader.firstFreeAddr); }

	void setEmbeddedPointer(void *ptr, void *to)
	{ *(void **)(ptr) = to; }

	void * getOffsetPointer(void *basePtr, size_t offset) const
	{
		return reinterpret_cast<void *>(
			reinterpret_cast<intptr_t>(basePtr) + offset);
	}

	TopHeader topHeader;
};

#endif
