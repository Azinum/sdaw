// instrument.c

void InstrumentInit(instrument* Ins, instrument_type Type) {
  Ins->UserData.Data = NULL;
  Ins->UserData.Count = 0;
  Ins->Type = Type;
}

void InstrumentFree(instrument* Ins) {
  BufferFree(&Ins->UserData);
  M_Free(Ins, sizeof(instrument));
}
