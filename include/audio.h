// audio.h

#define SAMPLE_RATE 44100

typedef struct audio_source {
  float* Buffer;
  i32 SampleCount;
  i32 ChannelCount;
} audio_source;
