// audio.c

static i32 LoadAudioSource(const char* Path, audio_source* Source) {
  return LoadWAVE(Path, Source);
}

static i32 StoreAudioSource(const char* Path, audio_source* Source) {
  return StoreWAVE(Path, Source);
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
