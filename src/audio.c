// audio.c

static i32 ConvertToFloatBuffer(float* OutBuffer, i16* InBuffer, u32 SampleCount) {
  for (u32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] / ((float)INT16_MAX);
  }
  return NoError;
}

static i32 ConvertToInt16Buffer(i16* OutBuffer, float* InBuffer, u32 SampleCount) {
  for (u32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] * INT16_MAX;
  }
  return NoError;
}

static void ClearFloatBuffer(float* Buffer, i32 Size) {
  Assert(Buffer);
  Assert(Size > 0);
#if USE_SSE
  Assert(!(Size % 4));
  __m128 Zero = _mm_set1_ps(0.0f);
  __m128* Dest = (__m128*)Buffer;
  i32 ChunkSize = 4 * sizeof(float);
  Size /= ChunkSize;
  for (i32 ChunkIndex = 0; ChunkIndex < Size; ++ChunkIndex, ++Dest) {
    *Dest = Zero;
  }
#else
  memset(Buffer, 0, Size);
#endif
}

static i32 LoadAudioSource(const char* Path, audio_source* Source) {
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

static i32 StoreAudioSource(const char* Path, audio_source* Source) {
  char* Ext = FetchExtension(Path);
  if (!strncmp(Ext, ".wav", MAX_PATH_SIZE)) {
    return StoreWAVE(Path, Source);
  }
  else {
    fprintf(stderr, "%s: Extension '%s' not supported for file '%s'\n", __FUNCTION__, Ext, Path);
  }
  return Error;
}

static i32 InitAudioSource(audio_source* Source, u32 SampleCount, u32 ChannelCount) {
  i32 Result = NoError;

  Source->Buffer = M_Calloc(SampleCount * ChannelCount, sizeof(float));
  if (!Source->Buffer) {
    return Error;
  }
  Source->SampleCount = SampleCount;
  Source->ChannelCount = ChannelCount;
  return Result;
}

static void UnloadAudioSource(audio_source* Source) {
  Assert(Source);
  if (Source->Buffer) {
    M_Free(Source->Buffer, sizeof(float) * Source->SampleCount);
  }
  memset(Source, 0, sizeof(audio_source));
}
