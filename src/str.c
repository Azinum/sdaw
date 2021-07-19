// str.c

void StringInit(string* String) {
  String->Data = NULL;
  String->Count = 0;
  String->Size = 0;
}

// NOTE(lucas): Shrink/minimize the string if the alloced memory is more than a certain
// threshold value (based on how much of the string that is currenly in use)
void StringMinimize(string* String) {
  u32 Threshold = String->Size / 2;
  if (String->Count < Threshold) {
    void* Data = M_Realloc(String->Data, String->Size, Threshold);
    if (Data) {
      String->Data = Data;
      String->Size = Threshold;
    }
  }
}

void StringPush(string* String, char Element) {
  u32 RequestSize = 1;
  if (String->Count + RequestSize < String->Size) {
    String->Data[String->Count++] = Element;
  }
  else {
    ListPush(String->Data, String->Size, Element);
    String->Count = String->Size;
  }
  StringMinimize(String);
}

void StringPushPrintf(string* String, const char* Format, ...) {
  char Chunk[MAX_BUFFER_SIZE] = {0};
  va_list Args;
  va_start(Args, Format);
  u32 NumBytes = vsnprintf(Chunk, MAX_BUFFER_SIZE, Format, Args);
  va_end(Args);

  u32 RequestSize = NumBytes + String->Count;

  if (RequestSize < String->Size) {
    memcpy(&String->Data[String->Count], Chunk, sizeof(char) * NumBytes);
    String->Count += NumBytes;
  }
  else {
    void* Data = M_Realloc(String->Data, String->Size, RequestSize);
    if (Data) {
      String->Data = Data;
      memcpy(&String->Data[String->Count], Chunk, sizeof(char) * NumBytes);
      String->Size = String->Count = RequestSize;
    }
  }
  StringMinimize(String);
}

void StringPrintf(string* String, const char* Format, ...) {
  char Chunk[MAX_BUFFER_SIZE] = {0};
  va_list Args;
  va_start(Args, Format);
  u32 NumBytes = vsnprintf(Chunk, MAX_BUFFER_SIZE, Format, Args);
  va_end(Args);

  if (NumBytes < String->Size) {
    memcpy(&String->Data[0], Chunk, sizeof(char) * NumBytes);
    String->Count = NumBytes;
  }
  else {
    void* Data = M_Realloc(String->Data, String->Size, NumBytes);
    if (Data) {
      String->Data = Data;
      memcpy(&String->Data[0], Chunk, sizeof(char) * NumBytes);
      String->Size = String->Count = NumBytes;
    }
  }
  StringMinimize(String);
}

void StringFree(string* String) {
  if (String->Data) {
    ListFree(String->Data, String->Size);
    String->Data = NULL;
    String->Count = String->Size = 0;
  }
}

void BufferInit(buffer* Buffer) {
  Buffer->Data = NULL;
  Buffer->Count = 0;
}

void BufferPush(buffer* Buffer, char Element) {
  ListPush(Buffer->Data, Buffer->Count, Element);
}

void BufferFree(buffer* Buffer) {
  if (Buffer->Data) {
    ListFree(Buffer->Data, Buffer->Count);
  }
}
