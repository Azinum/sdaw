// engine.c

#include "mixer.c"
#include "instrument.c"
#include "audio_engine.c"
#include "effect.c"

#include "midi.c"
#include "midi_serial.c"
#include "midi_apple.c"

#include "osc_test.c"
#include "sampler.c"
#include "audio_input.c"
#include "draw.c"
#include "ui.c"
#include "window.c"

static i32 BaseNote = 0;
static float AttackTime = 0.1f;
static float ReleaseTime = 15.0f;

static i32 ButtonCount = 0;

#define MAX_NOTE 127
float NoteTable[MAX_NOTE] = {0};

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;

  MidiInitHandle(MIDI_HANDLE_APPLE);
  MidiInit();
  MidiOpenDevices();

  midi_event MidiEvents[MAX_MIDI_EVENT] = {0};
  u32 MidiEventCount = 0;
  memset(NoteTable, 0, ArraySize(NoteTable) * sizeof(float));

  if (WindowOpen(G_WindowWidth, G_WindowHeight, TITLE, G_Vsync, G_FullScreen) == NoError) {
    WindowAddResizeCallback(UI_WindowResizeCallback);
    WindowAddResizeCallback(RendererResizeWindowCallback);

    RendererInit();
    Mixer->Active = 1; // NOTE(lucas): We don't start the mixer until we have opened our window and initialized the renderer (to reduce startup audio glitches)
    while (WindowPollEvents() == 0) {
      TIMER_START();

      MidiEventCount = MidiFetchEvents(MidiEvents);

      if (MidiEventCount > 0) {
        for (i32 EventIndex = 0; EventIndex < MidiEventCount; ++EventIndex) {
          midi_event* Event = &MidiEvents[EventIndex];
          u8 Message = Event->Message;
          u8 High = HighNibble(Message);
          u8 Low = LowNibble(Message);
          u8 A = Event->A;
          u8 B = Event->B;
          switch (Message) {
            case MIDI_NOTE_ON: {
              printf("NOTE ON, note: %u, velocity: %u\n", A, B);
              float Velocity = (float)B / UINT8_MAX;
              NoteTable[A] = Velocity;
              break;
            }
            case MIDI_NOTE_OFF: {
              NoteTable[A] = 0;
              printf("NOTE OFF, note: %u, velocity: %u\n", A, B);
              break;
            }
            default:
              break;
          }
        }
      }

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
      if (KeyPressed[GLFW_KEY_5]) {
        MixerToggleActiveBus(Mixer, 4);
      }
      if (KeyPressed[GLFW_KEY_6]) {
        MixerToggleActiveBus(Mixer, 5);
      }
      if (KeyPressed[GLFW_KEY_7]) {
        MixerToggleActiveBus(Mixer, 6);
      }
      if (KeyPressed[GLFW_KEY_8]) {
        MixerToggleActiveBus(Mixer, 7);
      }
      if (KeyPressed[GLFW_KEY_9]) {
        MixerToggleActiveBus(Mixer, 8);
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
      if (KeyPressed[GLFW_KEY_M]) {
        MemoryPrintInfo(stdout);
      }

      RendererBeginFrame();

      UI_Begin();
      UI_SetPlacement(PLACEMENT_VERTICAL);

#if 1
      if (UI_DoContainer(UI_ID)) {
        if (UI_DoTextButton(UI_ID, "A")) {
          puts("A");
        }
        if (UI_DoTextButton(UI_ID, "B")) {
          puts("B");
        }
        if (UI_DoTextButton(UI_ID, "C")) {
          puts("C");
        }
        UI_SetContainerSize(V2(480, 128));
        if (UI_DoContainer(UI_ID)) {
          UI_SetPlacement(PLACEMENT_HORIZONTAL);
          if (UI_DoTextButton(UI_ID, "Add")) {
            ButtonCount = Clamp(ButtonCount + 1, 0, 100);
            UI_Refresh();
          }
          if (UI_DoTextButton(UI_ID, "Remove")) {
            ButtonCount = Clamp(ButtonCount - 1, 0, 100);
            UI_Refresh(); // Refresh ui when removing elements
          }
          for (i32 ButtonIndex = 0; ButtonIndex < ButtonCount; ++ButtonIndex) {
            if (UI_DoTextButton(UI_ID + ButtonIndex, "A")) {
              printf("ID: %u\n", UI_ID + ButtonIndex);
            }
          }
        }
      }
#endif
#if 0
      if (UI_DoContainer(UI_ID, V2(32, 300), V2(32 * 47 + 6, 32 * 7), V3(0.15f, 0.15f, 0.15f), 0)) {
        if (UI_DoButton(UI_ID, V2(0, 0), V2(64, 32), UIColorDecline)) {
          MixerRemoveBus(Mixer, Mixer->BusCount - 1);
        }
        if (UI_DoButton(UI_ID, V2(0, 33), V2(64, 32), UIColorAccept)) {
          bus* Bus = MixerAddBus0(Mixer, 2, NULL, NULL);
          if (Bus) {
            instrument* Sampler = InstrumentCreate(SamplerInit, SamplerFree, SamplerProcess);
            MixerAttachInstrumentToBus0(Mixer, Bus, Sampler);
          }
        }
      }
#endif

      MixerRender(Mixer);

      UI_Render();

      RendererEndFrame();

      TIMER_END();
    }
    RendererFree();
    CloseSerial();
    MidiCloseDevices();
  }
  return NoError;
}

i32 EngineInit() {
  audio_engine* Engine = &AudioEngine;
  mixer* Mixer = &Engine->Mixer;
  MixerInit(Mixer, G_SampleRate, G_FramesPerBuffer);

  AudioEngineStateInit(G_SampleRate, G_FramesPerBuffer);
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
