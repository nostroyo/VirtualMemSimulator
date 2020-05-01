//---------------------------------------------------------------------------

#include "PhysicalMemory.h"
#include <stdio.h>
#include <string.h>

PPhysical_Memory Physical_Memory_Create()
{
	PPhysical_Memory self = (PPhysical_Memory) malloc(sizeof(*self));

	self->hPhysicalMemFile = CreateFile(L"Physical_Mem.bin",
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (INVALID_HANDLE_VALUE == self->hPhysicalMemFile) {
		printf("Error à la création des fichiers error code : %lu", GetLastError());
		return NULL;
	}

	LPCVOID lpBuffer = malloc(PHYSICAL_MEMORY_SIZE);
	memset((void *)lpBuffer, '\0', PHYSICAL_MEMORY_SIZE);
	DWORD lnbWritten;

	if (!WriteFile(self->hPhysicalMemFile, lpBuffer, PHYSICAL_MEMORY_SIZE, &lnbWritten,
		NULL))
	{
		printf("Error à la création des fichiers error code : %lu", GetLastError());
		return NULL;
	}

	self->hMemFileMapping = CreateFileMapping(self->hPhysicalMemFile, NULL, PAGE_READWRITE, 0, 0, NULL);

	self->pPhysicalMem = MapViewOfFile(self->hMemFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
   if (NULL == self->pPhysicalMem) {
		DWORD LLastError = GetLastError();
		printf("Error à la création des fichiers error code : %lu", LLastError);
		return NULL;
   }
	return self;
}

void Physical_Memory_Destroy(PPhysical_Memory object)
{
	UnmapViewOfFile(object->pPhysicalMem);
	CloseHandle(object->hPhysicalMemFile);
	CloseHandle(object->hMemFileMapping);
	free(object);
}

LPVOID Physical_Memory_ReadMem(PPhysical_Memory self, UINT16 address)
{
	return (PUINT8) self->pPhysicalMem + address;
}

void Physical_Memory_WriteMem(PPhysical_Memory self, UINT16 address, LPCVOID pBufferToWrite, BYTE length)
{
	memcpy((PUINT8) self->pPhysicalMem + address, pBufferToWrite, length);
}

