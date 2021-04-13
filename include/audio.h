// audio.h

typedef struct audio_source {
  float* Buffer;
  u32 SampleCount;
  u32 ChannelCount;
} audio_source;
