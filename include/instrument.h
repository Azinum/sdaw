// instrument.h

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

typedef struct instrument_def {
  const char* Name;
  instrument_cb InitCb;
  instrument_cb FreeCb;
  instrument_process_cb ProcessCb;
} instrument_def;

enum instrument_def_type {
  OSC_TEST,
  SAMPLER,
  AUDIO_INPUT,

  MAX_INSTRUMENT_DEF,
};

instrument* InstrumentCreate(instrument_cb InitCb, instrument_cb FreeCb, instrument_process_cb Process);

i32 InstrumentAllocUserData(instrument* Ins, i32 Size);

void InstrumentFree(instrument* Ins);

#endif
