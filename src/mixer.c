// mixer.c

#define MASTER_BUS_INDEX 0
#define MASTER_CHANNEL_COUNT 2

static void FreeBus(mixer* Mixer, bus* Bus);
static i32 RemoveBus(mixer* Mixer, i32 BusIndex);

void FreeBus(mixer* Mixer, bus* Bus) {
  if (Bus->InternalBuffer) {
    M_Free(Bus->Buffer, sizeof(f32) * Bus->ChannelCount * Mixer->FramesPerBuffer);
  }
  if (Bus->Ins) {
    InstrumentFree(Bus->Ins);
  }
}

i32 RemoveBus(mixer* Mixer, i32 BusIndex) {
  if (BusIndex > MASTER_BUS_INDEX && BusIndex < Mixer->BusCount) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus == Mixer->FocusedBus) {
      Mixer->FocusedBus = NULL;
    }
    FreeBus(Mixer, Bus);
    --Mixer->BusCount;
    if (Mixer->BusCount > 0) {
      bus* Top = &Mixer->Buses[Mixer->BusCount];
      if (Mixer->FocusedBus == Top) {
        Mixer->FocusedBus = Bus;
      }
      *Bus = Mixer->Buses[Mixer->BusCount];
    }
  }
  return NoError;
}

i32 MixerInit(mixer* Mixer, i32 SampleRate, i32 FramesPerBuffer) {
  Mixer->SampleRate = SampleRate;
  Mixer->FramesPerBuffer = FramesPerBuffer;
  Mixer->FocusedBus = NULL;
  Mixer->Active = 0;

  bus* Master = &Mixer->Buses[0];
  Master->Buffer = NULL;
  Master->ChannelCount = MASTER_CHANNEL_COUNT;
  Master->ID = 0;
  Master->Pan = V2(1, 1);
  Master->Db = V2(DB_MIN, DB_MIN);
  Master->Active = 1;
  Master->Disabled = 0;
  Master->InternalBuffer = 0;
  Master->ToRemove = 0;
  Master->MidiInput = 0;

  Mixer->BusCount = 1;

  return NoError;
}

bus* MixerGetBus(mixer* Mixer, i32 BusIndex) {
  if (BusIndex > MASTER_BUS_INDEX && BusIndex < Mixer->BusCount) {
    return &Mixer->Buses[BusIndex];
  }
  return NULL;
}

bus* MixerGetFocusedBus(mixer* Mixer) {
  return Mixer->FocusedBus;
}

i32 MixerAddBus(mixer* Mixer, i32 ChannelCount, f32* Buffer) {
  if (Mixer->BusCount < MAX_AUDIO_BUS) {
    bus* Bus = &Mixer->Buses[Mixer->BusCount];
    if (!Buffer) {
      Bus->Buffer = M_Calloc(sizeof(f32), ChannelCount * Mixer->FramesPerBuffer);
      Bus->InternalBuffer = 1;
    }
    else {
      Bus->Buffer = Buffer;
      Bus->InternalBuffer = 0;
    }
    Bus->ChannelCount = ChannelCount;
    Bus->ID = RandomSeed();
    Bus->Pan = V2(1, 1);
    Bus->Db = V2(DB_MIN, DB_MIN);
    Bus->Active = 1;
    Bus->Disabled = 0;
    Bus->ToRemove = 0;
    Bus->MidiInput = 0;
    Bus->Ins = NULL;

    Mixer->BusCount++;
  }
  else {
    fprintf(stderr, "Maximum amount of buses are in use (%i)\n", Mixer->BusCount);
    return Error;
  }
  return NoError;
}

