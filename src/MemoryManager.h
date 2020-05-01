#ifndef MemoryManagerH
#define MemoryManagerH

#include "PageTable.h"
#include "PhysicalMemory.h"

#define VIRTUAL_MEMORY_UPPER_SIZE 64 * 1024
#define PAGE_TABLE_SIZE 16

typedef struct {

	bool IsPhysicalPageFree[PHYSICAL_MEMORY_SIZE / PAGE_SIZE];
	PPageTableObject pPageTable;
	PPhysical_Memory pPhysicalMemory;
	HANDLE hPageFile;


} MemManager, *PMemManager;

typedef bool (*ReadOrWrite_Fp)(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);

PMemManager MemoryManager_Create();
void MemoryManager_Destroy(PMemManager object);

bool MemoryManager_ReadMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);
bool MemoryManager_WriteMem(PMemManager self, UINT16 address,  UCHAR length, LPVOID pBuffer);

#endif
