// sampler.c

typedef struct sampler_instrument_data {
  r32 TimeStamp;
  i32 Index;
  u8 Reverse;
  u8 Distort;
  u8 Weird;
  u8 Step;
  audio_source Source;
} sampler_instrument_data;

i32 SamplerInit(instrument* Ins) {
  i32 Result = NoError;
  if ((Result = InstrumentAllocUserData(Ins, sizeof(sampler_instrument_data))) == NoError) {
    sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
    Sampler->TimeStamp = 0;
    Sampler->Index = 0;
    Sampler->Reverse = 0;
    Sampler->Distort = 0;
    Sampler->Step = 0;
    // Result = LoadAudioSourceFromDataPath("data/audio/basic_kick.ogg", &Sampler->Source);
    Result = LoadAudioSourceFromDataPath("data/audio/dark_wind.ogg", &Sampler->Source);
  }
  return Result;
}

i32 SamplerProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
  audio_source* Source = &Sampler->Source;
  r32* Iter = Bus->Buffer;
  r32 Time = AudioEngine.Time;

  if (!Sampler->Step) {
    Sampler->Index = AudioEngine.Tick * Bus->ChannelCount;
  }

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    r32 Frame0 = 0.0f;
    r32 Frame1 = 0.0f;

    if (Sampler->Step) {
      r32 TimeStamp = Sampler->TimeStamp + ((60.0f / TempoBPM));
      if (Time >= TimeStamp) {
        r32 Delta = Time - TimeStamp; // Compensation for overstepping the time stamp
        Sampler->TimeStamp = Time - Delta;
        Sampler->Index = 0;
      }
    }

    if (Sampler->Reverse) {
      if (Sampler->Index < Source->SampleCount) {
        if (Source->ChannelCount == 2) {
          Frame0 = Source->Buffer[Source->SampleCount - Sampler->Index++];
          Frame1 = Source->Buffer[Source->SampleCount - Sampler->Index++];
        }
        else if (Source->ChannelCount == 1) {
          Frame0 = Frame1 = Source->Buffer[Source->SampleCount - Sampler->Index++];
        }
      }
    }
    else {
      if (Sampler->Index < Source->SampleCount) {
        if (Source->ChannelCount == 2) {
          Frame0 = Source->Buffer[Sampler->Index++];
          Frame1 = Source->Buffer[Sampler->Index++];
        }
        else if (Source->ChannelCount == 1) {
          Frame0 = Frame1 = Source->Buffer[Sampler->Index++];
        }
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
  if (Sampler->Distort) {
    Distortion(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.25f, 120.0f);
  }
  if (Sampler->Weird) {
    WeirdEffect(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.25f, 1000.0f);
  }
  return NoError;
}

i32 SamplerDraw(instrument* Ins) {
  sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
  Assert(Sampler);

  UI_DoTextToggle(UI_ID, "Distort", &Sampler->Distort);
  UI_DoTextToggle(UI_ID, "Weird", &Sampler->Weird);
  UI_DoTextToggle(UI_ID, "Reverse", &Sampler->Reverse);
  UI_DoTextToggle(UI_ID, "Step", &Sampler->Step);
  return NoError;
}

i32 SamplerFree(instrument* Ins) {
  sampler_instrument_data* Sampler = (sampler_instrument_data*)Ins->UserData.Data;
  Assert(Sampler);
  UnloadAudioSource(&Sampler->Source);
  return NoError;
}
