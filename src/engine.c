// engine.c

#include "mixer.c"
#include "audio_engine.c"
#include "osc_test.c"
#include "draw.c"
#include "window.c"

static i32 BaseNote = 0;

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;
  if (WindowOpen(G_WindowWidth, G_WindowHeight, TITLE, G_Vsync, G_FullScreen) == NoError) {
    RendererInit();
    while (WindowPollEvents() == 0) {
      TIMER_START();

      if (KeyPressed[GLFW_KEY_SPACE]) {
        Engine->IsPlaying = !Engine->IsPlaying;
      }
      if (KeyPressed[GLFW_KEY_Z]) {
        BaseNote -= 12;
        if (BaseNote < 0)
          BaseNote = 0;
      }
      if (KeyPressed[GLFW_KEY_X]) {
        BaseNote += 12;
        if (BaseNote >= (FREQ_TABLE_SIZE - 12))
          BaseNote = (FREQ_TABLE_SIZE - 12);
      }

      if (KeyPressed[GLFW_KEY_A]) {
        OscTestPlayNote(BaseNote + 0);
      }
      if (KeyPressed[GLFW_KEY_S]) {
        OscTestPlayNote(BaseNote + 2);
      }
      if (KeyPressed[GLFW_KEY_D]) {
        OscTestPlayNote(BaseNote + 3);
      }
      if (KeyPressed[GLFW_KEY_F]) {
        OscTestPlayNote(BaseNote + 5);
      }
      if (KeyPressed[GLFW_KEY_G]) {
        OscTestPlayNote(BaseNote + 7);
      }
      if (KeyPressed[GLFW_KEY_H]) {
        OscTestPlayNote(BaseNote + 8);
      }
      if (KeyPressed[GLFW_KEY_J]) {
        OscTestPlayNote(BaseNote + 10);
      }
      if (KeyPressed[GLFW_KEY_K]) {
        OscTestPlayNote(BaseNote + 12);
      }
      if (KeyPressed[GLFW_KEY_L]) {
        OscTestPlayNote(BaseNote + 14);
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
