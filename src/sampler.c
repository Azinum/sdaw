// sampler.c

typedef struct sampler_instrument_data {
  float TimeStamp;
  i32 Index;
  audio_source Source;
} sampler_instrument_data;

i32 SamplerInit(instrument* Ins) {
  i32 Result = NoError;
  if ((Result = InstrumentAllocUserData(Ins, sizeof(sampler_instrument_data))) == NoError) {
    sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
    Sampler->TimeStamp = 0;
    Sampler->Index = 0;
    Result = LoadAudioSource("audio/basic_kick.ogg", &Sampler->Source);
  }
  return Result;
}

i32 SamplerProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
  audio_source* Source = &Sampler->Source;
  float* Iter = Bus->Buffer;
  float Time = AudioEngine.Time;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    float Frame0 = 0.0f;
    float Frame1 = 0.0f;

    float TimeStamp = Sampler->TimeStamp + ((60.0f / TempoBPM));
    if (Time >= TimeStamp) {
      float Delta = Time - TimeStamp; // Compensation for overstepping the time stamp
      Sampler->TimeStamp = Time - Delta;
      Sampler->Index = 0;
    }

    if (Sampler->Index < Source->SampleCount) {
      if (Source->ChannelCount == 2) {
        Frame0 = Source->Buffer[Sampler->Index++];
        Frame1 = Source->Buffer[Sampler->Index++];
      }
      else if (Source->ChannelCount == 1) {
        Frame0 = Frame1 = Source->Buffer[Sampler->Index++];
      }
    }
    if (Bus->ChannelCount == 2) {
      *Iter++ = Frame0;
      *Iter++ = Frame1;
    }
    else {
      *Iter++ = 0.5f * Frame0 + 0.5f * Frame1;
    }
  }
  return NoError;
}

i32 SamplerFree(instrument* Ins) {
  sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
  Assert(Sampler);
  UnloadAudioSource(&Sampler->Source);
  return NoError;
}
