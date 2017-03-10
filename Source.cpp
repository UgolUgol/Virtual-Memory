#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <bitset>
#include "MemoryAlloc.h"
#include "Virtualization.h"
#include "ClockStructure.h"

using namespace std;

#define PROCESSLIMIT 100

void Menu()
{
	cout << "C - Create Process\n";
	cout << "V - Create new variable\n";
	cout << "F - Free variable\n";
	cout << "W - Write to variable\n";
	cout << "R - Read from varaible\n";
	cout << "- - - - > ";
}

struct ProcessSetting
{
	Process process[PROCESSLIMIT];
	unsigned int psize;

	ProcessSetting()
	{
		psize = 0;
	}
};


int main()
{
	CreatePhysicalMemory();
	ProcessSetting set;
	bool Working = true;
	unsigned char key;

	while (Working)
	{
		//Menu();
		cin >> key;
		switch (key)
		{
		case 'C':
		{
					set.psize += 1;
					//std::cout << "- - - - - > Correctly" << std::endl;
					break;
		}
		case 'V':
		{
					unsigned int PID;
					//std::cout << "What process ? -> " << std::endl;
					std::cin >> PID;
					if (PID > set.psize)
					{
						//std::cout << "- - - - - - > Uncorrectly\n";
					}
					else
					{
						unsigned int VID;
						size_t MemorySize;
						//std::cout << "Value number(0 - 127) : \n";
						//std::cout << "- - - - - - > ";
						std::cin >> VID;
						//std::cout << "What size of memory ? \n";
						//std::cout << "- - - - - - > ";
						std::cin >> MemorySize;
						set.process[PID - 1].ProcessAddMemory(MemorySize, VID);
					}
					break;
           }
		case 'F':
		{
					unsigned int PID;
					//std::cout << "What process ? -> ";
					std::cin >> PID;
					if (PID > set.psize)
					{
						//std::cout << "- - - - - - > Uncorrectly\n";
					}
					else
					{
						unsigned int VID;
						size_t MemorySize;
						//std::cout << "Value number(0 - 127) : \n";
						//std::cout << "- - - - - - > ";
						std::cin >> VID;
						set.process[PID - 1].ProcessFreeMemory(VID);
					}
					break;
		}
		case 'W':
		{
					unsigned int PID;
					//std::cout << "What process ? -> ";
					std::cin >> PID;
					if (PID > set.psize)
					{
						//std::cout << "- - - - - - > Uncorrectly\n";
					}
					else
					{
						unsigned int VID;
						size_t DataSize;
						//std::cout << " Variable number ? -> ";
						std::cin >> VID;
						//std::cout << "Data size ? ->";
						std::cin >> DataSize;
						char* Data = (char*)malloc(sizeof(char)* DataSize);
						//std::cout << "Enter data : \n";
						cin >> Data;
						set.process[PID - 1].ProcessWriteData(VID, Data);
						free(Data);
					}
					break;
		}
		case 'R':
		{
					unsigned int PID;
					//std::cout << "What process ? -> ";
					std::cin >> PID;
					if (PID > set.psize)
					{
						//std::cout << "- - - - - - > Uncorrectly\n";
					}
					else
					{
						unsigned int VID;
						//std::cout << "Variable number ? -> ";
						std::cin >> VID;
						set.process[PID - 1].ProcessReadData(VID);
						std::cout << std::endl<<std::endl;
					}
					break;
		}
		case 'E':
			return 0;
	}
		//system("pause");
		//system("cls");
	}


	return 0;
}