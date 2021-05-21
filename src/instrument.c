// instrument.c

static void* LoaderThread(void* Instrument);

void* LoaderThread(void* Instrument) {
  instrument* Ins = (instrument*)Instrument;
  if (Ins->InitCb) {
    Ins->InitCb(Ins);
  }
  Ins->Ready = 1;
  pthread_join(Ins->LoadThread, NULL);
  return NULL;
}

instrument* InstrumentCreate(instrument_cb InitCb, instrument_cb FreeCb, instrument_process_cb Process) {
  instrument* Ins = M_Malloc(sizeof(instrument));
  if (Ins) {
    Ins->UserData.Data = NULL;
    Ins->UserData.Count = 0;
    Ins->InitCb = InitCb;
    Ins->FreeCb = FreeCb;
    Ins->Process = Process;
    if (Ins->InitCb) {
      Ins->Ready = 0;
      pthread_create(&Ins->LoadThread, NULL, LoaderThread, (void*)Ins);
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

void InstrumentFree(instrument* Ins) {
  if (Ins->FreeCb) {
    Ins->FreeCb(Ins);
  }
  BufferFree(&Ins->UserData);
  M_Free(Ins, sizeof(instrument));
}
