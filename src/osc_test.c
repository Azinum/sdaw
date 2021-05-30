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

#define MAX_NOTES 32

#define MAX_NOTE 127

static note_state _NoteTable[MAX_NOTES] = {0};
static i32 _NoteCount = 0;

typedef struct osc_test_instrument {
} osc_test_instrument;

inline i32 Sign(float Value);
static float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate);
static float SquareWave(i32 Tick, i32 FreqIndex, i32 SampleRate);
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

note_state* OscTestPlayNote(i32 FreqIndex, float AttackTime, float ReleaseTime, float Velocity) {
  if (_NoteCount < MAX_NOTES) {
    note_state Note = (note_state) {
      .Amp = 0.0f,
      .AttackTime = AttackTime,
      .ReleaseTime = ReleaseTime,
      .Velocity = Velocity,
      .FreqIndex = FreqIndex,
      .State = STATE_ATTACK,
    };
    _NoteTable[_NoteCount] = Note;
    return &_NoteTable[_NoteCount++];
  }
  return NULL;
}

// TODO(lucas): Generalized ADSR
i32 OscTestProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  TIMER_START();

  float* Iter = Bus->Buffer;
  i32 Tick = AudioEngine.Tick;
  float Time = AudioEngine.Time;
  float DeltaTime = AudioEngine.DeltaTime;

  float TimeStamp = InsTime + (60.0f / TempoBPM);
  if (Time >= TimeStamp) {
    float Delta = Time - TimeStamp;
    InsTime = Time - Delta;
    i32 Note = MelodyTable[MelodyIndex];
    if (Note >= 0) {
      note_state* NewNote = OscTestPlayNote(MelodyTable[MelodyIndex], DefaultAttackTime, DefaultReleaseTime, 0.5f);
      if (NewNote) {
        NewNote->Velocity = 0.25f;
      }
    }
    MelodyIndex = (MelodyIndex + 1) % ArraySize(MelodyTable);
  }

  ClearNoteTable(&_NoteTable[0], &_NoteCount);
  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex, ++Tick) {
    float Frame0 = 0.0f;
    float Frame1 = 0.0f;
#if 0
    for (i32 NoteIndex = 0; NoteIndex < _NoteCount; ++NoteIndex) {
      note_state* Note = &_NoteTable[NoteIndex];
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
#endif
    for (i32 NoteIndex = 0; NoteIndex < MAX_NOTE; ++NoteIndex) {
      float NoteIsPressed = NoteTable[NoteIndex];
      if (NoteIsPressed > 0.0f) {
        Frame0 += NoteIsPressed * 0.5f * SineWave(Tick, NoteIndex, SampleRate);
        Frame1 += NoteIsPressed * 0.5f * SineWave(Tick, NoteIndex, SampleRate);
      }
    }
    if (Bus->ChannelCount == 2) {
      *(Iter++) = Frame0;
      *(Iter++) = Frame1;
    }
    else {
      *(Iter++) = 0.5f * Frame0 + 0.5f * Frame1;
    }
  }

  // WeirdEffect2(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.02f, 50.0f);
  Distortion(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.5f, 230.0f);
  WeirdEffect(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.5f, 100.0f);
  Distortion(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.5f, 230.0f);

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

i32 OscTestInit(instrument* Ins) {
  i32 Result = NoError;
  Result = InstrumentAllocUserData(Ins, sizeof(osc_test_instrument));
  return Result;
}

i32 OscTestFree(instrument* Ins) {
  return NoError;
}
