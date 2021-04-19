// memory.c
// tracks basic memory information

struct {
  u32 Total;
  u32 Blocks;
} MemoryInfo;

#define MemoryInfoUpdate(AddTotal, AddNumBlocks) \
  MemoryInfo.Total += (AddTotal); \
  MemoryInfo.Blocks += (AddNumBlocks)

i32 MemoryTotal() {
  return MemoryInfo.Total;
}

i32 MemoryNumBlocks() {
  return MemoryInfo.Blocks;
}

void MemoryPrintInfo(FILE* File) {
  fprintf(File,
    "Memory info:\n  Allocated blocks: %i, Total: %.3g KB (%i bytes)\n",
    MemoryInfo.Blocks,
    MemoryInfo.Total / 1024.0f,
    MemoryInfo.Total
  );
}

void* M_Malloc(const u32 Size) {
  void* Data = malloc(Size);
  if (!Data)
    return NULL;
  MemoryInfoUpdate(Size, 1);
  return Data;
}

void* M_Calloc(const u32 Size, const u32 Count) {
  void* Data = calloc(Size, Count);
  if (!Data)
    return NULL;
  MemoryInfoUpdate(Size * Count, 1);
  return Data;
}

void* M_Realloc(void* Data, const u32 OldSize, const u32 NewSize) {
  assert(Data);
  i32 Diff = NewSize - OldSize;
  void* Temp = realloc(Data, NewSize);
  if (!Temp)
    return NULL;
  MemoryInfoUpdate(Diff, 0);
  return Temp;
}

void M_Free(void* Data, const u32 Size) {
  assert(Data);
  free(Data);
  MemoryInfoUpdate(-Size, -1);
}
