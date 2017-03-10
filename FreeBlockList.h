#ifndef _FREE_BLIST_H
#define _FREE_BLIST_H

#define RADIX_COUNT 22

typedef size_t virtual_address;

struct ListItem
{
	virtual_address BeginPtr;
	ListItem* Next;

	ListItem()
	{
		this->BeginPtr = -1;
		this->Next = nullptr;
	}
};



class FreeBlockList
{
	ListItem* Head;
	size_t FreeBlockCount;
public:
	FreeBlockList()
	{
		this->FreeBlockCount = 0;
		Head = nullptr;
	}

	bool PushFreeBlock(virtual_address BlockPtr)
	{
		ListItem* Tail = Head;
		Head = new ListItem();
		Head->Next = Tail;
		Head->BeginPtr = BlockPtr;
		this->FreeBlockCount++;
		return true;
	}

	virtual_address PopFreeBlock(size_t BlockNumber)
	{
		ListItem* Restruct = this->Head;
		ListItem* ListRubbish;
		virtual_address PopBlock;
		this->FreeBlockCount--;
		if (BlockNumber)
		{
			for (int i = 0; i < BlockNumber - 1; i++)
			{
				Restruct = Restruct->Next;
			}

			PopBlock = Restruct->Next->BeginPtr;
			ListRubbish = Restruct->Next;
			Restruct->Next = Restruct->Next->Next;
			delete ListRubbish;
			return PopBlock;
		}

		else
		{
			ListRubbish = this->Head;
			PopBlock = this->Head->BeginPtr;
			this->Head = this->Head->Next;
			delete ListRubbish;
			return PopBlock;
		}
	}

	virtual_address ShowAddress(size_t BlockNumber)
	{
		ListItem* Current = this->Head;
		for (int i = 0; i < BlockNumber; i++)
		{
			Current = Current->Next;
		}
		return Current->BeginPtr;
	}

	size_t FreeBlocksCount()
	{
		return this->FreeBlockCount;
	}
};


#endif