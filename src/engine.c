// engine.c

#include "mixer.c"
#include "instrument.c"
#include "audio_engine.c"
#include "effect.c"
#include "osc_test.c"
#include "sampler.c"
#include "audio_input.c"
#include "draw.c"
#include "debug_ui.c"
#include "window.c"

static i32 BaseNote = 0;
static float AttackTime = 0.1f;
static float ReleaseTime = 15.0f;

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;
  if (WindowOpen(G_WindowWidth, G_WindowHeight, TITLE, G_Vsync, G_FullScreen) == NoError) {
    RendererInit();
    Mixer->Active = 1; // NOTE(lucas): We don't start the mixer until we have opened our window and initialized the renderer (to reduce startup audio glitches)
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
      if (KeyPressed[GLFW_KEY_9]) {
        MemoryPrintInfo(stdout);
      }

      if (KeyPressed[GLFW_KEY_1]) {
        MixerToggleActiveBus(Mixer, 0);
      }
      if (KeyPressed[GLFW_KEY_2]) {
        MixerToggleActiveBus(Mixer, 1);
      }
      if (KeyPressed[GLFW_KEY_3]) {
        MixerToggleActiveBus(Mixer, 2);
      }
      if (KeyPressed[GLFW_KEY_4]) {
        MixerToggleActiveBus(Mixer, 3);
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


      UI_Begin();
      if (UI_DoContainer(UI_ID, V2(32, 300), V2(1400, 200), V3(0.15f, 0.15f, 0.15f), 0)) {
        if (UI_DoButton(UI_ID, V2(0, 0), V2(64, 32), V3(0.9f, 0.25f, 0.25f))) {
          MixerRemoveBus(Mixer, Mixer->BusCount - 1);
        }
        if (UI_DoButton(UI_ID, V2(0, 32), V2(64, 32), V3(0.25f, 0.8f, 0.25f))) {
          bus* Bus = MixerAddBus0(Mixer, 2, NULL, NULL);
          if (Bus) {
            instrument* Sampler = InstrumentCreate(SamplerInit, SamplerFree, SamplerProcess);
            MixerAttachInstrumentToBus0(Mixer, Bus, Sampler);
          }
        }
        if (UI_DoButton(UI_ID, V2(0, 64), V2(64, 32), V3(0.25f, 0.25f, 0.9f))) {
          MemoryPrintInfo(stdout);
        }
      }

      MixerRender(Mixer);

      UI_Render();

      WindowSwapBuffers();
      WindowClear(0, 0, 0);

      TIMER_END();
    }
    RendererFree();
  }
  return NoError;
}

i32 EngineInit() {
  audio_engine* Engine = &AudioEngine;
  mixer* Mixer = &Engine->Mixer;
  MixerInit(Mixer, SAMPLE_RATE, FRAMES_PER_BUFFER);

  AudioEngineInit(SAMPLE_RATE, FRAMES_PER_BUFFER);
  AudioEngineStart(NULL);
  EngineRun(Engine);
  WindowClose();
  return NoError;
}

void EngineFree() {
  audio_engine* Engine = &AudioEngine;
  mixer* Mixer = &Engine->Mixer;
  AudioEngineTerminate();
  MixerFree(Mixer);
}
