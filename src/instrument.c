// instrument.c

instrument* InstrumentCreate(instrument_type Type, instrument_callback InitCb, instrument_callback FreeCb) {
  instrument* Ins = M_Malloc(sizeof(instrument));
  if (Ins) {
    Ins->UserData.Data = NULL;
    Ins->UserData.Count = 0;
    Ins->Type = Type;
    Ins->InitCb = InitCb;
    Ins->FreeCb = FreeCb;
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
