// stream.c

#define STREAM_BUFFER_COUNT 4

typedef struct stream_buffer {
  r32* Data;
  u32 Count;
  u32 Size;
} stream_buffer;

typedef struct stream_state {
  u8 Recording;
  u8 BufferTarget;
  u8 ShouldExit;
  u8 Done;
  u8 DoneWritingBuffer;

  stream_buffer StreamBuffer[STREAM_BUFFER_COUNT];
  stream_buffer* Buffer;
  stream_buffer* WriteBuffer;
  pthread_t WriteThread;
  pthread_mutex_t Mutex;

  FILE* File;
} stream_state;

static stream_state S;

static void StreamBufferInit(stream_buffer* Buffer, u32 Size);
static void SwitchStreamBufferTarget();
static void* StreamWriteThread(void* StreamState);

void StreamBufferInit(stream_buffer* Buffer, u32 Size) {
  Buffer->Data = M_Malloc(Size);
  Buffer->Count = 0;  // Number of bytes that has been written to this buffer
  Buffer->Size = Size;  // How much allocated memory in total
}

void SwitchStreamBufferTarget() {
  S.Buffer = &S.StreamBuffer[S.BufferTarget];
  S.BufferTarget = (S.BufferTarget + 1) % STREAM_BUFFER_COUNT;
  S.WriteBuffer = &S.StreamBuffer[S.BufferTarget];
}

void* StreamWriteThread(void* StreamState) {
  stream_state* Stream = (stream_state*)StreamState;
  Stream->Done = 0;

  while (!Stream->ShouldExit && Stream->Recording) {
    Stream->DoneWritingBuffer = 0;
    if (pthread_mutex_trylock(&Stream->Mutex)) {
      stream_buffer* Buffer = Stream->WriteBuffer;
      if (Buffer) {
        if (Stream->File) {
          fwrite(&Buffer->Data[0], sizeof(float), Buffer->Count, Stream->File);
          Buffer->Count = 0;
        }
      }
      pthread_mutex_unlock(&Stream->Mutex);
    }
    sleep(0);
  }
  pthread_join(Stream->WriteThread, NULL);
  Stream->Done = 1;
  Stream->Recording = 0;
  return NULL;
}

i32 StreamInit(i32 FramesPerBuffer, i32 ChannelCount, const char* Path) {
  S.Recording = 0;
  S.BufferTarget = 0;
  S.ShouldExit = 0;
  S.Done = 1;
  S.DoneWritingBuffer = 0;

  for (u32 BufferIndex = 0; BufferIndex < STREAM_BUFFER_COUNT; ++BufferIndex) {
    StreamBufferInit(&S.StreamBuffer[BufferIndex], FramesPerBuffer * ChannelCount * G_StreamBufferSizeMultiple * sizeof(float));
  }
  S.Buffer = NULL;
  S.WriteBuffer = NULL;
  S.Mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  S.File = fopen(Path, "wb");

  SwitchStreamBufferTarget();
  return NoError;
}

i32 StreamStartRecording() {
  S.Recording = 1;
  pthread_create(&S.WriteThread, NULL, StreamWriteThread, (void*)&S);
  return NoError;
}

i32 StreamStopRecording() {
  S.Recording = 0;
  return NoError;
}

u8 StreamIsRecording() {
  return S.Recording;
}

i32 StreamWriteBuffer(float* InBuffer, u32 BufferSize) {
  if (!S.Recording || InBuffer == NULL) {
    return NoError;
  }
  if (S.Buffer->Count * sizeof(float) + BufferSize < S.Buffer->Size) {
    stream_buffer* Buffer = S.Buffer;
    if (Buffer) {
      CopyFloatBuffer(&Buffer->Data[Buffer->Count], InBuffer, BufferSize);
      Buffer->Count += BufferSize / sizeof(float);
    }
  }
  else {
    // Uh, disk probably too slow...
    fprintf(stderr, "%s: Buffer (%i) filled\n", __FUNCTION__, S.BufferTarget);
  }
  if (S.Buffer->Count * sizeof(float) > (S.Buffer->Size / G_StreamBufferDenom)) {
    if (pthread_mutex_trylock(&S.Mutex)) {
      SwitchStreamBufferTarget();
      pthread_mutex_unlock(&S.Mutex);
    }
  }
  return NoError;
}

void StreamFree() {
  for (u32 BufferIndex = 0; BufferIndex < STREAM_BUFFER_COUNT; ++BufferIndex) {
    M_Free(S.StreamBuffer[BufferIndex].Data, S.StreamBuffer[BufferIndex].Size);
  }
  if (S.File) {
    fclose(S.File);
  }

#if 1
  buffer AudioFileContents;
  if (ReadFile("record.data", &AudioFileContents) == NoError) {
    audio_source Source = (audio_source) {
      .Buffer = (float*)&AudioFileContents.Data[0],
      .SampleCount = AudioFileContents.Count / sizeof(float),
      .ChannelCount = 2,
    };
    StoreAudioSource("record.wav", &Source);
    BufferFree(&AudioFileContents);
  }
#endif
  S.ShouldExit = 1;
  while (!S.Done) {
    sleep(0);
  }
}
