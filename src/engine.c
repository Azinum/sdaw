// engine.c

#include "mixer.c"
#include "audio_engine.c"
#include "osc_test.c"
#include "draw.c"
#include "window.c"

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;
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
  return NoError;
}

i32 EngineInit() {
  mixer* Mixer = &AudioEngine.Mixer;
  MixerInit(Mixer, SAMPLE_RATE, FRAMES_PER_BUFFER);

  AudioEngineInit(SAMPLE_RATE, FRAMES_PER_BUFFER);
  AudioEngineStart(EngineRun);

  MixerFree(Mixer);
  WindowClose();
  return NoError;
}

void EngineFree() {
  AudioEngineTerminate();
}
