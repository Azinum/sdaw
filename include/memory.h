// memory.h

i32 MemoryTotal();

i32 MemoryNumBlocks();

void MemoryPrintInfo(FILE* File);

void* M_Malloc(const u32 Size);

void* M_Calloc(const u32 Size, const u32 Count);

void* M_Realloc(void* Data, const u32 OldSize, const u32 NewSize);

void M_Free(void* Data, const u32 Size);

