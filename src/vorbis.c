// vorbis.c
// wrapper for stb_vorbis

#include "ext/stb_vorbis.c"

static i32 StoreOgg(const char* Path, audio_source* Source) {
  // Not implemented
  return Error;
}

static i32 LoadOgg(const char* Path, audio_source* Source) {
  i32 Result = NoError;
  i32 ChannelCount = 0;
  i32 SampleRate = 0;
  i16* Buffer = NULL;

  i32 SampleCount = stb_vorbis_decode_filename(Path, &ChannelCount, &SampleRate, &Buffer);
  if (SampleCount < 0) {
    fprintf(stderr, "%s: Failed to decode file '%s'\n", __FUNCTION__, Path);
    return Error;
  }
  if (SampleRate != G_SampleRate) {
    fprintf(stderr, "%s: Warning: Using sample rate (%i) which is different from the configured sample rate (%i) in file '%s'\n", __FUNCTION__, SampleRate, G_SampleRate, Path);
  }
  Source->ChannelCount = ChannelCount;
  Source->SampleCount = SampleCount * ChannelCount;
  Source->Buffer = M_Malloc(sizeof(float) * Source->SampleCount);
  if (!Source->Buffer) {
    fprintf(stderr, "%s: Failed to allocate sample buffer\n", __FUNCTION__);
    Result = Error;
    goto Done;
  }
  ConvertToFloatBuffer(Source->Buffer, Buffer, Source->SampleCount);
Done:
  free(Buffer);
  return Result;
}
