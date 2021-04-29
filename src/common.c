// common.c

char* FetchExtension(const char* Path) {
  char* Ext = strrchr(Path, '.');
  return Ext;
}

i32 ReadFile(const char* Path, buffer* Buffer) {
  i32 Result = NoError;

  FILE* File = fopen(Path, "rb");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    return Error;
  }

  fseek(File, 0, SEEK_END);
  u32 Size = ftell(File);
  fseek(File, 0, SEEK_SET);
  Buffer->Data = M_Malloc(Size * sizeof(char));
  Buffer->Count = Size;
  if (!Buffer->Data) {
    Buffer->Count = 0;
    Result = Error;
    goto Done;
  }

  u32 BytesRead = fread(Buffer->Data, 1, Size, File);
  if (BytesRead != Size) {
    fprintf(stderr, "Failed to read file '%s'\n", Path);
    Result = Error;
    goto Done;
  }

Done:
  fclose(File);
  return Result;
}

i32 ReadFileAndNullTerminate(const char* Path, buffer* Buffer) {
  i32 Result = NoError;

  if ((Result = ReadFile(Path, Buffer)) == NoError) {
    Buffer->Data[Buffer->Count - 1] = '\0';
  }
  return Result;
}
