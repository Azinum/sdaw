// audio_engine.h

typedef struct audio_engine {
  u32 SampleRate;
  u32 FramesPerBuffer;
  u32 Tick;
  float* Out;
  float Time;
  u8 IsPlaying;
} audio_engine;

#define MAX_AUDIO_BUS 64

typedef struct bus {
  float* Buffer;
  u32 ChannelCount;
  u8 Active;
} bus;

typedef struct mixer {
  bus Busses[MAX_AUDIO_BUS];
} mixer;

typedef i32 (*callback)();

i32 AudioEngineInit(u32 SampleRate, u32 FramesPerBuffer);

i32 AudioEngineStart(callback Callback);

void AudioEngineTerminate();
