// audio_engine.h

static i32 TempoBPM = 120;

#define MAX_AUDIO_BUS 64

typedef struct bus {
  float* Buffer;
  i32 ChannelCount;
  v2 Pan;
  u8 Active;
  u8 InternalBuffer;
} bus;

typedef struct mixer {
  bus Buses[MAX_AUDIO_BUS];
  i32 BusCount;
  i32 SampleRate;
  i32 FramesPerBuffer;
} mixer;

typedef struct audio_state {
  audio_source* Source;
  i32 Index;
} audio_state;

typedef struct instrument {
  buffer UserData;
  i32 Type;
} instrument;

typedef struct audio_engine {
  i32 SampleRate;
  i32 FramesPerBuffer;
  i32 Tick;
  float* Out;
  float* In;
  float Time;
  float DeltaTime;
  u8 IsPlaying;
  u8 Initialized;
  mixer Mixer;
} audio_engine;

extern audio_engine AudioEngine;

typedef i32 (*callback)(audio_engine*);

i32 AudioEngineInit(i32 SampleRate, i32 FramesPerBuffer);

i32 AudioEngineStart(callback Callback);

i32 AudioEngineRestart();

void AudioEngineTerminate();
