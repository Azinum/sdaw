// audio_engine.h

#ifndef _AUDIO_ENGINE_H
#define _AUDIO_ENGINE_H

#include <pthread.h>

static i32 TempoBPM = 140;

#define MAX_AUDIO_BUS 32

struct instrument;
struct bus;

typedef i32 (*instrument_cb)(struct instrument* Ins);
typedef i32 (*instrument_process_cb)(struct instrument* Ins, struct bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

typedef struct instrument {
  buffer UserData;
  i32 Type;
  instrument_cb Init;
  instrument_cb Destroy;
  instrument_cb Draw;
  instrument_process_cb Process;
  pthread_t LoadThread;
  u8 Ready;
} instrument;

typedef struct bus {
  float* Buffer;
  i32 ChannelCount;
  u32 ID;
  v2 Pan;
  v2 Db;
  u8 Active;
  u8 Disabled;
  u8 InternalBuffer;
  u8 ToRemove;
  u8 MidiInput;
  instrument* Ins;
} bus;

typedef struct mixer {
  bus Buses[MAX_AUDIO_BUS];
  i32 BusCount;
  i32 SampleRate;
  i32 FramesPerBuffer;
  bus* FocusedBus;
  u8 Active;
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
  u8 Playing;
  u8 Recording;
  u8 Initialized;
  mixer Mixer;
} audio_engine;

extern audio_engine AudioEngine;

typedef i32 (*callback)(audio_engine*);

i32 AudioEngineStateInit(i32 SampleRate, i32 FramesPerBuffer);

i32 AudioEngineStartRecording();

i32 AudioEngineStopRecording();

i32 AudioEngineProcess(const void* InBuffer, void* OutBuffer);

i32 AudioEngineStart(callback Callback);

void AudioEngineExit();

void AudioEngineTerminate();

#endif
