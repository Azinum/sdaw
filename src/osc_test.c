// osc_test.c

static float InsTime = 0;
static i32 MelodyTable[] = {
#if 0
  24, 27, 29, 31,
  24, 27, 29, 31,
  24, 27, 29, 31,
  32, 31, 29, 27,
#else
  -1,
#endif
};

static i32 MelodyIndex = 0;
static float InitAmp = 0.5f;
static float MasterAmp = 0.5f;

static float DefaultAttackTime = 0.01f;
static float DefaultReleaseTime = 0.8f;

typedef enum ins_state {
  STATE_ATTACK = 0,
  STATE_RELEASE,
  STATE_DONE,
} ins_state;

typedef struct note_state {
  float Amp;
  float AttackTime;
  float ReleaseTime;
  float Velocity;
  i32 FreqIndex;
  ins_state State;
} note_state;

#define EFFECT_BUFFER_SIZE (1024 * 16)
static float EffectBuffer[EFFECT_BUFFER_SIZE] = {0};
static i32 EffectIndex = 0;
static i32 CurrentEffectIndex = 0;

#define MAX_NOTES 32

static note_state NoteTable[MAX_NOTES] = {0};
static i32 NoteCount = 0;

typedef struct osc_test_instrument {
  note_state NoteTable[MAX_NOTES];
  i32 NoteCount;
} osc_test_instrument;

inline i32 Sign(float Value);
static float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate);
static float SquareWave(i32 Tick, i32 FreqIndex, i32 SampleRate);
static void Distortion(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount);
static void WeirdEffect(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount);
static void WeirdEffect2(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount);
static void ClearNoteTable(note_state* Table, i32* Count);

i32 Sign(float Value) {
  return Value >= 0 ? 1 : -1;
}

float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate) {
  float Freq = FreqTable[FreqIndex % FreqTableSize];
  return Sin((Tick * Freq * 2 * PI32) / (float)SampleRate);
}

float SquareWave(i32 Tick, i32 FreqIndex, i32 SampleRate) {
  float Freq = FreqTable[FreqIndex % FreqTableSize];
  return Sign(Sin((Tick * Freq * 2 * PI32) / SampleRate));
}

