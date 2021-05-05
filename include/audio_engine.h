// audio_engine.h

static i32 TempoBPM = 120;

#define MAX_AUDIO_BUS 32

struct instrument;
struct bus;

typedef i32 (*instrument_cb)(struct instrument* Ins);
typedef i32 (*instrument_process_cb)(struct instrument* Ins, struct bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

typedef struct instrument {
  buffer UserData;
  i32 Type;
  instrument_cb InitCb;
  instrument_cb FreeCb;
  instrument_process_cb Process;
} instrument;

typedef struct bus {
  float* Buffer;
  i32 ChannelCount;
  v2 Pan;
  v2 Db;
  v2 DbFade;  // NOTE(lucas): Temporary, for rendering
  u8 Active;
  u8 Disabled;
  u8 InternalBuffer;
  instrument* Ins;
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
