// audio_engine.c

audio_engine AudioEngine;

i32 AudioEngineInit(audio_engine* Engine, i32 SampleRate, i32 FramesPerBuffer);

#if USE_SDL
  #include "audio_sdl.c"
#else
  #include "audio_pa.c"
#endif

i32 AudioEngineStateInit(i32 SampleRate, i32 FramesPerBuffer) {
  audio_engine* Engine = &AudioEngine;

  Engine->SampleRate = SampleRate;
  Engine->FramesPerBuffer = FramesPerBuffer;
  Engine->Tick = 0;
  Engine->Out = NULL;
  Engine->In = NULL;
  Engine->Time = 0.0f;
  Engine->DeltaTime = 0.0f;
  Engine->Playing = 1;
  Engine->Recording = 0;
  Engine->Initialized = 1;

  StreamInit(FramesPerBuffer, 2, "record.data");

  i32 Result = NoError;
  if ((Result = AudioEngineInit(Engine, SampleRate, FramesPerBuffer) != NoError)) {
    fprintf(stderr, "Failed to initialize audio engine\n");
  }
  return Result;
}

i32 AudioEngineStartRecording() {
  if (!StreamIsRecording()) {
    StreamStartRecording();
  }
  return NoError;
}

i32 AudioEngineStopRecording() {
  if (StreamIsRecording()) {
    StreamStopRecording();
  }
  return NoError;
}

i32 AudioEngineProcess(const void* InBuffer, void* OutBuffer) {
  TIMER_START();

  audio_engine* Engine = &AudioEngine;
  mixer* Mixer = &Engine->Mixer;

  Engine->In = (float*)InBuffer;
  Engine->Out = (float*)OutBuffer;

  if (Mixer->Active) {
    MixerClearBuffers(Mixer);
    MixerSumBuses(Mixer, Engine->Playing, Engine->Out, Engine->In);
    if (Engine->Recording) {
      StreamWriteBuffer(Engine->Out, sizeof(float) * MASTER_CHANNEL_COUNT * Engine->FramesPerBuffer);
    }
  }
  else {
    ClearFloatBuffer(Engine->Out, sizeof(float) * MASTER_CHANNEL_COUNT * Engine->FramesPerBuffer);
  }
  if (AudioEngine.Playing) {
    const float DeltaTime = (1.0f / Engine->SampleRate) * Engine->FramesPerBuffer;
    Engine->DeltaTime = DeltaTime;
    Engine->Time += DeltaTime;
    Engine->Tick += Engine->FramesPerBuffer;
  }
  TIMER_END();
  return NoError;
}

void AudioEngineTerminate() {
  AudioEngineStopRecording();
  StreamFree();
  AudioEngineExit();
}
