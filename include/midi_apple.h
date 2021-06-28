// midi_apple.h

#ifndef _MIDI_APPLE_H
#define _MIDI_APPLE_H

#if __APPLE__

i32 MidiAppleInit();

u32 MidiAppleFetchEvents(midi_event* Dest);

i32 MidiAppleOpenDevices();

void MidiAppleCloseDevices();

#endif  // __APPLE__

#endif
