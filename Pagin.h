#ifndef _PAGIN_H
#define _PAGIN_H

#include "ClockStructure.h"

class VirtualPagin
{
protected:
	ClockStructure ActivePages;

	void AddActivePage(Note* NewPagePtr, int PageNum)
	{
		ActivePages.PushNewPage(NewPagePtr, PageNum);
	}

	size_t RemoveActivePage(size_t PageNum)
	{
		return ActivePages.DeleteActivePage(PageNum);
	}

	Note* ReturnActivePage(size_t PageNum)
	{
		return ActivePages.GetPage(PageNum);
	}

	size_t FindOldPage()
	{
		return ActivePages.ClockFind();
	}

};


#endif