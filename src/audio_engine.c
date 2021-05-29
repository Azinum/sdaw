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

  AudioEngine.SampleRate = SampleRate;
  AudioEngine.FramesPerBuffer = FramesPerBuffer;
  AudioEngine.Tick = 0;
  AudioEngine.Out = NULL;
  AudioEngine.In = NULL;
  AudioEngine.Time = 0.0f;
  AudioEngine.DeltaTime = 0.0f;
  AudioEngine.IsPlaying = 1;
  AudioEngine.Initialized = 1;

  i32 Result = NoError;
  if ((Result = AudioEngineInit(Engine, SampleRate, FramesPerBuffer) != NoError)) {
    fprintf(stderr, "Failed to initialize audio engine\n");
  }
  return Result;
}

i32 AudioEngineProcess(const void* InBuffer, void* OutBuffer) {
  TIMER_START();

  audio_engine* Engine = &AudioEngine;
  mixer* Mixer = &Engine->Mixer;

  Engine->In = (float*)InBuffer;
  Engine->Out = (float*)OutBuffer;

  if (Mixer->Active) {
    MixerClearBuffers(Mixer);
    MixerSumBuses(Mixer, Engine->IsPlaying, Engine->Out, Engine->In);
  }
  else {
    ClearFloatBuffer(Engine->Out, sizeof(float) * MASTER_CHANNEL_COUNT * Engine->FramesPerBuffer);
  }
  if (AudioEngine.IsPlaying) {
    const float DeltaTime = (1.0f / Engine->SampleRate) * Engine->FramesPerBuffer;
    Engine->DeltaTime = DeltaTime;
    Engine->Time += DeltaTime;
    Engine->Tick += Engine->FramesPerBuffer;
  }
  TIMER_END();
  return NoError;
}
