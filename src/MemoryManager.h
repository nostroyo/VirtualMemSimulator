#ifndef MemoryManagerH
#define MemoryManagerH

#include "PageTable.h"
#include "PhysicalMemory.h"

#define VIRTUAL_MEMORY_UPPER_SIZE 64 * 1024
#define PAGE_TABLE_SIZE 16
#define PAGE_SIZE 4 * 1024

typedef struct {

	PPageTableObject pPageTable;
	PPhysical_Memory pPhysicalMemory;
} MemManager, *PMemManager;

typedef BOOL (*ReadOrWrite_Fp)(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);

PMemManager MemoryManager_Create();
void MemoryManager_Destroy(PMemManager object);

BOOL MemoryManager_ReadMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);
BOOL MemoryManager_WriteMem(PMemManager self, UINT16 address,  UCHAR length, LPVOID pBuffer);


#endif
