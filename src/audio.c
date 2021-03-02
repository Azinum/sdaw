// audio.c

// NOTE(lucas): Abstraction layer function for loading multiple types audio source files. Only supports WAVE for now.
static i32 LoadAudioSource(const char* Path, audio_source* Source) {
  return LoadWAVE(Path, Source);
}

static void UnloadAudioSource(audio_source* Source) {
  Assert(Source);
  if (Source->Buffer) {
    free(Source->Buffer);
  }
  memset(Source, 0, sizeof(audio_source));
}
