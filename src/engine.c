// engine.c

#include "mixer.c"
#include "audio_engine.c"
#include "osc_test.c"
#include "draw.c"
#include "window.c"

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;
  MixerInit(Mixer, Engine->SampleRate, Engine->FramesPerBuffer);

  if (WindowOpen(G_WindowWidth, G_WindowHeight, TITLE, G_Vsync, G_FullScreen) == NoError) {
    RendererInit();
    while (WindowPollEvents() == 0) {
      TIMER_START();

      if (KeyPressed[GLFW_KEY_SPACE]) {
        Engine->IsPlaying = !Engine->IsPlaying;
      }

      MixerRender(Mixer);

      WindowSwapBuffers();
      WindowClear(0, 0, 0);

      TIMER_END();
    }
    RendererFree();
  }
  MixerFree(Mixer);
  return NoError;
}

i32 EngineInit() {
  AudioEngineInit(SAMPLE_RATE, FRAMES_PER_BUFFER);
  AudioEngineStart(EngineRun);
  WindowClose();
  return NoError;
}

void EngineFree() {
  AudioEngineTerminate();
}
