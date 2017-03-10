#ifndef _INSTRUMENTS_H
#define _INSTRUMENSTS_H

#include <fstream>
#include <cstdlib>



// in last version PTE and Virtual2Physical was in Virtualization.h 

class Instruments
{
protected:
	PageTable PTE;

	bool GetBit(size_t Number, size_t Offset)
	{
		size_t Concat = 1 << Offset;
		size_t Bit = Number & Concat;
		if (!Bit)
			return false;
		return true;
	}

	void ChangeNBit(size_t& Num, size_t Offset)
	{
		Num ^= (1 << Offset);
	}


	void* Virtual2Physical(virtual_address Translit)
	{
		size_t PageNumber = (Translit & GETFIRST10BYTES) >> 12;
		if (!PTE.TableNote[PageNumber].isLoad)
		{
			std::cout << "This page is not is RAM\n";
			return 0;
		}
		size_t AddressPrefix = PTE.TableNote[PageNumber].nPageBlock << 12;
		size_t Offset = (Translit & GETLAST12BYTES);
		size_t FullAddress = (AddressPrefix | Offset) + (size_t)ZeroPtr;

		return (void*)FullAddress;
	}

	void WritePageOnDisk(size_t PageNum)
	{
		if (PTE.TableNote[PageNum].Modification)
		{
			char cache[10];
			char Path[30] = { "Swap/Page" };
			_itoa(PageNum, cache, 10);
			strcat(Path, cache);
			std::ofstream ofs(Path);

			virtual_address Begin = PAGE_SIZE * PageNum;
			while (Begin < PAGE_SIZE * (PageNum + 1))
			{
				char ptr = *(char*)Virtual2Physical(Begin);
				void* Remover = Virtual2Physical(Begin);
				*(char*)Remover = 0;
				ofs << ptr;
				Begin++;
			}
			ofs.close();
		}
	}


	void ReadFromDisk(virtual_address CurrentPage)
	{
		size_t PageNum = (CurrentPage & GETFIRST10BYTES) >> 12;
		char Path[30] = { "Swap/Page" };
		char cache[10];
		_itoa(PageNum, cache, 10);
		strcat(Path, cache);
		
		std::ifstream ifs(Path);
		virtual_address Begin = PAGE_SIZE * PageNum;
		while (Begin < PAGE_SIZE * (PageNum + 1))
		{
			void* Save = Virtual2Physical(Begin);
			char ptr;
			ifs >> ptr;
			*(char*)Save = ptr;
			Begin++;
		}
		ifs.close();
	}
};


#endif