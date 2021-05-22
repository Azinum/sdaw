// memory.h

i64 MemoryTotal();

i64 MemoryNumBlocks();

void MemoryPrintInfo(FILE* File);

void* M_Malloc(const i32 Size);

void* M_Calloc(const i32 Size, const i32 Count);

void* M_Realloc(void* Data, const i32 OldSize, const i32 NewSize);

void M_Free(void* Data, const i32 Size);

