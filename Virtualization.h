#ifndef _VIRTUALIZATION_H
#define _VIRTUALIZATION_H

#include "MemoryAlloc.h"
#include "PTE.h"
#include "ClockStructure.h"
#include "Pagin.h"


#define PAGE_SIZE 4096
#define VIRTUAL_MEMORY_SIZE  4194304
#define PHYSICAL_MEMORY_SIZE 131072
#define PAGESBLOCKCOUNT      32
#define GETFIRST10BYTES      4190208
#define GETLAST12BYTES       4095
#define CORRECTOFFSET        262143
#define SETALLBLOCKSFREE     4294967295
#define NONFREEPHYSICALMEMORY -1


typedef void* PhysicalMemory;

extern PhysicalMemory UsableMemory;
extern PhysicalMemory ZeroPtr;

PhysicalMemory UsableMemory; 
PhysicalMemory ZeroPtr;
size_t         PhysicalBlocksBitMap;

#include "Insturments.h"

void CreatePhysicalMemory()
{
	UsableMemory = malloc(PHYSICAL_MEMORY_SIZE);
	ZeroPtr = UsableMemory;
	PhysicalBlocksBitMap = SETALLBLOCKSFREE;
	memset(UsableMemory, 0, PHYSICAL_MEMORY_SIZE);
}

class VirtualMemory : protected VirtualPagin, protected Instruments
{
protected:


	bool FindFreeVirtualPage(virtual_address& FirstPageNum)
	{
		size_t CurrentPage = 0;
		while (CurrentPage < PTE_SIZE && PTE.TableNote[CurrentPage].Reserved)
			CurrentPage++;
		if (CurrentPage == PTE_SIZE)
			return false;

		FirstPageNum = CurrentPage;
		PTE.TableNote[CurrentPage].Reserved = 1;

		return true;
	}
    


	int ReservePhysicalPageBlock()
	{
		for (int Offset = 0; Offset < PAGESBLOCKCOUNT;Offset++)
		{
			if (GetBit(PhysicalBlocksBitMap, Offset))
			{
				ChangeNBit(PhysicalBlocksBitMap, Offset);
				return Offset;
			}
		}

		return NONFREEPHYSICALMEMORY;
	}




	virtual_address ReserveVirtualPages(size_t PagesCount, bool& Successful)
	{
		virtual_address PageBeginAddress;
		size_t PageNumber; 
		size_t ReserveCount = 0;
		if (!FindFreeVirtualPage(PageNumber))
			return 0;

		PagesCount--;
		ReserveCount++;

		PageBeginAddress = PageNumber * PAGE_SIZE;
		while (PagesCount > 0)
		{
			PageNumber++;
			if (PTE.TableNote[PageNumber].Reserved)
			{
				VirtualPageTurnBack(PageBeginAddress, (ReserveCount - 1) * PAGE_SIZE);
				return 0;
			}
			PTE.TableNote[PageNumber].Reserved = 1;
			PagesCount--;
			ReserveCount++;
		}
		Successful = true;
		return PageBeginAddress;
	}




	//This is for uploading virtual memory after Free procedure, if we free full pages

	void ClearPhysicalArea(int PageBlock)
	{
		memset((char*)ZeroPtr + PageBlock * PAGE_SIZE, 0, PAGE_SIZE);
	}

	void SetPageBlockFree(size_t BlockNum)
	{
		size_t FreeRadix = 1 << BlockNum;
		PhysicalBlocksBitMap |= FreeRadix;
	}

	void SetPageDefault(size_t PageNum)
	{
		PTE.TableNote[PageNum].Handing = 0;
		PTE.TableNote[PageNum].Modification = 0;
		PTE.TableNote[PageNum].Reserved = 0;
		SetPageBlockFree(PTE.TableNote[PageNum].nPageBlock);
		PTE.TableNote[PageNum].nPageBlock = 0;
	}


	size_t UploadPageFromActives(size_t PageNum)
	{
		size_t FreePageBlock = 5;
		if (PTE.TableNote[PageNum].isLoad)
		{
			FreePageBlock = RemoveActivePage(PageNum);
		}
		PTE.TableNote[PageNum].isLoad = 0;

		return FreePageBlock;
	}


