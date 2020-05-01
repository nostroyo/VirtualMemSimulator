//---------------------------------------------------------------------------

#ifndef PageTableH
#define PageTableH
//---------------------------------------------------------------------------
#include<stdlib>
#include <stdbool.h>


typedef struct
{
	bool isInPhysicalMem;
	unsigned char pageNumber;
} pageTableEntry, *PPageTableEntry;

typedef struct
{   unsigned char nbPageTableEntry;
	PPageTableEntry *pPageTable;
} PageTableObject, *PPageTableObject;

PPageTableObject pageTable_Create(unsigned char tableSize);
void pageTable_Destroy(PPageTableObject object);

PPageTableEntry pageTable_GetEntry(PPageTableObject self, unsigned char index);
bool pageTable_AddEntryAtIndex(PPageTableObject self, unsigned char index, PPageTableEntry entry);



#endif
