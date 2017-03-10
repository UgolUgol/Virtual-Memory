#ifndef _MEMORY_ALLOC_H
#define _MEMORY_ALLOC_H

#include <iostream>
#include "FreeBlockList.h"
#include <cmath>


class Allocator
{
	FreeBlockList _FreeBlocks[RADIX_COUNT + 1];
public:
	Allocator() {};

	size_t CorrectMemorySize(size_t MemorySize);
	size_t FindNextBlock(size_t BlockSize);
	
	unsigned int GetPow(size_t _);

	bool            isFree(virtual_address CheckingBlock, size_t BlockSize, bool AngryMod);
	bool            isPowerOfTwo(size_t Block_size);
	bool            exceptionAllocatorNoSuchBlock(size_t MemorySize, bool AngryMod);

	virtual_address _Malloc(size_t MemorySize);
	virtual_address Min(virtual_address First, virtual_address Second);
	virtual_address MergeCurrentBlocks(virtual_address First, virtual_address Second, size_t BlockSize);
	virtual_address FindBrotherBlock(virtual_address First, size_t BlockSize);
	virtual_address GiveBlockAddress(size_t BlockSize);
	virtual_address _Free(virtual_address MustBeFreePtr, size_t BlockSize);


	void            AddMemoryBlock(size_t VBAddress, size_t BlockSize);
	void            SplitCurrentBlock(size_t BlockSize);
	void            ShowFreeBlocksStatistics();
};




virtual_address Allocator::GiveBlockAddress(size_t BlockSize)
{
	return _FreeBlocks[BlockSize].PopFreeBlock(0);
}


bool Allocator::isPowerOfTwo(size_t BlockSize)
{
	if (!(BlockSize & (BlockSize - 1)))
		return true;
	else
		return false;
}

virtual_address Allocator::Min(virtual_address First, virtual_address Second)
{
   return First > Second ? Second : First;
}


size_t Allocator::CorrectMemorySize(size_t MemorySize)
{
	if (!(MemorySize & (MemorySize - 1)))
		return MemorySize;
	while (MemorySize & (MemorySize - 1))
		MemorySize = MemorySize & (MemorySize - 1);
	return MemorySize << 1;
}

unsigned int Allocator::GetPow(size_t _)
{
	_ -= 1;
	unsigned int Pow = 0;
	while (_)
	{
		_ >>= 1;
		Pow++;
	}
	return Pow;
}

size_t Allocator::FindNextBlock(size_t BlockSize)
{
	while (!_FreeBlocks[BlockSize].FreeBlocksCount())
	{
		BlockSize++;
	}

	return BlockSize;
}



void Allocator::SplitCurrentBlock(size_t BlockSize)
{
	virtual_address First = _FreeBlocks[BlockSize].PopFreeBlock(0);
	virtual_address Second = pow(2, BlockSize) / 2 + First;
	_FreeBlocks[BlockSize - 1].PushFreeBlock(Second);
	_FreeBlocks[BlockSize - 1].PushFreeBlock(First);
}


virtual_address Allocator::MergeCurrentBlocks(virtual_address First, virtual_address Second, size_t BlockSize)
{
	unsigned int SecondBlockPow = GetPow(BlockSize);
	for (int i = 0;; i++)
	{
		if (Second == _FreeBlocks[SecondBlockPow].ShowAddress(i))
		{
			_FreeBlocks[SecondBlockPow].PopFreeBlock(i);
			break;
		}
	}
	
	_FreeBlocks[SecondBlockPow + 1].PushFreeBlock(Min(First, Second));

	return Min(First, Second);
}

virtual_address Allocator::_Malloc(size_t MemorySize)
{
	MemorySize = CorrectMemorySize(MemorySize);
	unsigned int CurrentPow = GetPow(MemorySize);
	size_t NextBlock;

	while ((NextBlock = FindNextBlock(CurrentPow) ) != CurrentPow)
	{
		SplitCurrentBlock(NextBlock);
	}
	std::cout << "- - - - - - > " << MemorySize << " has been allocated\n";
	return _FreeBlocks[CurrentPow].PopFreeBlock(0);
}



virtual_address Allocator::FindBrotherBlock(virtual_address First, size_t BlockSize)
{
	size_t NextBiggerBlock = CorrectMemorySize(BlockSize + 1);
	if ((First & (NextBiggerBlock - 1)) == 0)
		return First + BlockSize;
	else if ((First & (NextBiggerBlock - 1)) == BlockSize)
		return First - BlockSize;

}




bool Allocator::isFree(virtual_address CheckingBlock, size_t BlockSize, bool AngryMod = false)
{
	unsigned int CurrentBlockPow = GetPow(BlockSize);
	for (int i = 0; i < _FreeBlocks[CurrentBlockPow].FreeBlocksCount(); i++)
	{
		if (CheckingBlock == _FreeBlocks[CurrentBlockPow].ShowAddress(i))
		{
			if (AngryMod) _FreeBlocks[CurrentBlockPow].PopFreeBlock(i);                           //if AngryMod, we will delete this block from frees
			return true;
		}
	}

	return false;
}





virtual_address Allocator::_Free(virtual_address MustBeFreePtr, size_t BlockSize)
{
	size_t CurrentPow = GetPow(BlockSize);
	BlockSize = CorrectMemorySize(BlockSize);
	virtual_address BrotherPtr = FindBrotherBlock(MustBeFreePtr, BlockSize); 

	while (isFree(BrotherPtr, BlockSize))
	{
		MergeCurrentBlocks(MustBeFreePtr, BrotherPtr, BlockSize);
		BlockSize = CorrectMemorySize(BlockSize + 1);
		CurrentPow = GetPow(BlockSize);
		MustBeFreePtr = _FreeBlocks[CurrentPow].PopFreeBlock(0);
		BrotherPtr = FindBrotherBlock(MustBeFreePtr, BlockSize);
    }
	std::cout << "- - - - - - > Block with size " << BlockSize << " has been freed\n";
	_FreeBlocks[CurrentPow].PushFreeBlock(MustBeFreePtr);
	return MustBeFreePtr;
}


void Allocator::ShowFreeBlocksStatistics()
{
	for (int i = 0; i <= 22; i++)
		std::cout << pow(2, i) << " blocks " << _FreeBlocks[i].FreeBlocksCount() << std::endl;
	std::cout << std::endl;
}

bool Allocator::exceptionAllocatorNoSuchBlock(size_t MemorySize, bool AngryMod = false)
{
	MemorySize = CorrectMemorySize(MemorySize);
	unsigned int CurrentPow = GetPow(MemorySize);
	while (CurrentPow <= RADIX_COUNT)
	{
		if (_FreeBlocks[CurrentPow].FreeBlocksCount() )
			break;
		CurrentPow++;

		if (AngryMod)
		{
			CurrentPow = RADIX_COUNT + 1;
			break;
		}
	}

	if (CurrentPow > RADIX_COUNT)
		return true;
	else
		return false;
}



void Allocator::AddMemoryBlock(size_t VBAddress, size_t BlockSize)
{
	size_t BlockRadix = GetPow(BlockSize);
	_FreeBlocks[BlockRadix].PushFreeBlock(VBAddress);
}

#endif