	void VirtualPageTurnBack(virtual_address PageAddress, size_t BlockSize)
	{
		size_t pages_count = BlockSize / PAGE_SIZE;
		while (pages_count > 0)
		{
			size_t PageNum = (PageAddress & GETFIRST10BYTES) >> 12;
			SetPageDefault(PageNum);
			UploadPageFromActives(PageNum);
			PageAddress += PAGE_SIZE;
			pages_count--;
			std::cout << "- - - - - -> We return virtual page number " << PageNum << " to system\n";
		}
	}

	
	// Next procedures working with writting to Virtual Memory 


	size_t VirtualPageReplacement()
	{
		size_t PageReplace = FindOldPage();
		WritePageOnDisk(PageReplace);
		size_t FreePageBlock = UploadPageFromActives(PageReplace);

		//set default parameters
		PTE.TableNote[PageReplace].Modification = 0;
		PTE.TableNote[PageReplace].nPageBlock = 0;
		return FreePageBlock;
	}


	bool CheckVirtualPageInMemory(virtual_address CurrentPage)
	{
		size_t PageNum = (CurrentPage & GETFIRST10BYTES) >> 12;
		if (!PTE.TableNote[PageNum].isLoad)
			return false;
		return true;
	}


	void LoadPageInMemory(size_t PageNum, size_t PageBlockNum)
	{
		PTE.TableNote[PageNum].Handing = 1;
		PTE.TableNote[PageNum].isLoad = 1;
		PTE.TableNote[PageNum].nPageBlock = PageBlockNum;
		AddActivePage(&PTE.TableNote[PageNum], PageNum);
	}


	void SetFlagModificate(size_t PageNum)
	{
		PTE.TableNote[PageNum].Modification = 1;
	}

	void PageFault(virtual_address PageAddress, char Attribute)
	{
		size_t PageNum = (PageAddress & GETFIRST10BYTES) >> 12;
		int PBReserved = ReservePhysicalPageBlock();

		if (PBReserved == NONFREEPHYSICALMEMORY)
		{
			PBReserved = VirtualPageReplacement();
		}
		LoadPageInMemory(PageNum, PBReserved);
		ClearPhysicalArea(PBReserved);
		if(Attribute == 'w')
			SetFlagModificate(PageNum);
	}
	
};









class Process : protected VirtualMemory
{
	virtual_address ProcData[128];
	size_t          DataUsesMemory[128];
	Allocator       FAllocator;
public:

	Process()
	{
		for (int i = 0; i < 128; i++)
			DataUsesMemory[i] = 0;
	}

	void ProcessAddMemory(size_t MemorySize, unsigned int Variable);
	void ProcessFreeMemory(unsigned int Variable);
	void CheckPageFree(virtual_address wasFree);
	void ProcessWriteData(unsigned int Variable, const char* Data);
	void ProcessReadData(unsigned int Variable);
	void wLoadPagesInMemory(virtual_address CurrentPage, size_t PageCount, const char* Data);              //Load and Write
	void rLoadPagesInMemory(virtual_address CurrentPage, size_t PageCount);                                //Load and Read


	unsigned int _WriteToPage(virtual_address CurrentPage, const char* Data, unsigned int Offset);        //this function will be used when page will be in MEMORY

	bool _ReadFromPage(virtual_address CurrentPage);
	bool GetVirtualPages(size_t MemorySize);

	size_t GetVirtualPagesCount(size_t MemorySize);

	virtual_address _Malloc(size_t MemorySize, size_t Variable);
};





void Process::wLoadPagesInMemory(virtual_address CurrentPage, size_t PageCount, const char* Data)
{
	unsigned int Offset = 0;

	do
	{
		if (!CheckVirtualPageInMemory(CurrentPage))
		{
			PageFault(CurrentPage,'w');
		}
		Offset = _WriteToPage(CurrentPage, Data, Offset);
		CurrentPage += PAGE_SIZE;
		PageCount--;
	} while (Offset && PageCount);
}

void Process::rLoadPagesInMemory(virtual_address CurrentPage, size_t PageCount)
{
	bool Reading = true;
	while (PageCount > 0 && Reading)
	{
		if (!CheckVirtualPageInMemory(CurrentPage))
		{
			PageFault(CurrentPage,'r');
			ReadFromDisk(CurrentPage);
		}
		Reading = _ReadFromPage(CurrentPage);
		CurrentPage += PAGE_SIZE;
		PageCount--;
	}
}


