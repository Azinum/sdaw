// audio_engine.h

#define MAX_AUDIO_BUS 64

typedef struct bus {
  float* Buffer;
  u32 ChannelCount;
  u8 Active;
  u8 InternalBuffer;
} bus;

typedef struct mixer {
  bus Busses[MAX_AUDIO_BUS];
  u32 BusCount;
  u32 FramesPerBuffer;
} mixer;

typedef struct audio_state {
  audio_source* Source;
  i32 Index;
} audio_state;

typedef struct audio_engine {
  u32 SampleRate;
  u32 FramesPerBuffer;
  i32 Tick;
  float* Out;
  float Time;
  u8 IsPlaying;
  mixer Mixer;
  audio_source TempAudioSource;
  audio_state TempSound;
} audio_engine;

extern audio_engine AudioEngine;

typedef i32 (*callback)();

i32 AudioEngineInit(u32 SampleRate, u32 FramesPerBuffer);

i32 AudioEngineStart(callback Callback);

void AudioEngineTerminate();
