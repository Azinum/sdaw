// mixer.c

static i32 MixerInit(mixer* Mixer, i32 ChannelCount, i32 FramesPerBuffer) {
{
  bus* Master = &Mixer->Busses[0];
  Master->Buffer = NULL;
  Master->ChannelCount = ChannelCount;
  Master->Pan = V2(1, 1);
  Master->Active = 1;
  Master->InternalBuffer = 0;
}
  Mixer->BusCount = 1;
  MixerInitBus(Mixer, 1, 1, NULL);
  return NoError;
}

static i32 MixerInitBus(mixer* Mixer, i32 BusIndex, i32 ChannelCount, float* Buffer) {
  if (BusIndex > 0 && BusIndex < MAX_AUDIO_BUS && Mixer->BusCount < MAX_AUDIO_BUS) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (!Buffer) {
      Bus->Buffer = M_Calloc(sizeof(float), ChannelCount * AudioEngine.FramesPerBuffer);
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
  return NoError;
}

static i32 MixerClearBuffers(mixer* Mixer) {
  TIMER_START();

  // TODO(lucas): Do timings to compare SIMD/no SIMD when we have some more buses in use
#if USE_SSE
  __m128 Zero = _mm_set1_ps(0.0f);
  __m128* Dest = NULL;
  // TODO(lucas): We probably want to use a big contiguous array of
  // frame buffers, but for now buffers are seperately allocated for each bus
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (Bus->InternalBuffer) {
      i32 ChunkSize = 4 * sizeof(float);
      i32 BufferSize = (sizeof(float) * Bus->ChannelCount * AudioEngine.FramesPerBuffer) / ChunkSize;
      Dest = (__m128*)&Bus->Buffer[0];
      for (i32 ChunkIndex = 0; ChunkIndex < BufferSize; ++ChunkIndex, ++Dest) {
        *Dest = Zero;
      }
    }
  }
#else
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    // NOTE(lucas): The buffer is cleared elsewhere if it is not internal
    if (Bus->InternalBuffer) {
      memset(Bus->Buffer, 0, sizeof(float) * Bus->ChannelCount * AudioEngine.FramesPerBuffer);
    }
  }
#endif
  TIMER_END();
  return NoError;
}

static i32 MixerSumBusses(mixer* Mixer, float* OutBuffer) {
  TIMER_START();

  bus* Master = &Mixer->Busses[0];
  Master->Buffer = OutBuffer;
  memset(Master->Buffer, 0, sizeof(float) * Master->ChannelCount * AudioEngine.FramesPerBuffer);
  if (!AudioEngine.IsPlaying) {
    return NoError;
  }

  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (Bus->Active && Bus->Buffer) {
      i32 Tick = AudioEngine.Tick;
      float* Iter = &Master->Buffer[0];
      OscTestProcess(Bus->Buffer, Bus->ChannelCount, AudioEngine.FramesPerBuffer, AudioEngine.SampleRate);
      for (i32 FrameIndex = 0; FrameIndex < AudioEngine.FramesPerBuffer; ++FrameIndex) {
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
        Tick++;
      }
    }
  }

  TIMER_END();
  return NoError;
}

static void MixerFree(mixer* Mixer) {
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Busses[BusIndex];
    if (Bus->InternalBuffer) {
      M_Free(Bus->Buffer, sizeof(float) * Bus->ChannelCount * AudioEngine.FramesPerBuffer);
    }
    memset(Bus, 0, sizeof(bus));
  }
}