void Process::CheckPageFree(virtual_address wasFree)
{
	size_t BlockPow = 12;
	for (; BlockPow <= RADIX_COUNT; BlockPow++)
	{
		size_t BlockSize = 1 << BlockPow;
		if (FAllocator.isFree(wasFree, BlockSize, true))
		{
			std::cout << "- - - - - - > We found free pages with total size " << BlockSize << std::endl;
			VirtualPageTurnBack(wasFree, BlockSize);
			break;
		}
	}
}


size_t Process::GetVirtualPagesCount(size_t MemorySize)
{
	return ((FAllocator.CorrectMemorySize(MemorySize) - 1) / PAGE_SIZE) + 1;
}


void Process::ProcessAddMemory(size_t MemorySize, unsigned int Variable)
{
	ProcData[Variable] = _Malloc(MemorySize, Variable);
}

void Process::ProcessFreeMemory(unsigned int Variable)
{
	virtual_address wasFree = FAllocator._Free(ProcData[Variable], DataUsesMemory[Variable]);
	ProcData[Variable] = 0;
	DataUsesMemory[Variable] = 0;
	CheckPageFree(wasFree);
	//FAllocator.ShowFreeBlocksStatistics(); system("pause");
}


bool Process::GetVirtualPages(size_t MemorySize)
{
	unsigned int pages_count = GetVirtualPagesCount(MemorySize);
	bool SuccessfulReservation = false;
	virtual_address NewBlockBeginPtr = ReserveVirtualPages(pages_count, SuccessfulReservation);

	if (!SuccessfulReservation)
		return false;

	std::cout << "- - - - - - > " << pages_count << "pages with size " << PAGE_SIZE << " has been given for allocator\n";
	FAllocator.AddMemoryBlock(NewBlockBeginPtr, pages_count * PAGE_SIZE);
	return true;
}


virtual_address Process::_Malloc(size_t MemorySize, unsigned int Variable)
{
	if (FAllocator.exceptionAllocatorNoSuchBlock(MemorySize))
	{
		if (!GetVirtualPages(MemorySize))
		{
			std::cout << "Error, there isn't free pages for you allocator\n";
			return 0;
		}
	}

	DataUsesMemory[Variable] = MemorySize;

	return FAllocator._Malloc(MemorySize);
}


void Process::ProcessWriteData(unsigned int Variable, const char* Data)
{
	if (!DataUsesMemory[Variable])
	{
		std::cout << "This variable isn't using in this process\n";
		return;
	}

	size_t VariablePagesCount = GetVirtualPagesCount(DataUsesMemory[Variable]);
	wLoadPagesInMemory(ProcData[Variable], VariablePagesCount, Data);

	//вроде сделал запись в переменную, осталось обращения для чтения. проверить все еще раз.
}


unsigned int Process::_WriteToPage(virtual_address CurrentPage, const char* Data, unsigned int Offset)
{
	int CurPos = Offset;
	for (int PosInPage = 0; PosInPage < PAGE_SIZE; CurPos++, PosInPage++, CurrentPage++)
	{
		char* ptr = (char*)Virtual2Physical(CurrentPage);
		*ptr = Data[CurPos];
		if (CurPos == strlen(Data) - 1)
		{
			return 0;
		}
	}
	return CurPos;
}


void Process::ProcessReadData(unsigned int Variable)
{
	if (!DataUsesMemory[Variable])
	{
		std::cout << "This variable is not usable now\n";
		return;
	}

	size_t VariablePagesCount = GetVirtualPagesCount(DataUsesMemory[Variable]);
	rLoadPagesInMemory(ProcData[Variable], VariablePagesCount);
}


bool Process::_ReadFromPage(virtual_address CurrentPage)
{
	for (int CurPos = 0; CurPos < PAGE_SIZE; CurPos++, CurrentPage++)
	{
		char ptr = *(char*)Virtual2Physical(CurrentPage);
		if (ptr == 0) return false;
		std::cout << ptr;
	}
	return true;
}


#endif
