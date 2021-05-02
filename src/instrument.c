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

void InstrumentFree(instrument* Ins) {
  if (Ins->FreeCb) {
    Ins->FreeCb(Ins);
  }
  BufferFree(&Ins->UserData);
  M_Free(Ins, sizeof(instrument));
}
