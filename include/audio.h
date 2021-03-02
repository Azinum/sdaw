// audio.h

typedef struct audio_source {
  float* Buffer;
  i32 SampleCount;
  i32 ChannelCount;
} audio_source;
