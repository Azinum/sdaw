// riff.c

#include <limits.h>

#define SAMPLE_RATE 44100

static char RiffId[] = {'R', 'I', 'F', 'F'};
static char WaveId[] = {'W', 'A', 'V', 'E'};
static char DataChunkId[] = {'d', 'a', 't', 'a'};
static char ChunkListId[] = {'L', 'I', 'S', 'T'};
static char FormatId[] = {'f', 'm', 't', ' '};

static void PrintWaveHeader(wave_header* Header) {
  printf(
    "RiffId:  %.4s\n"
    "Size:    %i\n"
    "WaveId:  %.4s\n"
    ,
    Header->RiffId,
    Header->Size,
    Header->WaveId
  );
}

static void PrintWaveFormat(wave_format* Header) {
  printf(
    "FormatId:      %.4s\n"
    "Size:          %i\n"
    "Type:          0x%x\n"
    "ChannelCount:  %i\n"
    "SampleRate:    %i\n"
    "DataRate:      %i\n"
    "DataBlockSize: %i\n"
    "BitsPerSample: %i\n"
    ,
    Header->FormatId,
    Header->Size,
    Header->Type,
    Header->ChannelCount,
    Header->SampleRate,
    Header->DataRate,
    Header->DataBlockSize,
    Header->BitsPerSample
  );
}

static void PrintWaveChunk(wave_chunk* Header) {
  printf(
    "ChunkId: %.4s\n"
    "Size:    %i\n"
    ,
    Header->ChunkId,
    Header->Size
  );
}

static i32 ValidateWaveHeader(wave_header* Header) {
  if (strncmp(Header->RiffId, RiffId, ArraySize(RiffId)) != 0) {
    return Error;
  }
  if (strncmp(Header->WaveId, WaveId, ArraySize(WaveId)) != 0) {
    return Error;
  }
  return NoError;
}

static i32 ValidateWaveFormat(wave_format* Header) {
  if (strncmp(Header->FormatId, FormatId, ArraySize(FormatId)) != 0) {
    return Error;
  }
  if (Header->Type != FORMAT_PCM) {
    return Error;
  }
  return NoError;
}

static i32 ValidateWaveChunk(wave_chunk* Header, i32* HasListTag) {
  if (!strncmp(Header->ChunkId, DataChunkId, ArraySize(DataChunkId))) {
    return NoError;
  }
  if (!strncmp(Header->ChunkId, ChunkListId, ArraySize(ChunkListId))) {
    if (HasListTag) {
      *HasListTag = 1;
    }
    return NoError;
  }
  return Error;
}

static i32 IterateWaveFile(void* Dest, i32 Size, FILE* File, const char* Path) {
  i32 ReadSize = 0;

  ReadSize = fread(Dest, 1, Size, File);

  if (ReadSize != Size) {
    fprintf(stderr, "Failed to read WAVE file '%s'\n", Path);
    return Error;
  }
  return NoError;
}

static i32 ConvertToFloatBuffer(float* OutBuffer, i16* InBuffer, i32 SampleCount) {
  for (i32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] / 32768.0f;
  }
  return NoError;
}

static i32 ConvertTo16Buffer(i16* OutBuffer, float* InBuffer, i32 SampleCount) {
  for (i32 SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex) {
    *OutBuffer++ = InBuffer[SampleIndex] * INT16_MAX;
  }
  return NoError;
}

static void InitWaveHeader(wave_header* Header, i32 Size) {
  strncpy(Header->RiffId, RiffId, ArraySize(RiffId));
  Header->Size = Size;
  strncpy(Header->WaveId, WaveId, ArraySize(WaveId));
}

static void InitWaveFormat(wave_format* Header, i32 SampleRate, i32 ChannelCount, i16 BitsPerSample) {
  strncpy(Header->FormatId, FormatId, ArraySize(FormatId));
  Header->Size = 16;
  Header->Type = FORMAT_PCM;
  Header->ChannelCount = ChannelCount;
  Header->SampleRate = SampleRate;
  Header->DataRate = (SampleRate * ChannelCount * BitsPerSample) / 8;
  Header->DataBlockSize = BitsPerSample / 8;
  Header->BitsPerSample = BitsPerSample;
}

static void InitWaveDataChunk(wave_chunk* Header, i32 Size) {
  strncpy(Header->ChunkId, DataChunkId, ArraySize(DataChunkId));
  Header->Size = Size;
}

static i32 StoreWAVE(const char* Path, audio_source* Source) {
  i32 Result = NoError;
  FILE* File = fopen(Path, "w");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    return Error;
  }

  i16 BitsPerSample = 16;
  i16 DataBlockSize = BitsPerSample / 8;
  i32 DataChunkSize = Source->SampleCount * Source->ChannelCount * DataBlockSize;
  i32 TotalSize = WaveMinSize + DataChunkSize - sizeof(wave_chunk);

  wave_header WaveHeader;
  InitWaveHeader(&WaveHeader, TotalSize);

  wave_format WaveFormat;
  InitWaveFormat(&WaveFormat, SAMPLE_RATE, Source->ChannelCount, BitsPerSample);

  wave_chunk WaveChunk;
  InitWaveDataChunk(&WaveChunk, DataChunkSize);

  if ((Result = ValidateWaveHeader(&WaveHeader)) != NoError) {
    goto Done;
  }

  if ((Result = ValidateWaveFormat(&WaveFormat)) != NoError) {
    goto Done;
  }

  if ((Result = ValidateWaveChunk(&WaveChunk, NULL)) != NoError) {
    goto Done;
  }

