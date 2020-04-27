#ifndef PhysicalMemoryH
#define PhysicalMemoryH

#include <Windows.h>


typedef struct
{
	 HANDLE hPhysicalMemFile, hMemFileMapping;

	 LPVOID pPhysicalMem;

} Physical_Memory, *PPhysical_Memory;

PPhysical_Memory Physical_Memory_Create();
void Physical_Memory_Destroy(PPhysical_Memory object);

LPVOID Physical_Memory_ReadMem(PPhysical_Memory self, UINT16 address);
void Physical_Memory_WriteMem(PPhysical_Memory self, UINT16 address, LPCVOID pBufferToWrite, BYTE length);




#endif
