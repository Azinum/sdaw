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

i32 MixerClearBuffers(mixer* Mixer) {
  TIMER_START();

#if USE_SSE
  __m128 Zero = _mm_set1_ps(0.0f);
  __m128* Dest = NULL;
  // TODO(lucas): We probably want to use a big contiguous array of
  // frame buffers, but for now buffers are seperately allocated for each bus
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->InternalBuffer) {
      i32 ChunkSize = 4 * sizeof(float);
      i32 BufferSize = (sizeof(float) * Bus->ChannelCount * Mixer->FramesPerBuffer) / ChunkSize;
      Dest = (__m128*)&Bus->Buffer[0];
      for (i32 ChunkIndex = 0; ChunkIndex < BufferSize; ++ChunkIndex, ++Dest) {
        *Dest = Zero;
      }
    }
  }
#else
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    // NOTE(lucas): The buffer is cleared elsewhere if it is not internal
    if (Bus->InternalBuffer) {
      memset(Bus->Buffer, 0, sizeof(float) * Bus->ChannelCount * Mixer->FramesPerBuffer);
    }
  }
#endif
  TIMER_END();
  return NoError;
}

i32 MixerSumBuses(mixer* Mixer, u8 IsPlaying, float* OutBuffer) {
  TIMER_START();

  bus* Master = &Mixer->Buses[0];
  Master->Buffer = OutBuffer;
  memset(Master->Buffer, 0, sizeof(float) * Master->ChannelCount * Mixer->FramesPerBuffer);
  if (!IsPlaying) {
    return NoError;
  }

  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->Active && Bus->Buffer) {
      float* Iter = &Master->Buffer[0];
      OscTestProcess(Bus->Buffer, Bus->ChannelCount, Mixer->FramesPerBuffer, Mixer->SampleRate);
      for (i32 FrameIndex = 0; FrameIndex < Mixer->FramesPerBuffer; ++FrameIndex) {
        if (Bus->ChannelCount == 2) {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          float Frame1 = Bus->Buffer[FrameIndex * Bus->ChannelCount + 1];
          *(Iter++) += Frame0 * Bus->Pan.X;
          *(Iter++) += Frame1 * Bus->Pan.Y;
        }
        else {
          float Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          *(Iter++) += Frame0 * Bus->Pan.X;
          *(Iter++) += Frame0 * Bus->Pan.Y;
        }
      }
    }
  }

  TIMER_END();
  return NoError;
}

i32 MixerRender(mixer* Mixer) {

#define TILE_SIZE 32
#define GAP 8

  for (i32 BusIndex = 0; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    v3 P = V3((1 + BusIndex) * TILE_SIZE, TILE_SIZE, 0);
    DrawRect(P, TILE_SIZE - GAP, TILE_SIZE - GAP);
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
