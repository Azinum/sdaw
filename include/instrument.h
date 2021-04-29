// instrument.h

// TODO(lucas): Remove
typedef enum instrument_type {
  INSTRUMENT_OSC_TEST,

  MAX_INSTRUMENT_TYPE,
} instrument_type;

void InstrumentInit(instrument* Ins, instrument_type Type);

void InstrumentFree(instrument* Ins);
