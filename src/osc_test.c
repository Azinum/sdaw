// osc_test.c

static float InsTime = 0;
static i32 Lf = 0;
static i32 Rf = 0;
static i32 MelodyTable[] = {
  24, 12, 12, 12,
  24, 12, 12, 15,
};
static i32 MelodyIndex = 0;
static float InitAmp = 0.5f;
static float Amp = 0.0f;

typedef enum ins_state {
  STATE_ATTACK = 0,
  STATE_RELEASE,
} ins_state;

static ins_state State = STATE_ATTACK;

inline float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate);

float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate) {
  float Freq = FreqTable[FreqIndex % FREQ_TABLE_SIZE];
  return sin((Tick * Freq * 2 * PI32) / SampleRate);
}

i32 OscTestProcess(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, i32 SampleRate) {
  if (!AudioEngine.IsPlaying)
    return NoError;

  i32 Tick = AudioEngine.Tick;
  float Time = AudioEngine.Time;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    float TimeStamp = InsTime + (60.0f / TEMPO_BPM);
    if (Time >= TimeStamp) {
      float Delta = Time - TimeStamp;
      InsTime = Time - Delta;
      Lf = MelodyTable[MelodyIndex];
      Rf = Lf + 12;
      MelodyIndex = (MelodyIndex + 1) % ArraySize(MelodyTable);
      Amp = 0;
      State = STATE_ATTACK;
    }
    switch (State) {
      case STATE_ATTACK: {
        Amp += (1.0f / 50.0f);
        if (Amp >= InitAmp) {
          State = STATE_RELEASE;
        }
        break;
      }
      case STATE_RELEASE: {
        Amp = Lerp(Amp, 0, 0.0001f);
        break;
      }
      default:
        break;
    }
    float Frame0 = Amp * SineWave(Tick % SampleRate, Lf, SampleRate);
    float Frame1 = Amp * SineWave(Tick % SampleRate, Rf, SampleRate);
    if (ChannelCount == 2) {
      *(Buffer++) = Frame0;
      *(Buffer++) = Frame1;
    }
    else {
      *(Buffer++) = 0.5f * (Frame0 + Frame1);
    }
    ++Tick;
  }
  return NoError;
}
