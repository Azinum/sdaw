// mixer.c

static i32 MixerInit(mixer* Mixer, i32 ChannelCount, u32 FramesPerBuffer) {
{
  bus* Master = &Mixer->Busses[0];
  Master->Buffer = NULL;
  Master->ChannelCount = ChannelCount;
  Master->Active = 1;
  Master->InternalBuffer = 0;
}
  Mixer->BusCount = 1;
  Mixer->FramesPerBuffer = FramesPerBuffer;
  MixerInitBus(Mixer, 1, 1, NULL);
  return NoError;
}

static i32 MixerInitBus(mixer* Mixer, i32 BusIndex, i32 ChannelCount, float* Buffer) {
  if (BusIndex > 0 && BusIndex < MAX_AUDIO_BUS && Mixer->BusCount < MAX_AUDIO_BUS) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (!Buffer) {
      Bus->Buffer = M_Calloc(sizeof(float), ChannelCount * Mixer->FramesPerBuffer);
      Bus->InternalBuffer = 1;
    }
    else {
      Bus->Buffer = Buffer;
      Bus->InternalBuffer = 0;
    }
    Bus->ChannelCount = ChannelCount;
    Bus->Active = 1;

    Mixer->BusCount++;
  }
  return NoError;
}

static i32 MixerSumBusses(mixer* Mixer, float* OutBuffer) {
  bus* Master = &Mixer->Busses[0];
  Master->Buffer = OutBuffer;
  memset(Master->Buffer, 0, sizeof(float) * Master->ChannelCount * Mixer->FramesPerBuffer);
  if (!AudioEngine.IsPlaying) {
    return NoError;
  }

  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (Bus->Active && Bus->Buffer) {
      i32 Tick = AudioEngine.Tick;
      float* Iter = &Master->Buffer[0];
      for (i32 FrameIndex = 0; FrameIndex < Mixer->FramesPerBuffer; ++FrameIndex) {
#if 0
        audio_state* TempSound = &AudioEngine.TempSound;
        if (TempSound) {
          audio_source* Source = TempSound->Source;
          TempSound->Index = Tick;
          if (TempSound->Index < Source->SampleCount) {
            if (Source->ChannelCount == 2) {
              float Frame0 = Source->Buffer[TempSound->Index * Source->ChannelCount + 0];
              float Frame1 = Source->Buffer[TempSound->Index * Source->ChannelCount + 1];
              *(Iter++) += Frame0;
              *(Iter++) += Frame1;
            }
            else {
              float Frame0 = Source->Buffer[TempSound->Index * Source->ChannelCount + 0];
              *(Iter++) += Frame0;
              *(Iter++) += Frame0;
            }
          }
        }
#else
        if (Bus->ChannelCount == 2) {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          float Frame1 = Bus->Buffer[FrameIndex * Bus->ChannelCount + 1];
          *(Iter++) += Frame0;
          *(Iter++) += Frame1;
        }
        else {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          *(Iter++) += Frame0;
          *(Iter++) += Frame0;
        }
#endif
        Tick++;
      }
    }
  }
  return NoError;
}

static void MixerFree(mixer* Mixer) {
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (Bus->InternalBuffer) {
      M_Free(Bus->Buffer, sizeof(float) * Bus->ChannelCount * Mixer->FramesPerBuffer);
    }
    memset(Bus, 0, sizeof(bus));
  }
}
