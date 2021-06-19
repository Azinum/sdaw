// midi_serial.h

#ifndef _MIDI_SERIAL_H
#define _MIDI_SERIAL_H

i32 MidiSerialInit();

u32 MidiSerialFetchEvents(midi_event* Dest);

i32 MidiSerialOpenDevices();

i32 OpenSerial(const char* Device);

u32 FetchMidiEvents(midi_event* Dest);  // Fetch midi events. Returns the number of events recieved.

void CloseSerial();

void MidiSerialCloseDevices();

#endif
