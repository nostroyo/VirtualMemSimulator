//---------------------------------------------------------------------------

#include "MemoryManager.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>

#include "PageTable.h"
#include "PhysicalMemory.h"

static bool TranslateVirtualToPhysicalAddress(PMemManager self, UINT16 VirtualAddress, /*out*/ PUINT16 PhysicalAddress);
static bool WriteMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);
static bool ReadWriteHandleMultiplePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer, ReadOrWrite_Fp readOrWriteOnePageFunc);
static bool ReadMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);
// return true if at least one available, false otherwise
static bool GetFirstPhysicalPageAvailable(PPageTableObject self, /*out*/ unsigned char *pageIndex);

bool ReadWriteHandleMultiplePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer, ReadOrWrite_Fp readOrWriteOnePageFunc)
{
	UINT16 lAddress = address;
	UCHAR lLength = length;
	UINT8 *pUInt8Buffer = (UINT8 *) pBuffer;
	bool result = true;
	// Handle memory on 2 pages
	if ((address & 0xFFF) + length >= PAGE_SIZE)
	{
		UCHAR lengthFirstPage = PAGE_SIZE - (address & 0xFFF);
		result = readOrWriteOnePageFunc(self, address, lengthFirstPage, pBuffer);
		lAddress = address + lengthFirstPage;
		lLength = length - lengthFirstPage;
		pUInt8Buffer += lengthFirstPage;
	}
	if (result)
	{
		result = readOrWriteOnePageFunc(self, lAddress, lLength, (LPVOID) pUInt8Buffer);
	}

	return result;
}

bool TranslateVirtualToPhysicalAddress(PMemManager self, UINT16 VirtualAddress, /*out*/ PUINT16 PhysicalAddress)
{
		// only 4 MSB of address are index to page table
	UINT8 virtualPageIndex = VirtualAddress >> 12;

	PPageTableEntry pEntry = pageTable_GetEntry(self->pPageTable, virtualPageIndex);
	if (pEntry)
	{
		unsigned int physicalAdress;
		// Reconstruct adress
		if (pEntry->isInPhysicalMem) {
			unsigned int offset = VirtualAddress & 0xFFF;
			*PhysicalAddress = (pEntry->pageNumber << 12) | offset;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool GetFirstPhysicalPageAvailable(PMemManager self, /*out*/ unsigned char *pageIndex)
{
	bool result = false;

	for (unsigned char i = 0; i < (sizeof(self->IsPhysicalPageFree) / sizeof(self->IsPhysicalPageFree[0])); i++)
	{
		if (!self->pPageTable->pPageTable[i])
		{
			*pageIndex = i;
			result = true;
			break;
		}
	}
	return result;
}

bool WriteMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	UINT16 physicalAdress;

	if (TranslateVirtualToPhysicalAddress(self, address, &physicalAdress))
	{
		Physical_Memory_WriteMem(self->pPhysicalMemory, physicalAdress, pBuffer, length);
		return true;
	}
	else
	{
		// only 4 MSB of address are index to page table
		UINT8 virtualPageIndex = address>> 12;
		unsigned char physicalPageIndex = 0xFF;

		// handle full physical memory
		if (GetFirstPhysicalPageAvailable(self, &physicalPageIndex))
		{
			PPageTableEntry pEntry = (PPageTableEntry) malloc(sizeof(*pEntry));
			pEntry->pageNumber = physicalPageIndex;
			pEntry->isInPhysicalMem = true;
			pageTable_AddEntryAtIndex(self->pPageTable, virtualPageIndex, pEntry);
			self->IsPhysicalPageFree[physicalPageIndex] = false;
			return WriteMemOnOnePage(self, address, length, pBuffer);

		}
		else
		{

			// handle full physical memory with pageFile
		}
	}
}

bool ReadMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	bool readMemResult = false;
	UINT16 physicalAdress;
	if (TranslateVirtualToPhysicalAddress(self, address, &physicalAdress))
	{
		LPVOID pBufferInPhysicalMem = Physical_Memory_ReadMem(self->pPhysicalMemory, physicalAdress);
		if (pBufferInPhysicalMem)
		{
			memcpy(pBuffer, pBufferInPhysicalMem, length);
			readMemResult = true;
		}
	}
	return readMemResult;
}

PMemManager MemoryManager_Create()
{
	PMemManager pMemoryManagerInstance;

	pMemoryManagerInstance = (PMemManager) malloc(sizeof(*pMemoryManagerInstance));

	pMemoryManagerInstance->pPageTable = pageTable_Create(PAGE_TABLE_SIZE);
	if (!pMemoryManagerInstance->pPageTable)
		return NULL;

	pMemoryManagerInstance->pPhysicalMemory = Physical_Memory_Create();
	if (!pMemoryManagerInstance->pPhysicalMemory)
		return NULL;

	pMemoryManagerInstance->hPageFile = CreateFile(L"PageFile.bin",
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == pMemoryManagerInstance->hPageFile) {
		printf("Error à la création du fichier 'PageFile.bin' error code : %lu", GetLastError());
		return NULL;
	}

	srand(time(NULL));

	// make all physical page free on the array
	memset(pMemoryManagerInstance->IsPhysicalPageFree, 0xFF,
		sizeof(pMemoryManagerInstance->IsPhysicalPageFree));
	return pMemoryManagerInstance;
}

void MemoryManager_Destroy(PMemManager object)
{
    CloseHandle(object->hPageFile);
	Physical_Memory_Destroy(object->pPhysicalMemory);
	pageTable_Destroy(object->pPageTable);
    free(object);
}

bool MemoryManager_WriteMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	return ReadWriteHandleMultiplePage(self, address, length, pBuffer, WriteMemOnOnePage);
}

bool MemoryManager_ReadMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	return ReadWriteHandleMultiplePage(self, address, length, pBuffer, ReadMemOnOnePage);
}


