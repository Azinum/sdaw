// riff.c

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
  char RiffId[] = {'R', 'I', 'F', 'F'};
  char WaveId[] = {'W', 'A', 'V', 'E'};
  if (strncmp(Header->RiffId, RiffId, ARR_SIZE(RiffId)) != 0) {
    return Error;
  }
  if (strncmp(Header->WaveId, WaveId, ARR_SIZE(WaveId)) != 0) {
    return Error;
  }
  return NoError;
}

static i32 ValidateWaveFormat(wave_format* Header) {
  char FormatId[] = {'f', 'm', 't', ' '};
  if (strncmp(Header->FormatId, FormatId, ARR_SIZE(FormatId)) != 0) {
    return Error;
  }
  if (Header->Type != FORMAT_PCM) {
    return Error;
  }
  return NoError;
}

static i32 ValidateWaveChunk(wave_chunk* Header, i32* ListTag) {
  char ChunkId[] = {'d', 'a', 't', 'a'};
  char ChunkListId[] = {'L', 'I', 'S', 'T'};
  if (!strncmp(Header->ChunkId, ChunkId, ARR_SIZE(ChunkId))) {
    return NoError;
  }
  if (!strncmp(Header->ChunkId, ChunkListId, ARR_SIZE(ChunkListId))) {
    *ListTag = 1;
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
  void* Buffer = malloc(WaveChunk.Size);
  if (!Buffer) {
    fprintf(stderr, "Failed to allocate sample buffer\n");
    Result = Error;
    goto Done;
  }

  if ((Result = IterateWaveFile(Buffer, WaveChunk.Size, File, Path)) != NoError) {
    fprintf(stderr, "Failed to read sample buffer\n");
    goto Done;
  }

  Source->Buffer = malloc(sizeof(float) * SampleCount);
  Source->SampleCount = SampleCount;
  Source->ChannelCount = WaveFormat.ChannelCount;
  ConvertToFloatBuffer(Source->Buffer, (i16*)Buffer, Source->SampleCount);
  free(Buffer);

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

