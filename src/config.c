// config.c

#define CONFIG_FILE_NAME ".sdaw"

#define Scan(ScanStatus, Iterator, Format, ...) { \
	u32 NumBytesRead = 0; \
	ScanStatus = sscanf(Iterator, Format "%n", __VA_ARGS__, &NumBytesRead); \
	Iterator += NumBytesRead; \
}

#define MAX_WORD_SIZE 128

static i32 ParseConfig(buffer* Buffer);

i32 ParseConfig(buffer* Buffer) {
  i32 Result = NoError;
  if (Buffer->Count == 0) {
    return Result;
  }
  char Word[MAX_WORD_SIZE] = {};
  char* Iterator = &Buffer->Data[0];
  i32 ScanResult = 0;
  for (;;) {
    Scan(ScanResult, Iterator, "%s", Word);
    if (ScanResult == EOF) {
      break;
    }
    if (!strncmp(Word, S_SampleRate, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_SampleRate);
    }
    else if (!strncmp(Word, S_FramesPerBuffer, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_FramesPerBuffer);
    }
    else if (!strncmp(Word, S_WindowWidth, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_WindowWidth);
    }
    else if (!strncmp(Word, S_WindowHeight, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_WindowHeight);
    }
    else if (!strncmp(Word, S_FullScreen, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_FullScreen);
    }
    else if (!strncmp(Word, S_Vsync, MAX_WORD_SIZE)) {
      Scan(ScanResult, Iterator, "%i", &G_Vsync);
    }
  }
  return Result;
}

// Need to automate this in some way, some day
i32 WriteConfig(const char* Path) {
  FILE* File = fopen(Path, "w");
  if (File) {
    fprintf(File, "%s %i\n", S_SampleRate, G_SampleRate);
    fprintf(File, "%s %i\n", S_FramesPerBuffer, G_FramesPerBuffer);
    fprintf(File, "%s %i\n", S_WindowWidth, G_WindowWidth);
    fprintf(File, "%s %i\n", S_WindowHeight, G_WindowHeight);
    fprintf(File, "%s %i\n", S_FullScreen, G_FullScreen);
    fprintf(File, "%s %i\n", S_Vsync, G_Vsync);

    fclose(File);
  }
  else {
    fprintf(stderr, "Failed to write config file '%s'\n", Path);
    return Error;
  }
  return NoError;
}

i32 LoadConfig() {
  i32 Result = NoError;
  char Path[MAX_PATH_SIZE] = {};
  char* Home = HomePath();

  snprintf(Path, MAX_PATH_SIZE, "%s/.config/sdaw/%s", Home, CONFIG_FILE_NAME);
  FILE* File = fopen(Path, "r");
  if (!File) {
    if (WriteConfig(Path) == NoError) {
      fprintf(stdout, "Default configuration file written to '%s'\n", Path);
    }
    else {
      snprintf(Path, MAX_PATH_SIZE, "%s/%s", Home, CONFIG_FILE_NAME);
      File = fopen(Path, "r");
      if (!File) {
        fprintf(stderr, "Failed to open configuration file '%s'\n", Path);
        if (WriteConfig(Path) == NoError) {
          fprintf(stdout, "Default configuration file written to '%s'\n", Path);
        }
        Result = NoError;
        goto Done;
      }
    }
  }
  if (File) {
    fclose(File);
  }

  buffer Buffer = {};
  if (ReadFile(Path, &Buffer) == NoError) {
    Result = ParseConfig(&Buffer);
    BufferFree(&Buffer);
  }
  else {
    fprintf(stderr, "Failed to parse configuration file '%s'\n", Path);
  }

Done:
  return Result;
}
