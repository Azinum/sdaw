// instrument.c

instrument* InstrumentCreate(instrument_cb InitCb, instrument_cb FreeCb, instrument_process_cb Process) {
  instrument* Ins = M_Malloc(sizeof(instrument));
  if (Ins) {
    Ins->UserData.Data = NULL;
    Ins->UserData.Count = 0;
    Ins->InitCb = InitCb;
    Ins->FreeCb = FreeCb;
    Ins->Process = Process;
    if (Ins->InitCb) {
      Ins->InitCb(Ins);
    }
  }
  else {
    // TODO: Handle
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
