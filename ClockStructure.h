#ifndef _CLOCK_STRUCTURE_H
#define _CLOCK_STRUCTURE_H

#include "PTE.h"


struct OneForkedItem
{
	Note* PagePtr;
	size_t PageNumber;
	OneForkedItem* next;

	OneForkedItem()
	{
		PagePtr = 0;
		next = nullptr;
	}
};

class ClockStructure
{
public:
	OneForkedItem* Arrow;
	OneForkedItem* Head;
	size_t         ActiveVirtualPagesCount;

	ClockStructure()
	{
		this->Arrow = nullptr;
		this->ActiveVirtualPagesCount = 0;
	}

	void PushNewPage(Note* NewPagePtr, size_t PageNum);
	void ShowAllActivePages();

	size_t DeleteActivePage(size_t PageNum);              //Delete Virtual Page from ActiveList and returning it Physical Block
	size_t ClockFind();

	Note* GetPage(size_t PageNumber);
};



size_t ClockStructure::ClockFind()
{
	OneForkedItem* Begin = Arrow;
	while (!Begin->PagePtr->Handing)
	{
		Begin->PagePtr->Handing = 0;
		Begin = Begin->next;
		Arrow = Begin->next;
	}
	return Begin->PageNumber;
}


void ClockStructure::ShowAllActivePages()
{
	OneForkedItem Beg = *Head;
	for (int i = 0; i < this->ActiveVirtualPagesCount; i++)
	{
		std::cout << Beg.PageNumber << " ";
		Beg = *(Beg.next);
	}
	std::cout<<std::endl;
};


size_t ClockStructure::DeleteActivePage(size_t PageNum)
{
	OneForkedItem* Begin = this->Head;
	size_t PageBlockNum = 0;

	for (int i = 0; i < this->ActiveVirtualPagesCount; i++)
	{
		if (Begin->next->PageNumber == PageNum)
		{
			OneForkedItem* Deleting = Begin->next;
			Begin->next = Deleting->next;
			if (Deleting == Head)
			{
				this->Head = Deleting->next;
				this->Arrow = Deleting->next;
			}
			if (Deleting == Arrow)
			{
				this->Arrow = Deleting->next;
			}
			this->ActiveVirtualPagesCount--;
			PageBlockNum = Deleting->PagePtr->nPageBlock;
			free(Deleting);
			break;
		}
		Begin = Begin->next;
	}

	return PageBlockNum;
}



void ClockStructure::PushNewPage(Note* NewPagePtr, size_t PageNum)
{
	if (!this->ActiveVirtualPagesCount)
	{
		this->Head = new OneForkedItem;
		this->Head->PagePtr = NewPagePtr;
		this->Head->PageNumber = PageNum;
		this->Head->next = Head;
		this->Arrow = this->Head;
		this->ActiveVirtualPagesCount++;
		return;
	}

	else
	{
		int LastPage = 0;
		OneForkedItem* Current = this->Head;
		while (LastPage < this->ActiveVirtualPagesCount - 1)
		{
			Current = Current->next;
			LastPage++;
		}
		OneForkedItem* Pushing = new OneForkedItem;
		Pushing->PagePtr = NewPagePtr;
		Pushing->PageNumber = PageNum;
		Pushing->next = this->Head;
		Current->next = Pushing;
		this->ActiveVirtualPagesCount++;
	}
}



Note* ClockStructure::GetPage(size_t PageNumber)
{
	OneForkedItem* Begin = this->Head;
	bool NoSuchPage = false;

	while (Begin->PageNumber != PageNumber)
	{
		Begin = Begin->next;
		if (Begin == this->Head)
		{
			NoSuchPage = true;
			break;
		}
	}

	if (!NoSuchPage)
	{
		return Begin->PagePtr;
	}

	return nullptr;
};


#endif