#if 0
  printf("WAVE file to store '%s':\n", Path);
  printf("===\n");
  PrintWaveHeader(&WaveHeader);
  printf("===\n");
  PrintWaveFormat(&WaveFormat);
  printf("===\n");
  PrintWaveChunk(&WaveChunk);
#endif
  fwrite(&WaveHeader, 1, sizeof(wave_header), File);
  fwrite(&WaveFormat, 1, sizeof(wave_format), File);
  fwrite(&WaveChunk, 1, sizeof(wave_chunk), File);

#if 0
  i16* Buffer = malloc(sizeof(i16) * Source->SampleCount);
  i16* Iter = Buffer;
  ConvertTo16Buffer(Buffer, Source->Buffer, Source->SampleCount);
  for (i32 SampleIndex = 0; SampleIndex < Source->SampleCount; ++SampleIndex) {
    fwrite(Iter++, 1, sizeof(i16), File);
  }
  free(Buffer);
#else
  float* Iter = Source->Buffer;
  for (i32 SampleIndex = 0; SampleIndex < Source->SampleCount; ++SampleIndex) {
    i16 Sample = (i16)(*(Iter++) * INT16_MAX);
    fwrite(&Sample, 1, sizeof(i16), File);
  }
#endif
Done:
  fclose(File);
  return Result;
}

static i32 LoadWAVE(const char* Path, audio_source* Source) {
  i32 Result = NoError;
  FILE* File = fopen(Path, "r");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    return Error;
  }

  Assert(WaveMinSize == 44);

  fseek(File, 0, SEEK_END);
  i32 FileSize = ftell(File);
  fseek(File, 0, SEEK_SET);
  (void)FileSize;

  if (FileSize < WaveMinSize) {
    fprintf(stderr, "Invalid WAVE file '%s'\n", Path);
    Result = Error;
    goto Done;
  }

  wave_header WaveHeader;
  if (IterateWaveFile(&WaveHeader, sizeof(wave_header), File, Path) != NoError) {
    Result = Error;
    goto Done;
  }

  if ((Result = ValidateWaveHeader(&WaveHeader)) != NoError) {
    goto Done;
  }

  wave_format WaveFormat;
  if (IterateWaveFile(&WaveFormat, sizeof(wave_format), File, Path) != NoError) {
    Result = Error;
    goto Done;
  }

  if ((Result = ValidateWaveFormat(&WaveFormat)) != NoError) {
    goto Done;
  }

  wave_chunk WaveChunk;
  i32 ListTag = 0;
  do {
    if (IterateWaveFile(&WaveChunk, sizeof(wave_chunk), File, Path) != NoError) {
      Result = Error;
      goto Done;
    }

    ListTag = 0;
    if ((Result = ValidateWaveChunk(&WaveChunk, &ListTag)) != NoError) {
      goto Done;
    }
    // NOTE(lucas): If there is a list tag in this chunk, skip it for now. Might want to use the contents of the list metadata later on.
    if (ListTag) {
      i32 ListTagSize = WaveChunk.Size;
      fseek(File, ListTagSize, SEEK_CUR);
    }
  } while (ListTag);

  i32 SampleCount = WaveFormat.ChannelCount * (WaveChunk.Size / WaveFormat.DataBlockSize);
  u32 BufferSize = WaveChunk.Size;
  void* Buffer = M_Malloc(BufferSize);
  if (!Buffer) {
    fprintf(stderr, "Failed to allocate sample buffer\n");
    Result = Error;
    goto Done;
  }

  if ((Result = IterateWaveFile(Buffer, WaveChunk.Size, File, Path)) != NoError) {
    fprintf(stderr, "Failed to read sample buffer\n");
    goto Done;
  }

  Source->Buffer = M_Malloc(sizeof(float) * SampleCount);
  Source->SampleCount = SampleCount;
  Source->ChannelCount = WaveFormat.ChannelCount;
  ConvertToFloatBuffer(Source->Buffer, (i16*)Buffer, Source->SampleCount);
  M_Free(Buffer, BufferSize);

#if 0
  printf("Loaded WAVE file '%s':\n", Path);
  printf("===\n");
  printf("SampleCount: %i\n", SampleCount);
  printf("===\n");
  PrintWaveHeader(&WaveHeader);
  printf("===\n");
  PrintWaveFormat(&WaveFormat);
  printf("===\n");
  PrintWaveChunk(&WaveChunk);
#else
  (void)PrintWaveHeader;
  (void)PrintWaveFormat;
  (void)PrintWaveChunk;
#endif
Done:
  fclose(File);
  return Result;
}
