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
static float Amp = 0.5f;

inline float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate);

float SineWave(i32 Tick, i32 FreqIndex, i32 SampleRate) {
  float Freq = FreqTable[FreqIndex % FREQ_TABLE_SIZE];
  return sin((Tick * Freq * 2 * PI32) / SampleRate);
}

i32 OscTestProcess(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, i32 SampleRate) {
  memset(Buffer, 0, sizeof(float) * ChannelCount * FramesPerBuffer);
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
      Amp = InitAmp;
    }
    Amp = Lerp(Amp, 0, 0.0001f);
    float Frame0 = Amp * SineWave(Tick, Lf, SampleRate);
    float Frame1 = Amp * SineWave(Tick, Rf, SampleRate);
    if (ChannelCount == 2) {
      *(Buffer++) = Frame0;
      *(Buffer++) = Frame1;
    }
    else {
      *(Buffer++) = 0.5f * Frame0 + 0.5f * Frame1;
    }
    ++Tick;
  }
  return NoError;
}
