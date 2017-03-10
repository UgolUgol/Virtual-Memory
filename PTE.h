#ifndef _PTE_H
#define _PTE_H

#define PTE_SIZE 1024

struct Note
{
	int nPageBlock;

	bool isLoad;
	bool Handing;
	bool Reserved;
	bool Modification;
};

struct PageTable
{
	Note TableNote[PTE_SIZE];

	PageTable()
	{
		for (int i = 0; i <= PTE_SIZE; i++)
		{
			TableNote[i].isLoad = 0;
			TableNote[i].Handing = 0;
			TableNote[i].Modification = 0;
			TableNote[i].Reserved = 0;
		}
	}
};

#endif