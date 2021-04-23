// osc_test.c

static float InsTime = 0;
static i32 MelodyTable[] = {
#if 0
  -1,
#else
  12, 0, 0, 0,
  0,  0, 0, 0,
#endif
};
static i32 MelodyIndex = 0;
static float InitAmp = 0.5f;
static float MasterAmp = 0.5f;

typedef enum ins_state {
  STATE_ATTACK = 0,
  STATE_RELEASE,
  STATE_DONE,
} ins_state;

static ins_state State = STATE_ATTACK;

typedef struct note_state {
  float Amp;
  i32 FreqIndex;
  ins_state State;
} note_state;

#define MAX_NOTES 32

static note_state NoteTable[MAX_NOTES] = {0};
static i32 NoteCount = 0;

inline float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate);
static void Distortion(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount);
static void ClearNoteTable(note_state* Table, i32* Count);

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

float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate) {
  float Freq = FreqTable[FreqIndex % FREQ_TABLE_SIZE];
  return sin((Tick * Freq * 2 * PI32) / SampleRate);
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

void OscTestPlayNote(i32 FreqIndex) {
  if (NoteCount < MAX_NOTES) {
    note_state Note = (note_state) {
      .Amp = 0.0f,
      .FreqIndex = FreqIndex,
      .State = STATE_ATTACK,
    };
    NoteTable[NoteCount++] = Note;
  }
}

i32 OscTestProcess(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, i32 SampleRate) {
  if (!AudioEngine.IsPlaying)
    return NoError;
  TIMER_START();

	float* Iter = Buffer;
  i32 Tick = AudioEngine.Tick;
  float Time = AudioEngine.Time;
  ClearNoteTable(&NoteTable[0], &NoteCount);
  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    float Frame0 = 0.0f;
    float Frame1 = 0.0f;

    float TimeStamp = InsTime + (60.0f / TEMPO_BPM);
    if (Time >= TimeStamp) {
      float Delta = Time - TimeStamp;
      InsTime = Time - Delta;
      i32 Note = MelodyTable[MelodyIndex];
      if (Note >= 0) {
        OscTestPlayNote(MelodyTable[MelodyIndex]);
      }
      MelodyIndex = (MelodyIndex + 1) % ArraySize(MelodyTable);
    }
    for (i32 NoteIndex = 0; NoteIndex < NoteCount; ++NoteIndex) {
      note_state* Note = &NoteTable[NoteIndex];
      switch (Note->State) {
        case STATE_ATTACK: {
          Note->Amp += (1.0f / 100.0f);
          if (Note->Amp >= InitAmp) {
            Note->State = STATE_RELEASE;
          }
          break;
        }
        case STATE_RELEASE: {
          Note->Amp -= (1.0f / 30000.0f);
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
      Frame0 += Note->Amp * SineWave(Tick, Note->FreqIndex, SampleRate);
      Frame1 += Note->Amp * SineWave(Tick, Note->FreqIndex, SampleRate);
    }
    if (ChannelCount == 2) {
      *(Iter++) = MasterAmp * Frame0;
      *(Iter++) = MasterAmp * Frame1;
    }
    else {
      *(Iter++) = MasterAmp * (0.5f * (Frame0 + Frame1));
    }
    ++Tick;
  }

	Distortion(Buffer, ChannelCount, FramesPerBuffer, 0.5f, 10.0f);
	Distortion(Buffer, ChannelCount, FramesPerBuffer, 0.1f, 30.0f);
  TIMER_END();
  return NoError;
}
