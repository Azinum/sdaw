// midi.h

#ifndef _MIDI_H
#define _MIDI_H

#define MAX_MIDI_EVENT 512

typedef union midi_event {
  struct {
    u8 Message;
    u8 A;
    u8 B;
    u8 C; // Unused
  };
  struct {
    u32 Data;
  };
} midi_event;

enum midi_message_event {
  MIDI_NOTE_ON = 0x90,
  MIDI_NOTE_ON_HIGH = 0x9,
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_OFF_HIGH = 0x8,
};

typedef struct midi_handle {
  const char* Handle;
  i32 (*Init)();
  u32 (*FetchEvents)(midi_event* Dest);
  i32 (*OpenDevices)();
  void (*CloseDevices)();
} midi_handle;

typedef enum midi_handle_type {
  MIDI_HANDLE_NULL,
  MIDI_HANDLE_SERIAL,
  MIDI_HANDLE_APPLE,

  MAX_MIDI_HANDLE,
} midi_handle_type;

extern midi_handle MidiHandles[MAX_MIDI_HANDLE];

extern midi_handle MidiHandle;

i32 MidiInitHandle(midi_handle_type HandleType);

i32 MidiInit();

u32 MidiFetchEvents(midi_event* Dest);

i32 MidiOpenDevices();

void MidiCloseDevices();

#endif
