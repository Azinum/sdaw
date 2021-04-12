// engine.c

#include "audio_engine.c"
#include "window.c"

static i32 EngineStart() {
  if (WindowOpen(800, 600, "Let them sounds go wild", 1) == NoError) {
    while (WindowPollEvents() == 0) {
      if (KeyPressed[GLFW_KEY_P]) {
        AudioEngine.IsPlaying = !AudioEngine.IsPlaying;
      }
      WindowSwapBuffers();
      WindowClear(0, 0, 0);
    }
  }
  return NoError;
}

i32 EngineInit() {
  AudioEngineInit(SAMPLE_RATE, FRAMES_PER_BUFFER);
  AudioEngineStart(EngineStart);
  return NoError;
}

void EngineFree() {
  AudioEngineTerminate();
}
