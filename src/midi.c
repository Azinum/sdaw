// midi.c

midi_handle MidiHandles[MAX_MIDI_HANDLE] = {
  {"Null MIDI"},
  {"Serial MIDI", MidiSerialInit, MidiSerialFetchEvents, MidiSerialOpenDevices, MidiSerialCloseDevices},
#if __APPLE__
  {"Core MIDI", MidiAppleInit, MidiAppleFetchEvents, MidiAppleOpenDevices, MidiAppleCloseDevices},
#else
  {"Core MIDI (not avaliable on your system)"},
#endif
};

midi_handle MidiHandle;
u8 MidiHandleInitialized = 0;

i32 MidiInitHandle(midi_handle_type HandleType) {
  if (HandleType >= 0 && HandleType < MAX_MIDI_HANDLE) {
    MidiHandle = MidiHandles[HandleType];
    MidiHandleInitialized = 1;
    return NoError;
  }
  fprintf(stderr, "Invalid MIDI handle id (got %i, expected a value from %i to %i)\n", HandleType, 0, MAX_MIDI_HANDLE);
  return Error;
}

i32 MidiInit() {
  CALL(MidiHandle.Init);
  return NoError;
}

u32 MidiFetchEvents(midi_event* Dest) {
  if (MidiHandle.FetchEvents)
    return MidiHandle.FetchEvents(Dest);
  return 0;
}

i32 MidiOpenDevices() {
  CALL(MidiHandle.OpenDevices);
  return NoError;
}

void MidiCloseDevices() {
  CALL(MidiHandle.CloseDevices);
}
