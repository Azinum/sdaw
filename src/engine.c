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

#define MAX_NOTE 127
float NoteTable[MAX_NOTE] = {0};

#define MAX_BUFFER_SIZE 512

char TitleBuffer[MAX_BUFFER_SIZE] = {};

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
    UI_Init();
    Mixer->Active = 1; // NOTE(lucas): We don't start the mixer until we have opened our window and initialized the renderer (to reduce startup audio glitches)
    while (WindowPollEvents() == 0) {
      REAL_TIMER_START();

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
              // printf("NOTE ON, note: %u, velocity: %u\n", A, B);
              float Velocity = (float)B / UINT8_MAX;
              NoteTable[A] = Velocity;
              break;
            }
            case MIDI_NOTE_OFF: {
              NoteTable[A] = 0;
              // printf("NOTE OFF, note: %u, velocity: %u\n", A, B);
              break;
            }
            default:
              break;
          }
        }
      }

      if (KeyPressed[GLFW_KEY_SPACE]) {
        Engine->Playing = !Engine->Playing;
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
      if (KeyPressed[GLFW_KEY_8] || GamepadButtonPressed[G_GamepadButtonLeft]) {
        --TempoBPM;
      }
      if (KeyPressed[GLFW_KEY_9] || GamepadButtonPressed[G_GamepadButtonRight]) {
        ++TempoBPM;
      }
      if (KeyPressed[GLFW_KEY_M]) {
        MemoryPrintInfo(stdout);
      }

      RendererBeginFrame();
      UI_Begin();

#if 1
      UI_SetPlacement(PLACEMENT_HORIZONTAL);

      if (UI_DoContainer(UI_ID)) {
        UI_SetContainerSizeMode(CONTAINER_SIZE_MODE_PERCENT);
        UI_SetContainerSize(V2(0.5f, 1.0f));

        if (UI_DoContainer(UI_ID)) {
          if (UI_DoTextButton(UI_ID, "Add")) {
            bus* Bus = MixerAddBus0(Mixer, 2, NULL, NULL);
            if (Bus) {
              instrument* Sampler = InstrumentCreate(INSTRUMENT_SAMPLER);
              MixerAttachInstrumentToBus0(Mixer, Bus, Sampler);
            }
          }
          if (UI_DoTextButton(UI_ID, "Remove")) {
            MixerRemoveBus(Mixer, Mixer->BusCount - 1);
          }
          if (UI_DoTextButton(UI_ID, "Reset")) {
            Engine->Tick = 0;
            Engine->Time = 0;
          }
          UI_SetContainerSize(V2(1.0f, 0.5f));
          if (UI_DoContainer(UI_ID)) {
            for (i32 InstrumentIndex = 0; InstrumentIndex < MAX_INSTRUMENT_DEF; ++InstrumentIndex) {
              instrument_def* InstrumentDef = &Instruments[InstrumentIndex];
              if (UI_DoTextButton(UI_ID + InstrumentIndex, InstrumentDef->Name)) {
                bus* Bus = MixerAddBus0(Mixer, 2, NULL, NULL);
                if (Bus) {
                  instrument* Instrument = InstrumentCreate(InstrumentIndex);
                  if (Instrument) {
                    MixerAttachInstrumentToBus0(Mixer, Bus, Instrument);
                  }
                }
              }
            }
            UI_EndContainer();
          }
          bus* Focus = MixerGetFocusedBus(Mixer);
          if (Focus) {
            if (UI_DoContainer(UI_ID)) {
              InstrumentDraw(Focus->Ins);
              UI_EndContainer();
            }
          }
          UI_EndContainer();
        }
        UI_SetContainerSize(V2(0.5f, 1.0f));
        if (UI_DoContainer(UI_ID)) {
          MixerRender(Mixer);
          UI_EndContainer();
        }
        UI_EndContainer();
      }
#endif

      UI_Render();
      RendererEndFrame();

      REAL_TIMER_END(
        snprintf(TitleBuffer, MAX_BUFFER_SIZE, "%s | fps: %i | bpm: %i | time: %.4g s", PROG_NAME, (i32)(1.0f / _DeltaTime), TempoBPM, Engine->Time);
        WindowSetTitle(TitleBuffer);
      );
    }
    RendererFree();
    UI_Free();
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
