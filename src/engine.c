// engine.c

#include "mixer.c"
#include "instrument.c"
#include "audio_engine.c"
#include "effect.c"
#include "serial_midi.c"
#include "osc_test.c"
#include "sampler.c"
#include "audio_input.c"
#include "draw.c"
#include "debug_ui.c"
#include "window.c"

static i32 BaseNote = 0;
static float AttackTime = 0.1f;
static float ReleaseTime = 15.0f;

// TODO(lucas): Move to elsewhere
enum midi_message {
  MIDI_NOTE_ON = 0x90,
  MIDI_NOTE_ON_HIGH = 0x9,  // NOTE(lucas): High nibble of the midi message
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_OFF_HIGH = 0x8,
};

#define MAX_NOTE 127
float NoteTable[MAX_NOTE] = {0};

#define LowNibble(Byte) (u8)(Byte & 0x0f)
#define HighNibble(Byte) (u8)((Byte & 0xf0) >> 4)

static i32 EngineRun(audio_engine* Engine) {
  mixer* Mixer = &Engine->Mixer;

  SerialMidiInit();
  OpenSerial("/dev/midi2");
  midi_event MidiEvents[MAX_MIDI_EVENT] = {0};
  u32 MidiEventCount = 0;
  memset(NoteTable, 0, ArraySize(NoteTable) * sizeof(float));

  if (WindowOpen(G_WindowWidth, G_WindowHeight, TITLE, G_Vsync, G_FullScreen) == NoError) {
    RendererInit();
    Mixer->Active = 1; // NOTE(lucas): We don't start the mixer until we have opened our window and initialized the renderer (to reduce startup audio glitches)
    while (WindowPollEvents() == 0) {
      TIMER_START();

      MidiEventCount = FetchMidiEvents(MidiEvents);

      if (MidiEventCount > 0) {
        for (i32 EventIndex = 0; EventIndex < MidiEventCount; ++EventIndex) {
          midi_event* Event = &MidiEvents[EventIndex];
          u8 Message = Event->Message;
          u8 High = HighNibble(Message);
          u8 Low = LowNibble(Message);
          u8 A = Event->A;
          u8 B = Event->B;
          switch (High) {
            case MIDI_NOTE_ON_HIGH: {
              printf("NOTE ON, note: %u, velocity: %u\n", A, B);
              float Velocity = (float)B / UINT8_MAX;
              NoteTable[A] = Velocity;
              break;
            }
            case MIDI_NOTE_OFF_HIGH: {
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

      UI_Begin();
      if (UI_DoContainer(UI_ID, V2(32, 300), V2(32 * 47 + 6, 32 * 7), V3(0.15f, 0.15f, 0.15f), 0)) {
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
        if (UI_DoButton(UI_ID, V2(0, 128), V2(32, 32), V3(0.4f, 0.1f, 0.9f))) {
          BaseNote = Clamp(BaseNote - 12, 0, MAX_NOTE - 12);
        }
        if (UI_DoButton(UI_ID, V2(32, 128), V2(32, 32), V3(0.5f, 0.4f, 0.9f))) {
          BaseNote = Clamp(BaseNote + 12, 0, MAX_NOTE - 12);
        }
        {
          v2 P = V2(0, 160);
          v2 Size = V2(16, 64);
#if 0
          for (i32 KeyIndex = 0; KeyIndex < 12; ++KeyIndex) {
            if (UI_DoSpecialButton(UI_ID + KeyIndex, P, Size, V3(0.7f, 0.7f, 0.7f))) {
              NoteTable[BaseNote + KeyIndex] = 0.15f;
            }
            else {
              NoteTable[BaseNote + KeyIndex] = 0.0f;
            }
            P.X += Size.W + 3;
          }
#else
          for (i32 KeyIndex = 0; KeyIndex < (MAX_NOTE * 0.75f) - 12; ++KeyIndex) {
            if (UI_DoSpecialButton(UI_ID + KeyIndex, P, Size, V3(0.7f, 0.7f, 0.7f))) {
              NoteTable[KeyIndex] = 0.15f;
            }
            else {
              NoteTable[KeyIndex] = 0.0f;
            }
            P.X += Size.W + 2;
          }
#endif
        }
      }

      MixerRender(Mixer);

      UI_Render();

      WindowSwapBuffers();
      WindowClear(0, 0, 0);

      TIMER_END();
    }
    RendererFree();
    CloseSerial();
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
