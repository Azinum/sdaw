// midi_apple.c

#if __APPLE__

#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>

#define MAX_INPUT_DEVICE 8
#define MAX_SOURCE_PORT 8

typedef struct midi_endpoint {
  MIDIEndpointRef Device;
  buffer Name;
} midi_endpoint;

typedef struct midi_source_port {
  MIDIEndpointRef Device;
  MIDIPortRef Port;
} midi_source_port;

typedef struct midi_apple_state {
  midi_endpoint InputDevices[MAX_INPUT_DEVICE];
  u32 InputDeviceCount;

  midi_source_port SourcePorts[MAX_SOURCE_PORT];
  u32 SourcePortCount;

  midi_event MidiEvents[MAX_MIDI_EVENT];
  u32 EventCount;

  MIDIClientRef Client;
} midi_apple_state;

midi_apple_state MidiApple;

static i32 GetDeviceName(MIDIObjectRef Object, buffer* Buffer);
static MIDIClientRef GetClientRef();
static void NotifyCallback(const MIDINotification* Message, void* RefCon);
static void ReadCallback(const MIDIPacketList* PacketList, void* ReadRefCon, void* SourceRefCon);
static i32 AddInputDevice(MIDIEndpointRef InputDevice, buffer Name);
static i32 AddSourcePort(MIDIEndpointRef InputDevice, MIDIPortRef Port);
static i32 OpenMidiReference(MIDIEndpointRef Device, buffer Name, u8 IsInput);
static void PushMidiEvent(midi_event Event);

i32 GetDeviceName(MIDIObjectRef Object, buffer* Buffer) {
  CFStringRef Name = NULL;
  if (MIDIObjectGetStringProperty(Object, kMIDIPropertyName, &Name) != noErr) {
    return Error;
  }
  i32 Length = CFStringGetLength(Name) + 1;
  Buffer->Data = M_Malloc(Length);
  Buffer->Count = Length;

	CFStringGetCString(Name, Buffer->Data, Buffer->Count, 0);
	CFRelease(Name);
  return NoError;
}

MIDIClientRef GetClientRef() {
  if (!MidiApple.Client) {
    MIDIClientCreate(CFSTR("MIDI client"), NotifyCallback, NULL, &MidiApple.Client);
  }
  return MidiApple.Client;
}

void NotifyCallback(const MIDINotification* Message, void* RefCon) {
  switch (Message->messageID) {
    case kMIDIMsgObjectRemoved: {
      printf("Device removed\n");
      break;
    }
    default:
      printf("Unhandled MIDI message\n");
      break;
  }
}

void ReadCallback(const MIDIPacketList* PacketList, void* ReadRefCon, void* SourceRefCon) {
  const MIDIPacket* Packet = &PacketList->packet[0];
  u32 PacketLength = 0;

  for (u32 PacketIndex = 0; PacketIndex < PacketList->numPackets; ++PacketIndex, Packet = MIDIPacketNext(Packet)) {
    PacketLength = Packet->length;

    u16 Index = 0;
    while (Index < PacketLength) {
      u16 Size = 0;
      u8 Stat = Packet->data[Index];
      if (Stat < 0xc0) {
        Size = 3;
      }
      else if (Stat < 0xe0) {
        Size = 2;
      }
      else if (Stat < 0xf0) {
        Size = 3;
      }
      else if (Stat == 0xf0) {
        // Handle
        Size = 0;
        Index = PacketLength;
        goto Done;
      }
      else if (Stat < 0xf3) {
        Size = 3;
      }
      else if (Stat == 0xf3) {
        Size = 2;
      }
      else {
        Size = 1;
      }

      u8 Channel = Stat & 0xf;
      const u8 Command = Stat & 0xf0;
      const u8 A = Packet->data[Index + 1];
      const u8 B = Packet->data[Index + 2];

      switch (Command) {
        case MIDI_NOTE_ON:
        case MIDI_NOTE_OFF: {
          midi_event Event = {0};
          Event.Message = Command;
          Event.A = A;
          Event.B = B;
          PushMidiEvent(Event);
          break;
        }
        case 0:
          break;
        default:
          break;
      }
Done:
      Index += Size;
    }
  }
}

i32 AddInputDevice(MIDIEndpointRef InputDevice, buffer Name) {
  if (MidiApple.InputDeviceCount < MAX_INPUT_DEVICE) {
    midi_endpoint Source = (midi_endpoint) {
      .Device = InputDevice,
      .Name = Name,
    };
    MidiApple.InputDevices[MidiApple.InputDeviceCount++] = Source;
    return NoError;
  }
  // TODO(lucas): Use dynamic list of devices
  return Error;
}

