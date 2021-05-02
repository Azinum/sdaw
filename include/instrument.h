// instrument.h

instrument* InstrumentCreate(instrument_cb InitCb, instrument_cb FreeCb, instrument_process_cb Process);

void InstrumentFree(instrument* Ins);
