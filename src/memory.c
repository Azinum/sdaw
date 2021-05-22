// memory.c
// tracks basic memory information

struct {
  i64 Total;
  i64 Blocks;
} MemoryInfo = {
  .Total = 0,
  .Blocks = 0,
};

#define MemoryInfoUpdate(AddTotal, AddNumBlocks) \
  MemoryInfo.Total += (AddTotal); \
  MemoryInfo.Blocks += (AddNumBlocks)

i64 MemoryTotal() {
  return MemoryInfo.Total;
}

i64 MemoryNumBlocks() {
  return MemoryInfo.Blocks;
}

void MemoryPrintInfo(FILE* File) {
  fprintf(File,
    "Memory info:\n  Allocated blocks: %li, Total: %g MB (%li bytes)\n",
    MemoryInfo.Blocks,
    MemoryInfo.Total / (1024.0f * 1024.0f),
    MemoryInfo.Total
  );
}

void* M_Malloc(const i32 Size) {
  void* Data = malloc(Size);
  if (!Data)
    return NULL;
  MemoryInfoUpdate(Size, 1);
  return Data;
}

void* M_Calloc(const i32 Size, const i32 Count) {
  void* Data = calloc(Size, Count);
  if (!Data)
    return NULL;
  MemoryInfoUpdate(Size * Count, 1);
  return Data;
}

void* M_Realloc(void* Data, const i32 OldSize, const i32 NewSize) {
  assert(Data);
  i32 Diff = NewSize - OldSize;
  void* Temp = realloc(Data, NewSize);
  if (!Temp)
    return NULL;
  MemoryInfoUpdate(Diff, 0);
  return Temp;
}

void M_Free(void* Data, const i32 Size) {
  assert(Data);
  free(Data);
  MemoryInfoUpdate(-Size, -1);
}
