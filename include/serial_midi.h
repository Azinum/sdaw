// serial_midi.h

#define MAX_MIDI_EVENT 512

typedef union midi_event {
  struct {
    u8 Message;
    u8 A;
    u8 B;
    u8 C;
  };
  struct {
    u32 Data;
  };
} midi_event;

void SerialMidiInit();

i32 OpenSerial(const char* Device);

u32 FetchMidiEvents(midi_event* Dest);  // Fetch midi events. Returns the number of events recieved.

void CloseSerial();
