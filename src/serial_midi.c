// serial_midi.c

#include <unistd.h>
#include <fcntl.h>

typedef struct serial_midi_state {
  pthread_mutex_t Mutex;
  pthread_t ReadThread;
  midi_event MidiEvents[MAX_MIDI_EVENT];
  u32 EventCount;
  i32 Fd;
  u8 HasInitialized;
  u8 ShouldExit;
} serial_midi_state;

static serial_midi_state SerialMidi = {0};

static void* SerialRead(void* State);
static i32 ReadEvent(i32 Fd, midi_event* Event);

void* SerialRead(void* State) {
  serial_midi_state* Serial = (serial_midi_state*)State;
  u32 ReadEvents = 0;
  const u32 MaxReadEvents = 32;
  while (!Serial->ShouldExit) {
    if (pthread_mutex_trylock(&Serial->Mutex)) {
      midi_event Event;
      while (Serial->EventCount < MAX_MIDI_EVENT && ReadEvents < MaxReadEvents && !Serial->ShouldExit) {
        if (ReadEvent(Serial->Fd, &Event)) {
          Serial->MidiEvents[Serial->EventCount++] = Event;
          ReadEvents++;
        }
        else {
          break;
        }
      }
      ReadEvents = 0;
      pthread_mutex_unlock(&Serial->Mutex);
    }
    sleep(0);
  }
  return NULL;
}

i32 ReadEvent(i32 Fd, midi_event* Event) {
  return (read(Fd, &Event->Data, 3)) == 3;
}

void SerialMidiInit() {
  SerialMidi.Mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  SerialMidi.EventCount = 0;
  SerialMidi.Fd = -1;
  SerialMidi.HasInitialized = 1;
  SerialMidi.ShouldExit = 0;
}

i32 OpenSerial(const char* Device) {
  Assert(SerialMidi.HasInitialized == 1);
  i32 Flags = O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK;
  i32 FileDescriptor = open(Device, Flags);
  if (FileDescriptor < 0) {
    fprintf(stderr, "Failed to open device '%s'\n", Device);
    return Error;
  }
  else {
    SerialMidi.Fd = FileDescriptor;
    pthread_create(&SerialMidi.ReadThread, NULL, SerialRead, (void*)&SerialMidi);
  }
  return NoError;
}

u32 FetchMidiEvents(midi_event* Dest) {
  u32 Count = 0;
  serial_midi_state* Serial = &SerialMidi;
  if (pthread_mutex_trylock(&Serial->Mutex)) {
    for (i32 EventIndex = 0; EventIndex < Serial->EventCount; ++EventIndex) {
      Dest[EventIndex] = Serial->MidiEvents[EventIndex];
    }
    Count = Serial->EventCount;
    Serial->EventCount = 0;
    pthread_mutex_unlock(&Serial->Mutex);
  }
  return Count;
}

void CloseSerial() {
  if (SerialMidi.Fd >= 0) {
    close(SerialMidi.Fd);
  }
  SerialMidi.ShouldExit = 1;
  pthread_join(SerialMidi.ReadThread, NULL);
}
