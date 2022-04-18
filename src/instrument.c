// instrument.c

static void* LoadThread(void* Instrument);
static void* UnloadThread(void* Instrument);

instrument_handler InsHandler = {0};

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
  if (Type < InsHandler.InstrumentCount) {
    instrument* Ins = M_Malloc(sizeof(instrument));
    if (Ins) {
      instrument_def* InsDef = &InsHandler.Instruments[Type];
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
  return NULL;
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
  Assert(Ins != NULL);
  return Ins->Destroy ? Ins->Destroy(Ins) : NoError;
}

i32 InstrumentDraw(instrument* Ins) {
  Assert(Ins != NULL);
  return Ins->Draw ? Ins->Draw(Ins) : NoError;
}

void InstrumentFree(instrument* Ins) {
  Assert(Ins != NULL);
  Ins->Ready = 0;
  pthread_create(&Ins->LoadThread, NULL, UnloadThread, (void*)Ins);
}

i32 InstrumentHandlerInit() {
  InsHandler.InstrumentCount = MAX_INSTRUMENT_DEF;
  InsHandler.Instruments = M_Malloc(sizeof(instrument_def) * MAX_INSTRUMENT_DEF);
  instrument_def* InsDef = &InsHandler.Instruments[0];
  *InsDef++ = (instrument_def) {"Oscillator Test", OscTestInit, OscTestFree, NULL, OscTestProcess};
  *InsDef++ = (instrument_def) {"Sampler", SamplerInit, SamplerFree, SamplerDraw, SamplerProcess};
  *InsDef++ = (instrument_def) {"Audio Input", AudioInputInit, AudioInputFree, AudioInputDraw, AudioInputProcess};
  return NoError;
}

void InstrumentHandlerFree() {
  M_Free(InsHandler.Instruments, sizeof(instrument_def) * InsHandler.InstrumentCount);
  InsHandler.InstrumentCount = 0;
}
