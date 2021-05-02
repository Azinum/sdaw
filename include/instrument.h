// instrument.h

// TODO(lucas): Remove
typedef enum instrument_type {
  INSTRUMENT_OSC_TEST,

  MAX_INSTRUMENT_TYPE,
} instrument_type;

instrument* InstrumentCreate(instrument_type Type, instrument_callback InitCb, instrument_callback FreeCb);

void InstrumentFree(instrument* Ins);
