//---------------------------------------------------------------------------

#pragma hdrstop

#include "MemoryManager.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "PageTable.h"
#include "PhysicalMemory.h"

static BOOL TranslateVirtualToPhysicalAddress(PMemManager self, UINT16 VirtualAddress, /*out*/ PUINT16 PhysicalAddress);
static BOOL WriteMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);
static BOOL ReadWriteHandleMultiplePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer, ReadOrWrite_Fp readOrWriteOnePageFunc);
static BOOL ReadMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer);

BOOL ReadWriteHandleMultiplePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer, ReadOrWrite_Fp readOrWriteOnePageFunc)
{
	UINT16 lAddress = address;
	UCHAR lLength = length;
	UINT8 *pUInt8Buffer = (UINT8 *) pBuffer;
	BOOL result = true;
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

BOOL TranslateVirtualToPhysicalAddress(PMemManager self, UINT16 VirtualAddress, /*out*/ PUINT16 PhysicalAddress)
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
		else
		{
//            PAGE FAULT HERE: TODO handle page file
		}
        return true;
	}
	else
	{
		return false;
	}
}

BOOL WriteMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
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
		PPageTableEntry pEntry = (PPageTableEntry) malloc(sizeof(*pEntry));
		pEntry->pageNumber = rand() % 8;
		pEntry->isInPhysicalMem = true;
		pageTable_AddEntryAtIndex(self->pPageTable, virtualPageIndex, pEntry);
		return WriteMemOnOnePage(self, address, length, pBuffer);
	}
}

PMemManager MemoryManager_Create()
{
	PMemManager memoryManagerInstance;
	memoryManagerInstance->pPageTable = pageTable_Create(PAGE_TABLE_SIZE);
	memoryManagerInstance->pPhysicalMemory = Physical_Memory_Create();
	srand(time(NULL));
    return memoryManagerInstance;
}

void MemoryManager_Destroy(PMemManager object)
{
	pageTable_Destroy(object->pPageTable);
	Physical_Memory_Destroy(object->pPhysicalMemory);
}

BOOL ReadMemOnOnePage(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	BOOL readMemResult = false;
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

BOOL MemoryManager_WriteMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	return ReadWriteHandleMultiplePage(self, address, length, pBuffer, WriteMemOnOnePage);
}

BOOL MemoryManager_ReadMem(PMemManager self, UINT16 address, UCHAR length, LPVOID pBuffer)
{
	return ReadWriteHandleMultiplePage(self, address, length, pBuffer, ReadMemOnOnePage);
}

#pragma package(smart_init)