// NOTE(lucas): Returns a reference to the bus that you added. It should be noted, however, that the reference is
// not persistant, in other words the reference can change so that it will point to a bus which you initially did not
// select. Thus, this function should be used with caution.
bus* MixerAddBus0(mixer* Mixer, i32 ChannelCount, f32* Buffer, i32* BusIndex) {
  bus* Bus = NULL;
  i32 Index = Mixer->BusCount;

  if (MixerAddBus(Mixer, ChannelCount, Buffer) == NoError) {
    Bus = &Mixer->Buses[Index];
    if (BusIndex) {
      *BusIndex = Index;
    }
  }
  else {
    if (BusIndex) {
      *BusIndex = -1;
    }
  }
  return Bus;
}

// NOTE(lucas): We mark the bus for later removal. This is to make sure we
// don't simultaneously process and try to free the bus.
i32 MixerRemoveBus(mixer* Mixer, i32 BusIndex) {
  if (BusIndex > MASTER_BUS_INDEX && BusIndex < Mixer->BusCount) {
    bus* Bus = &Mixer->Buses[BusIndex];
    Bus->ToRemove = 1;
  }
  return NoError;
}

i32 MixerAttachInstrumentToBus(mixer* Mixer, i32 BusIndex, instrument* Ins) {
  if (BusIndex > MASTER_BUS_INDEX && BusIndex < MAX_AUDIO_BUS) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->Ins) {
      instrument* Instrument = Bus->Ins;
      InstrumentFree(Instrument);
      Bus->Ins = NULL;
    }
    Bus->Ins = Ins;
  }
  else {
    return Error;
  }
  return NoError;
}

i32 MixerAttachInstrumentToBus0(mixer* Mixer, bus* Bus, instrument* Ins) {
  if (!Bus) {
    return Error;
  }
  if (Bus->Ins) {
    instrument* Instrument = Bus->Ins;
    InstrumentFree(Instrument);
    Bus->Ins = NULL;
  }
  Bus->Ins = Ins;
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
  for (i32 BusIndex = 0; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    // NOTE(lucas): The buffer is cleared elsewhere if it is not internal
    if (Bus->Buffer) {
      if (Bus->InternalBuffer || (Bus->Buffer && BusIndex == MASTER_BUS_INDEX)) {
        ClearFloatBuffer(Bus->Buffer, sizeof(f32) * Bus->ChannelCount * Mixer->FramesPerBuffer);
      }
    }
  }
  TIMER_END();
  return NoError;
}

i32 MixerSumBuses(mixer* Mixer, u8 Playing, f32* OutBuffer, f32* InBuffer) {
  TIMER_START();

  bus* Master = &Mixer->Buses[0];
  if (!Master->Buffer) {
    Master->Buffer = OutBuffer;
    ClearFloatBuffer(Master->Buffer, sizeof(f32) * Master->ChannelCount * Mixer->FramesPerBuffer);
  }

  if (!Master->Active || Master->Disabled || !Master->Buffer) {
    return NoError;
  }

  // Process all buses
  for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    if (Bus->ToRemove) {
      if (Bus->Ins) {
        if (!Bus->Ins->Ready) {
          // We are processing this instrument (loading/unloading), thus we cannot remove it as of yet
          continue;
        }
      }
      RemoveBus(Mixer, BusIndex);
      continue;
    }
    // NOTE(lucas): We do not want to process the bus if we are not playing
    if (!Playing) {
      continue;
    }

    instrument* Ins = Bus->Ins;
    if (Bus->Active && !Bus->Disabled && Bus->Buffer && !Bus->ToRemove) {
      if (Ins) {
        if (Ins->Process && Ins->Ready) {
          Ins->Process(Ins, Bus, Mixer->FramesPerBuffer, Mixer->SampleRate);
        }
      }
    }
  }

  // Early out if we are not playing
  if (!Playing) {
    return NoError;
  }

  // Sum all buses into the master bus
  for (i32 BusIndex = Mixer->BusCount - 1; BusIndex >= 0; --BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    f32* Iter = &Master->Buffer[0];
    if (!Bus->Disabled && !Bus->ToRemove && Bus->Buffer) {
      f32 Frame0 = 0.0f;
      f32 Frame1 = 0.0f;
      v2 Db = V2(0.0f, 0.0f);
      i32 WindowSize = Mixer->FramesPerBuffer;
      for (i32 FrameIndex = 0; FrameIndex < Mixer->FramesPerBuffer; ++FrameIndex) {
        if (Bus->ChannelCount == 2) {
          Frame0 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
          Frame1 = Bus->Buffer[FrameIndex * Bus->ChannelCount + 1];
        }
        else {
          Frame0 = Frame1 = Bus->Buffer[FrameIndex * Bus->ChannelCount];
        }
        if (Bus->Active && BusIndex != MASTER_BUS_INDEX && Bus->InternalBuffer) {
          *(Iter++) += Frame0 * Bus->Pan.X * Master->Pan.X;
          *(Iter++) += Frame1 * Bus->Pan.Y * Master->Pan.Y;
        }
        Db.L += 20.0f * Log10(Abs(Frame0));
        Db.R += 20.0f * Log10(Abs(Frame1));
      }
      Db.L /= WindowSize;
      Db.R /= WindowSize;
      Bus->Db = Db;
    }
  }
  TIMER_END();
  return NoError;
}

