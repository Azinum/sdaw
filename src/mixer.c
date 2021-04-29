// mixer.c

#define MASTER_CHANNEL_COUNT 2

i32 MixerInit(mixer* Mixer, i32 SampleRate, i32 FramesPerBuffer) {
  Mixer->SampleRate = SampleRate;
  Mixer->FramesPerBuffer = FramesPerBuffer;
{
  bus* Master = &Mixer->Buses[0];
  Master->Buffer = NULL;
  Master->ChannelCount = MASTER_CHANNEL_COUNT;
  Master->Pan = V2(1, 1);
  Master->Active = 1;
  Master->InternalBuffer = 0;
}
  Mixer->BusCount = 1;
  MixerAddBus(Mixer, 2, NULL);
  return NoError;
}

i32 MixerAddBus(mixer* Mixer, i32 ChannelCount, float* Buffer) {
  if (Mixer->BusCount < MAX_AUDIO_BUS) {
    bus* Bus = &Mixer->Buses[Mixer->BusCount];
    if (!Buffer) {
      Bus->Buffer = M_Calloc(sizeof(float), ChannelCount * Mixer->FramesPerBuffer);
      Bus->InternalBuffer = 1;
    }
    else {
      Bus->Buffer = Buffer;
      Bus->InternalBuffer = 0;
    }
    Bus->ChannelCount = ChannelCount;
    Bus->Pan = V2(1, 1);
    Bus->Active = 1;

    Mixer->BusCount++;
  }
  else {
    // Handle
  }
  return NoError;
}

i32 MixerToggleActiveBus(mixer* Mixer, i32 BusIndex) {
  if (BusIndex < Mixer->BusCount) {
    bus* Bus = &Mixer->Buses[BusIndex];
    Bus->Active = !Bus->Active;
  }
  return NoError;
}

i32 MixerClearBuffers(mixer* Mixer) {
  TIMER_START();
  // TODO(lucas): We probably want to use a big contiguous array of
  // frame buffers, but for now buffers are seperately allocated for each bus
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    // NOTE(lucas): The buffer is cleared elsewhere if it is not internal
    if (Bus->InternalBuffer) {
      i32 BufferSize = sizeof(float) * Bus->ChannelCount * Mixer->FramesPerBuffer;
      ClearFloatBuffer(Bus->Buffer, BufferSize);
    }
  }
  TIMER_END();
  return NoError;
}

i32 MixerSumBuses(mixer* Mixer, u8 IsPlaying, float* OutBuffer, float* InBuffer) {
  TIMER_START();

  bus* Master = &Mixer->Buses[0];
  Master->Buffer = OutBuffer;
  i32 MasterBufferSize = sizeof(float) * Master->ChannelCount * Mixer->FramesPerBuffer;
  ClearFloatBuffer(Master->Buffer, MasterBufferSize);
  if (!IsPlaying || !Master->Active) {
    return NoError;
  }

  float* Iter = &Master->Buffer[0];
  for (i32 FrameIndex = 0; FrameIndex < Mixer->FramesPerBuffer; ++FrameIndex) {
    float Input0 = *InBuffer++;
    float Input1 = *InBuffer++;
    *(Iter++) = Input0;
    *(Iter++) = Input0;
  }

#if 0
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->Active && Bus->Buffer) {
      float* Iter = &Master->Buffer[0];
      OscTestProcess(NULL /* instrument */, Bus, Mixer->FramesPerBuffer, Mixer->SampleRate);
      for (i32 FrameIndex = 0; FrameIndex < Mixer->FramesPerBuffer; ++FrameIndex) {
        if (Bus->ChannelCount == 2) {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          float Frame1 = Bus->Buffer[FrameIndex * Bus->ChannelCount + 1];
          *(Iter++) += Frame0 * Bus->Pan.X * Master->Pan.X;
          *(Iter++) += Frame1 * Bus->Pan.Y * Master->Pan.Y;
        }
        else {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          *(Iter++) += Frame0 * Bus->Pan.X;
          *(Iter++) += Frame0 * Bus->Pan.Y;
        }
      }
    }
  }
#endif
  TIMER_END();
  return NoError;
}

i32 MixerRender(mixer* Mixer) {
  const i32 TileSize = 32;
  const i32 Gap = 8;

  for (i32 BusIndex = 0; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    (void)Bus;
    v3 P = V3((1 + BusIndex) * TileSize, TileSize, 0);
    v2 Size = V2(TileSize - Gap, TileSize - Gap);
    v3 Color = V3(0, 0, 0);
    if (Bus->Active) {
      Color = V3(0.25f, 0.25f, 0.90f);
    }
    else {
      Color = V3(0.30f, 0.30f, 0.30f);
    }
    DrawRect(P, Size, Color);
    // TODO(lucas): Remove
    if (BusIndex == 1) {
      OscTestRender();
    }
  }
  return NoError;
}

void MixerFree(mixer* Mixer) {
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->InternalBuffer) {
      M_Free(Bus->Buffer, sizeof(float) * Bus->ChannelCount * Mixer->FramesPerBuffer);
    }
    memset(Bus, 0, sizeof(bus));
  }
}
