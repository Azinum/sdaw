// audio.c

i32 ConvertToFloatBuffer(float* OutBuffer, i16* InBuffer, u32 SampleCount) {
  for (u32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] / ((float)INT16_MAX);
  }
  return NoError;
}

i32 ConvertToInt16Buffer(i16* restrict OutBuffer, float* restrict InBuffer, u32 SampleCount) {
  for (u32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] * INT16_MAX;
  }
  return NoError;
}

void ClearFloatBuffer(float* restrict Buffer, u32 Size) {
  if (!Buffer) {
    return;
  }
  Assert(Size > 0);
#if USE_SSE
  Assert(!(Size % 4));
  __m128 Zero = _mm_set1_ps(0.0f);
  __m128* Dest = (__m128*)Buffer;
  u32 ChunkSize = 4 * sizeof(float);
  u32 MaxChunk = Size / ChunkSize;
  for (u32 ChunkIndex = 0; ChunkIndex < MaxChunk; ++ChunkIndex, ++Dest) {
    *Dest = Zero;
  }
#else
  memset(Buffer, 0, Size);
#endif
}

void CopyFloatBuffer(float* restrict DestBuffer, float* restrict Source, u32 Size) {
  Assert(DestBuffer && Source);
  Assert(Size > 0);
#if USE_SSE
  Assert(!(Size % 4));
  __m128* Dest = (__m128*)DestBuffer;
  __m128* Src = (__m128*)Source;
  u32 ChunkSize = 4 * sizeof(float);
  u32 MaxChunk = Size / ChunkSize;
  for (u32 ChunkIndex = 0; ChunkIndex < MaxChunk; ++ChunkIndex, ++Dest, ++Src) {
    *Dest = *Src;
  }
#else
  memcpy(DestBuffer, Source, Size);
#endif
}

// Eleminates floats that are on even indexes and replaces them with floats that are on uneven indexes
void CopyFloatBufferEliminateEven(float* DestBuffer, float* SourceBuffer, i32 Size) {
#if USE_SSE
  Assert(!(Size % 4));
  __m128* Dest = (__m128*)DestBuffer;
  __m128* Source = (__m128*)SourceBuffer;
  i32 ChunkSize = 4 * sizeof(float);
  i32 MaxChunk = Size / ChunkSize;
#define ShuffleMask 0x0
  for (i32 ChunkIndex = 0; ChunkIndex < MaxChunk; ++ChunkIndex, ++Dest, ++Source) {
    *Dest = _mm_shuffle_ps(*Dest, *Source, ShuffleMask);
  }
#else
  float* Dest = DestBuffer;
  float* Source = SourceBuffer;
  i32 ChunkSize = sizeof(float);
  i32 MaxChunk = Size / ChunkSize / 2;
  for (i32 ChunkIndex = 0; ChunkIndex < MaxChunk; ++ChunkIndex, Dest += 2, Source += 2) {
    *(Dest) = *Source;
    *(Dest + 1) = *Source;
  }
#endif
}

void CopyFloatBufferEliminateOdd(float* DestBuffer, float* SourceBuffer, i32 Size) {
  float* Dest = DestBuffer;
  float* Source = SourceBuffer;
  i32 ChunkSize = sizeof(float);
  i32 MaxChunk = Size / ChunkSize / 2;
  for (i32 ChunkIndex = 0; ChunkIndex < MaxChunk; ++ChunkIndex, Dest += 2, Source += 2) {
    *(Dest) = *(Source + 1);
    *(Dest + 1) = *(Source + 1);
  }
}

i32 LoadAudioSource(const char* Path, audio_source* Source) {
  char* Ext = FetchExtension(Path);
  if (!strncmp(Ext, ".wav", MAX_PATH_SIZE)) {
    return LoadWAVE(Path, Source);
  }
  else if (!strncmp(Ext, ".ogg", MAX_PATH_SIZE)) {
    return LoadOgg(Path, Source);
  }
  else {
    fprintf(stderr, "%s: Extension '%s' not supported for file '%s'\n", __FUNCTION__, Ext, Path);
  }
  return Error;
}

i32 LoadAudioSourceFromDataPath(const char* Path, audio_source* Source) {
  char FullPath[MAX_PATH_SIZE] = {0};
  snprintf(FullPath, MAX_PATH_SIZE, "%s/%s", GetDataPath(), Path);
  return LoadAudioSource(FullPath, Source);
}

i32 StoreAudioSource(const char* Path, audio_source* Source) {
  char* Ext = FetchExtension(Path);
  if (!strncmp(Ext, ".wav", MAX_PATH_SIZE)) {
    return StoreWAVE(Path, Source);
  }
  else {
    fprintf(stderr, "%s: Extension '%s' not supported for file '%s'\n", __FUNCTION__, Ext, Path);
  }
  return Error;
}

i32 InitAudioSource(audio_source* Source, u32 SampleCount, u32 ChannelCount) {
  i32 Result = NoError;

  Source->Buffer = M_Calloc(SampleCount * ChannelCount, sizeof(float));
  if (!Source->Buffer) {
    return Error;
  }
  Source->SampleCount = SampleCount;
  Source->ChannelCount = ChannelCount;
  return Result;
}

void UnloadAudioSource(audio_source* Source) {
  Assert(Source);
  if (Source->Buffer) {
    M_Free(Source->Buffer, sizeof(float) * Source->SampleCount);
  }
  memset(Source, 0, sizeof(audio_source));
}
