// midi_serial.c
// midi from serial connection

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

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

i32 MidiSerialInit() {
  SerialMidi.Mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  SerialMidi.EventCount = 0;
  SerialMidi.Fd = -1;
  SerialMidi.HasInitialized = 1;
  SerialMidi.ShouldExit = 0;
  return NoError;
}

i32 MidiSerialOpenDevices() {
  i32 Result = Error;

  const char* DevPath = "/dev";
  DIR* Directory = NULL;
  struct dirent* Entry = NULL;

  if ((Directory = opendir(DevPath)) != NULL) {
    while ((Entry = readdir(Directory)) != NULL) {
      if (StringContains(Entry->d_name, "midi")) {
        char Path[MAX_PATH_SIZE];
        snprintf(Path, MAX_PATH_SIZE, "%s/%s", DevPath, Entry->d_name);
        // printf("MIDI Device: %s (%s)\n", Entry->d_name, Path);
        Result = OpenSerial(Path);
        break;
      }
    }
    closedir(Directory);
  }
  else {
    return Result;
  }
  return Result;
}

u32 MidiSerialFetchEvents(midi_event* Dest) {
  u32 Count = 0;

  serial_midi_state* Serial = &SerialMidi;

  if (Serial->Fd < 0) {
    return 0;
  }

  if (pthread_mutex_trylock(&Serial->Mutex)) {
    for (u32 EventIndex = 0; EventIndex < Serial->EventCount; ++EventIndex) {
      Dest[EventIndex] = Serial->MidiEvents[EventIndex];
    }
    Count = Serial->EventCount;
    Serial->EventCount = 0;
    pthread_mutex_unlock(&Serial->Mutex);
  }
  return Count;
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

void CloseSerial() {
  SerialMidi.ShouldExit = 1;
  pthread_join(SerialMidi.ReadThread, NULL);
  if (SerialMidi.Fd >= 0) {
    close(SerialMidi.Fd);
  }
}

void MidiSerialCloseDevices() {

}
