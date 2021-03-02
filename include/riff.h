// riff.h

typedef struct wave_header {
  char RiffId[4];
  i32 Size;
  char WaveId[4];
} __attribute__((packed)) wave_header;

typedef struct wave_format {
  char FormatId[4];
  i32 Size;
  i16 Type;
  i16 ChannelCount;
  i32 SampleRate;
  i32 DataRate;
  i16 DataBlockSize;
  i16 BitsPerSample;
} __attribute__((packed)) wave_format;

typedef struct wave_chunk {
  char ChunkId[4];
  i32 Size;
} __attribute__((packed)) wave_chunk;

#define WaveMinSize ((i32)(sizeof(wave_header) + sizeof(wave_format) + sizeof(wave_chunk)))

#define FORMAT_PCM 0x1

static i32 StoreWAVE(const char* Path, audio_source* Source);

static i32 LoadWAVE(const char* Path, audio_source* Source);
