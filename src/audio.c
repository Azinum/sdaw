// audio.c

// NOTE(lucas): Abstraction layer function for loading multiple types audio source files. Only supports WAVE for now.
static i32 LoadAudioSource(const char* Path, audio_source* Source) {
  return LoadWAVE(Path, Source);
}

static i32 StoreAudioSource(const char* Path, audio_source* Source) {
  return StoreWAVE(Path, Source);
}

static i32 InitAudioSource(audio_source* Source, i32 SampleCount, i32 ChannelCount) {
  i32 Result = NoError;

  Source->Buffer = calloc(SampleCount * ChannelCount, sizeof(float));
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
    free(Source->Buffer);
  }
  memset(Source, 0, sizeof(audio_source));
}
