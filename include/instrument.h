// instrument.h

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

typedef struct instrument_def {
  const char* Name;
  instrument_cb Init;
  instrument_cb Destroy;
  instrument_cb Draw;
  instrument_process_cb Process;
} instrument_def;

typedef enum instrument_def_type {
  INSTRUMENT_OSC_TEST,
  INSTRUMENT_SAMPLER,
  INSTRUMENT_AUDIO_INPUT,

  MAX_INSTRUMENT_DEF,
} instrument_def_type;

typedef struct instrument_handler {
  instrument_def* Instruments;
  u32 InstrumentCount;
} instrument_handler;

extern instrument_handler InsHandler;

instrument* InstrumentCreate(instrument_def_type Type);

i32 InstrumentAllocUserData(instrument* Ins, i32 Size);

i32 InstrumentDestroy(instrument* Ins);

i32 InstrumentDraw(instrument* Ins);

void InstrumentFree(instrument* Ins);

i32 InstrumentHandlerInit();

void InstrumentHandlerFree();

#endif