i32 AddSourcePort(MIDIEndpointRef InputDevice, MIDIPortRef Port) {
  if (MidiApple.SourcePortCount < MAX_SOURCE_PORT) {
    midi_source_port SourcePort = (midi_source_port) {
      .Device = InputDevice,
      .Port = Port,
    };
    MidiApple.SourcePorts[MidiApple.SourcePortCount++] = SourcePort;
    return NoError;
  }
  return Error;
}

i32 OpenMidiReference(MIDIEndpointRef Device, buffer Name, u8 IsInput) {
  MIDIClientRef Client = GetClientRef();
  MIDIPortRef Port = 0;

  CFStringRef PortName = CFStringCreateWithCString(0, Name.Data, kCFStringEncodingASCII);
  if (IsInput) {
    MIDIInputPortCreate(MidiApple.Client, PortName, &ReadCallback, NULL, &Port);
  }
  else {
    // TODO(lucas): Implement
  }
  MIDIPortConnectSource(Port, Device, Name.Data);
  AddSourcePort(Device, Port);
  CFRelease(PortName);
  return NoError;
}

void PushMidiEvent(midi_event Event) {
  if (MidiApple.EventCount < MAX_MIDI_EVENT) {
    MidiApple.MidiEvents[MidiApple.EventCount++] = Event;
  }
}

i32 MidiAppleInit() {
  MidiApple.InputDeviceCount = 0;
  MidiApple.SourcePortCount = 0;
  MidiApple.EventCount = 0;
  MidiApple.Client = 0;
  return NoError;
}

u32 MidiAppleFetchEvents(midi_event* Dest) {
  u32 Count = MidiApple.EventCount;
  for (u32 EventIndex = 0; EventIndex < MidiApple.EventCount; ++EventIndex) {
    midi_event Event = MidiApple.MidiEvents[EventIndex];
    Dest[EventIndex] = Event;
  }
  MidiApple.EventCount = 0;
  return Count;
}

i32 MidiAppleOpenDevices() {
  i32 Result = NoError;

  ItemCount DeviceCount = MIDIGetNumberOfDevices();
  for (ItemCount Index = 0; Index < DeviceCount; ++Index) {
    MIDIDeviceRef Device = MIDIGetDevice(Index);
    buffer DeviceName = {0};
    if ((Result = GetDeviceName(Device, &DeviceName)) != NoError) {
      goto EndOfLoop;
    }
    i32 Offline = 0;
    MIDIObjectGetIntegerProperty(Device, kMIDIPropertyOffline, &Offline);

    // printf("Device name: %.*s (online: %s)\n", DeviceName.Count, DeviceName.Data, TruthString(!Offline));

    if (!Offline) {
      ItemCount EntityCount = MIDIDeviceGetNumberOfEntities(Device);

      for (ItemCount EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
        MIDIEntityRef Entity = MIDIDeviceGetEntity(Device, EntityIndex);
        buffer EntityName = {0};
        GetDeviceName(Entity, &EntityName);

        // printf("Entity: %.*s\n", EntityName.Count, EntityName.Data);

        ItemCount SourceCount = MIDIEntityGetNumberOfSources(Entity);
        for (i32 SourceIndex = 0; SourceIndex < SourceCount; ++SourceIndex) {
          MIDIEndpointRef Source = MIDIEntityGetSource(Entity, SourceIndex);
          buffer SourceName = {0};
          GetDeviceName(Source, &SourceName);
          AddInputDevice(Source, SourceName);
          OpenMidiReference(Source, SourceName, 1 /* input */);
          // printf("Source: %.*s\n", SourceName.Count, SourceName.Data);
        }
        BufferFree(&EntityName);
      }
    }

EndOfLoop:
    BufferFree(&DeviceName);
  }
  return Result;
}

void MidiAppleCloseDevices() {
  for (i32 Index = 0; Index < MidiApple.InputDeviceCount; ++Index) {
    midi_endpoint* InputDevice = &MidiApple.InputDevices[Index];
    // Clean up midi reference endpoint(s)!!!
    BufferFree(&InputDevice->Name);
  }

  for (i32 Index = 0; Index < MidiApple.SourcePortCount; ++Index) {
    midi_source_port* SourcePort = &MidiApple.SourcePorts[Index];
    // Clean up!!!
  }
}

#endif  // __APPLE__
