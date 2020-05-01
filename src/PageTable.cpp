#include <string.h>

#include "PageTable.h"

#include "PhysicalMemory.h"

PPageTableObject pageTable_Create(unsigned char tableSize)
{

	PPageTableObject self = (PPageTableObject) malloc(sizeof(*self));
	self->nbPageTableEntry = tableSize;
	self->pPageTable = (PPageTableEntry *) malloc(tableSize * sizeof(PPageTableEntry));
	memset(self->pPageTable, 0x00, tableSize * sizeof(PPageTableEntry));
	return self;
}

void pageTable_Destroy(PPageTableObject object)
{
  for (int i = 0; i < object->nbPageTableEntry; i++)
  {
	  if (NULL != object->pPageTable[i])
	  {
		free(object->pPageTable[i]);
	  }
  }

  free(object->pPageTable);
  free(object);
}

PPageTableEntry pageTable_GetEntry(PPageTableObject self, unsigned char index)
{
	PPageTableEntry result = NULL;
	if (index < self->nbPageTableEntry)
	{
		result = self->pPageTable[index];
	}
	return result;
}

bool pageTable_AddEntryAtIndex(PPageTableObject self, unsigned char index, PPageTableEntry entry)
{
	bool result = false;
	if (index < self->nbPageTableEntry)
	{
		if (!pageTable_GetEntry(self, index))
		{
			self->pPageTable[index] = entry;
			result = true;
		}
	}
	return result;
}