i32 MixerRender(mixer* Mixer) {
  const i32 TileSize = 24;
  v2 PrevButtonSize = UIButtonSize;
  UIButtonSize.H = TileSize;
  v3 PrevColorButton = UIColorButton;
  for (i32 BusIndex = 0; BusIndex < Mixer->BusCount; ++BusIndex) {
    bus* Bus = &Mixer->Buses[BusIndex];
    f32 DbFactorL = 1.0f / (1 + Abs(Bus->Db.L));
    f32 DbFactorR = 1.0f / (1 + Abs(Bus->Db.R));
    f32 DbFactorAverage = (DbFactorL + DbFactorR) / 2.0f;
    if (UI_DoBox(UI_ID + Bus->ID, V2(TileSize, TileSize), ColorGain(V3(0.3f, 1.0f, 0.3f), 10 * DbFactorAverage))) {
      Bus->Active = !Bus->Active;
    }
    if (BusIndex > MASTER_BUS_INDEX) {
      UIColorButton = UIColorDecline;
      if (UI_DoTextButton(UI_ID + Bus->ID + 1, "DEL")) {
        MixerRemoveBus(Mixer, BusIndex);
        continue;
      }
      UIColorButton = PrevColorButton;
      u8 ThisFocus = Mixer->FocusedBus == Bus;
      if (UI_DoTextToggle(UI_ID + Bus->ID + 2, "FOC", &ThisFocus)) {
        if (!ThisFocus) {
          Mixer->FocusedBus = NULL;
        }
        else {
          Mixer->FocusedBus = Bus;
        }
      }
      UI_DoTextToggle(UI_ID + Bus->ID + 3, "MID", &Bus->MidiInput);
    }
  }
  UIButtonSize = PrevButtonSize;
  UIColorButton = PrevColorButton;
  return NoError;
}

// NOTE(lucas): We spin and wait for buses to be completely free'd
void MixerFree(mixer* Mixer) {
  TIMER_START();

  u8 Spin = 0;
  u32 SpinCounter = 0;
  (void)SpinCounter;
  u8 InvokedFree = 0;

  do {
    Spin = 0;
    for (i32 BusIndex = 1; BusIndex < Mixer->BusCount; ++BusIndex) {
      bus* Bus = &Mixer->Buses[BusIndex];
      if (!InvokedFree) {
        FreeBus(Mixer, Bus);
      }
      if (Bus->Ins) {
        if (!Bus->Ins->Ready) {
          Spin = 1; // Continue spinning because we are not done processing this bus
        }
      }
    }
    InvokedFree = 1;
    ++SpinCounter;
    sleep(0);
  } while (Spin);

  TIMER_END(
#if 0
    printf("%s: SpinCounter: %i. Done in %g s.\n", __FUNCTION__, SpinCounter, _DeltaTime);
#endif
  );
}
