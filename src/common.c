// common.c

#include <errno.h>
#include <dirent.h> // opendir

char DataPath[MAX_PATH_SIZE] = {};

char* FetchExtension(const char* Path) {
  char* Ext = strrchr(Path, '.');
  return Ext;
}

char* HomePath() {
  return getenv("HOME");
}

i32 ReadFile(const char* Path, buffer* Buffer) {
  i32 Result = NoError;

  FILE* File = fopen(Path, "rb");
  if (!File) {
    // fprintf(stderr, "Failed to open file '%s'\n", Path);
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

const char* GetDataPath() {
  DIR* Dir = opendir(LOCAL_DATA_PATH);
  if (Dir) {
    closedir(Dir);
    return LOCAL_DATA_PATH;
  }
  else if (errno == ENOENT) {
    Dir = opendir(DATA_PATH);
    if (Dir) {
      closedir(Dir);
      return DATA_PATH;
    }
  }
  Assert(0 && "No data directory exists");
  return NULL;
}

// NOTE(lucas): Not thread safe
const char* DataPathConcat(const char* Path) {
  const char* Data = GetDataPath();
  snprintf(DataPath, MAX_PATH_SIZE, "%s/%s", Data, Path);
  return DataPath;
}

i32 ReadFileFromDataPath(const char* Path, buffer* Buffer) {
  char FullPath[MAX_PATH_SIZE];
  snprintf(FullPath, MAX_PATH_SIZE, "%s/%s", GetDataPath(), Path);
  return ReadFile(FullPath, Buffer);
}

i32 ReadFileAndNullTerminate(const char* Path, buffer* Buffer) {
  i32 Result = NoError;

  if ((Result = ReadFile(Path, Buffer)) == NoError) {
    Buffer->Data[Buffer->Count - 1] = '\0';
  }
  return Result;
}

float RandomFloat(float From, float To) {
  float Result = 0.0f;

  Result = (float)Rand() / (float)(RAND_MAX / To) + (float)Rand() / (float)(RAND_MAX / From);

  return Result;
}

u64 Hash(char* String) {
  u64 HashNumber = 5381;
  i32 Ch = 0;
  char* Iter = String;
  if (!Iter)
    return HashNumber;
  while (*Iter++ != '\0') {
    Ch = *Iter;
    HashNumber = ((HashNumber << 5) + HashNumber) + Ch;
  }
  return HashNumber;
}
