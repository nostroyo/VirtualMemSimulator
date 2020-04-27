# VirtualMemSimulator

As an exercice I try to reproduce a minimal virtual memory manager describe in the (excellent) book "MODERN OPERATING SYSTEMS
FOURTH EDITION" by ANDREW S. TANENBAUM & HERBERT BOS. This project is in C and Windows only as it use the Windows API.

For those who have the book , on page 197 it describe a how to use a 64KB virtual adress space on a 32KB physical memory.
Both physical and virtual memory are devided into 4KB pages.
I try to reproduce the behavior describe on page 199. 

Page frame = 4KB contiguous memory on physical memory
Virtual page = 4KB page on physical 

The physical memory is simulated by a mapped file in RAM using MapViewOfFile. 

How to use:
- Create the memory manager and get back a pointer to it.
  ```C
  pMemoryManager = MemoryManager_Create();
  ```
- Write a buffer (you have to allocate and free the buffer) where you want between 0x0000 and 0xffff  
  ```C
  LPVOID pBuffer = malloc(5);
  strcpy((char*)pBuffer, "toto");
  MemoryManager_WriteMem(pMemoryManager, 4095, 5, pBuffer);
  free(pBuffer);
  ```
  
- You can track where the memory manager write the buffer by inspecting the Physical_Mem.bin file.

- Read back the address (you have to allocate and free the buffer)
  ```C
  pBuffer = malloc(5);
  MemoryManager_ReadMem(pMemoryManager, 4095, 5, pBuffer);
  free(pBuffer);
  ```
  
- Destroy the memorymanager when you finished playing with it ;)*
  ```C
  MemoryManager_Destroy(pMemoryManager);
  ```