void Distortion(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;
    WetFrame *= Amount;
    WetFrame = Clamp(WetFrame, -1.0f, 1.0f);
    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;

    EffectBuffer[EffectIndex] = DryFrame;
    EffectIndex = (EffectIndex + 1) % EFFECT_BUFFER_SIZE;
    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect2(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;

    WetFrame = EffectBuffer[(i32)((CurrentEffectIndex ^ 0xb))];
    CurrentEffectIndex = (CurrentEffectIndex + 1) % EFFECT_BUFFER_SIZE;

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void ClearNoteTable(note_state* Table, i32* Count) {
  for (i32 NoteIndex = 0; NoteIndex < *Count; ++NoteIndex) {
    note_state* Note = &Table[NoteIndex];
    if (Note->State == STATE_DONE) {
      *Note = Table[--(*Count)];
      --NoteIndex;
      continue;
    }
  }
}

note_state* OscTestPlayNote(i32 FreqIndex, float AttackTime, float ReleaseTime) {
  if (NoteCount < MAX_NOTES) {
    note_state Note = (note_state) {
      .Amp = 0.0f,
      .AttackTime = AttackTime,
      .ReleaseTime = ReleaseTime,
      .Velocity = 0.35f,
      .FreqIndex = FreqIndex,
      .State = STATE_ATTACK,
    };
    NoteTable[NoteCount] = Note;
    return &NoteTable[NoteCount++];
  }
  return NULL;
}

i32 OscTestProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  if (!AudioEngine.IsPlaying)
    return NoError;
  TIMER_START();

  float* Iter = Bus->Buffer;
  i32 Tick = AudioEngine.Tick;
  float Time = AudioEngine.Time;
  float DeltaTime = AudioEngine.DeltaTime;

  ClearNoteTable(&NoteTable[0], &NoteCount);
  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    float Frame0 = 0.0f;
    float Frame1 = 0.0f;

    float TimeStamp = InsTime + (60.0f / TempoBPM / 2);
    if (Time >= TimeStamp) {
      float Delta = Time - TimeStamp;
      InsTime = Time - Delta;
      i32 Note = MelodyTable[MelodyIndex];
      if (Note >= 0) {
        note_state* NewNote = OscTestPlayNote(MelodyTable[MelodyIndex], DefaultAttackTime, DefaultReleaseTime);
        if (NewNote) {
          NewNote->Velocity = 0.25f;
        }
      }
      MelodyIndex = (MelodyIndex + 1) % ArraySize(MelodyTable);
    }
    for (i32 NoteIndex = 0; NoteIndex < NoteCount; ++NoteIndex) {
      note_state* Note = &NoteTable[NoteIndex];
      switch (Note->State) {
        case STATE_ATTACK: {
          Note->Amp += (1.0f / Note->AttackTime / 60.0f) * DeltaTime;
          if (Note->Amp >= InitAmp) {
            Note->State = STATE_RELEASE;
          }
          break;
        }
        case STATE_RELEASE: {
          Note->Amp -= (1.0f / Note->ReleaseTime / 60.0f) * DeltaTime;
          if (Note->Amp < 0.0f) {
            Note->Amp = 0.0f;
            Note->State = STATE_DONE;
          }
          break;
        }
        case STATE_DONE: {
          break;
        }
        default:
          break;
      }
      Frame0 += Note->Velocity * Note->Amp * SineWave(Tick, Note->FreqIndex, SampleRate);
      Frame1 += Note->Velocity * Note->Amp * SineWave(Tick, Note->FreqIndex, SampleRate);
    }
    if (Bus->ChannelCount == 2) {
      *(Iter++) = MasterAmp * Frame0;
      *(Iter++) = MasterAmp * Frame1;
    }
    else {
      *(Iter++) = MasterAmp * (0.5f * (Frame0 + Frame1));
    }
    ++Tick;
  }

  TIMER_END();
  return NoError;
}

void OscTestIncrAttackTime(float Amount) {
  DefaultAttackTime += Amount;
  if (DefaultAttackTime <= 0.0f)  // Leaving it at zero will give click noises
    DefaultAttackTime = 0.001f;
}

void OscTestIncrReleaseTime(float Amount) {
  DefaultReleaseTime += Amount;
  if (DefaultReleaseTime <= 0.0f)
    DefaultReleaseTime = 0.001f;
}

void OscTestRender() {
  const i32 Gap = 8;
  const i32 TileSize = 32;
  for (i32 NoteIndex = 0; NoteIndex < NoteCount; ++NoteIndex) {
    note_state* Note = &NoteTable[NoteIndex];
    v3 P = V3((1 + Note->FreqIndex) * TileSize, TileSize * 2, 0);
    v2 Size = V2(TileSize - Gap, (1 + 10.0f * Note->Amp) * TileSize - Gap);
    v3 Color = V3(0, 0, 0);
    switch (Note->State) {
      case STATE_ATTACK:
        Color = V3(1.0f * Note->Amp, 0.3f * Note->Amp, 0.3f * Note->Amp);
        break;
      case STATE_RELEASE:
        Color = V3(0.2f * Note->Amp, 0.3f * Note->Amp, 1.0f * Note->Amp);
        break;
      case STATE_DONE:
      default:
        break;
    }
    DrawRect(P, Size, Color);
  }
}

static i32 OscTestInit(instrument* Ins) {
  i32 Size = sizeof(osc_test_instrument);
  osc_test_instrument* OscTest = M_Malloc(Size);
  Ins->UserData.Data = (void*)OscTest;
  Ins->UserData.Count = Size;
  return NoError;
}

instrument* OscTestCreate() {
  instrument* Ins = M_Malloc(sizeof(instrument));
  if (Ins) {
    InstrumentInit(Ins, INSTRUMENT_OSC_TEST);
    OscTestInit(Ins);
  }
  else {
    // TODO(lucas): Handle
  }
  return Ins;
}
