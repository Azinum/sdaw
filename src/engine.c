// engine.c

#include "mixer.c"
#include "audio_engine.c"
#include "osc_test.c"
#include "draw.c"
#include "window.c"

static i32 BaseNote = 0;
static float AttackTime = 0.03f;
static float ReleaseTime = 3.0f;

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
        if (BaseNote >= (FreqTableSize - 12))
          BaseNote = (FreqTableSize - 12);
      }
      if (KeyPressed[GLFW_KEY_0]) {
        AudioEngineRestart();
      }

      if (KeyPressed[GLFW_KEY_1]) {
        OscTestIncrAttackTime(-0.02f);
      }
      if (KeyPressed[GLFW_KEY_2]) {
        OscTestIncrAttackTime(0.02f);
      }
      if (KeyPressed[GLFW_KEY_3]) {
        OscTestIncrReleaseTime(-0.02f);
      }
      if (KeyPressed[GLFW_KEY_4]) {
        OscTestIncrReleaseTime(0.02f);
      }
      if (KeyPressed[GLFW_KEY_0]) {
        TempoBPM = 120;
      }
      if (KeyPressed[GLFW_KEY_8]) {
        --TempoBPM;
      }
      if (KeyPressed[GLFW_KEY_9]) {
        ++TempoBPM;
      }

      if (KeyPressed[GLFW_KEY_A]) { // A
        OscTestPlayNote(BaseNote + 0, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_W]) {
          OscTestPlayNote(BaseNote + 1, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_S]) {
        OscTestPlayNote(BaseNote + 2, AttackTime, ReleaseTime);
      }
      if (KeyPressed[GLFW_KEY_D]) {
        OscTestPlayNote(BaseNote + 3, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_R]) {
          OscTestPlayNote(BaseNote + 4, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_F]) {
        OscTestPlayNote(BaseNote + 5, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_T]) {
          OscTestPlayNote(BaseNote + 6, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_G]) {
        OscTestPlayNote(BaseNote + 7, AttackTime, ReleaseTime);
      }
      if (KeyPressed[GLFW_KEY_H]) {
        OscTestPlayNote(BaseNote + 8, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_U]) {
          OscTestPlayNote(BaseNote + 9, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_J]) {
        OscTestPlayNote(BaseNote + 10, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_I]) {
          OscTestPlayNote(BaseNote + 11, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_K]) { // A
        OscTestPlayNote(BaseNote + 12, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_O]) {
          OscTestPlayNote(BaseNote + 13, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_L]) {
        OscTestPlayNote(BaseNote + 14, AttackTime, ReleaseTime);
      }
      if (KeyPressed[GLFW_KEY_SEMICOLON]) {
        OscTestPlayNote(BaseNote + 15, AttackTime, ReleaseTime);
      }
        if (KeyPressed[GLFW_KEY_LEFT_BRACKET]) {
          OscTestPlayNote(BaseNote + 16, AttackTime, ReleaseTime);
        }
      if (KeyPressed[GLFW_KEY_APOSTROPHE]) {
        OscTestPlayNote(BaseNote + 17, AttackTime, ReleaseTime);
      }

      MixerRender(Mixer);
      OscTestRender();

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
