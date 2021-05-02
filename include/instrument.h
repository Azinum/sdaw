// instrument.h

instrument* InstrumentCreate(instrument_cb InitCb, instrument_cb FreeCb, instrument_process_cb Process);

i32 InstrumentAllocUserData(instrument* Ins, i32 Size);

void InstrumentFree(instrument* Ins);
