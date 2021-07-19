// instrument.c

static void* LoadThread(void* Instrument);
static void* UnloadThread(void* Instrument);

instrument_def Instruments[MAX_INSTRUMENT_DEF] = {
  {"Oscillator Test", OscTestInit, OscTestFree, NULL, OscTestProcess},
  {"Sampler", SamplerInit, SamplerFree, SamplerDraw, SamplerProcess},
  {"Audio Input", AudioInputInit, AudioInputFree, AudioInputDraw, AudioInputProcess},
};

void* LoadThread(void* Instrument) {
  TIMER_START();

  instrument* Ins = (instrument*)Instrument;
  if (Ins->Init) {
    Ins->Init(Ins);
  }
  Ins->Ready = 1;
  pthread_join(Ins->LoadThread, NULL);

  TIMER_END();
  return NULL;
}

void* UnloadThread(void* Instrument) {
  TIMER_START();

  instrument* Ins = (instrument*)Instrument;
  InstrumentDestroy(Ins);

  BufferFree(&Ins->UserData);
  M_Free(Ins, sizeof(instrument));

  Ins->Ready = 1;
  pthread_join(Ins->LoadThread, NULL);

  TIMER_END();
  return NULL;
}

instrument* InstrumentCreate(instrument_def_type Type) {
  instrument* Ins = M_Malloc(sizeof(instrument));
  if (Ins) {
    instrument_def* InsDef = &Instruments[Type];
    Ins->UserData.Data = NULL;
    Ins->UserData.Count = 0;
    Ins->Init = InsDef->Init;
    Ins->Destroy = InsDef->Destroy;
    Ins->Draw = InsDef->Draw;
    Ins->Process = InsDef->Process;
    if (Ins->Init) {
      Ins->Ready = 0;
      pthread_create(&Ins->LoadThread, NULL, LoadThread, (void*)Ins);
    }
    else {
      Ins->Ready = 1;
    }
  }
  else {
    fprintf(stderr, "Failed to create instrument (out of memory?)\n");
    return NULL;
  }
  return Ins;
}

i32 InstrumentAllocUserData(instrument* Ins, i32 Size) {
  i32 Result = NoError;
  void* Data = M_Calloc(Size, 1);
  if (Data) {
    Ins->UserData.Data = Data;
    Ins->UserData.Count = Size;
  }
  else {
    fprintf(stderr, "Failed to allocate memory for user data\n");
    Result = Error;
  }
  return Result;
}

i32 InstrumentDestroy(instrument* Ins) {
  if (Ins) {
    return Ins->Destroy ? Ins->Destroy(Ins) : NoError;
  }
  return NoError;
}

i32 InstrumentDraw(instrument* Ins) {
  if (Ins) {
    return Ins->Draw ? Ins->Draw(Ins) : NoError;
  }
  return NoError;
}

void InstrumentFree(instrument* Ins) {
  Ins->Ready = 0;
  pthread_create(&Ins->LoadThread, NULL, UnloadThread, (void*)Ins);
}
