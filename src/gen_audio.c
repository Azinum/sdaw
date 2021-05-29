// gen_audio.c
// image to audio generator

typedef enum sampling_strategy {
  S_DEFAULT = 0,
  S_EXPERIMENTAL,

  MAX_SAMPLING_STRATEGY,
} sampling_strategy;

static const char* SamplingDesc[MAX_SAMPLING_STRATEGY] = {
  "default",
  "experimental",
};

typedef struct args {
  i32 FrameCopies;
  i32 ChannelCount;
  float WDenom;
  float HDenom;
  i32 XSpeed;
  i32 YSpeed;
  i32 SamplingStrategy;
} args;

static i32 GenerateSineWave(audio_source* Source, float Amp, float Freq);
static i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, float Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, float WDenom, float HDenom, i32 XSpeed, i32 YSpeed, i32 SamplingStrategy);

i32 GenerateSineWave(audio_source* Source, float Amp, float Freq) {
  float* Iter = Source->Buffer;
  for (i32 SampleIndex = 0; SampleIndex < Source->SampleCount; ++SampleIndex) {
    float Frame = Amp * sin((SampleIndex * Freq * 2 * PI32) / G_SampleRate);
    Amp = Lerp(Amp, 0.0f, 0.00005f);
    if (Source->ChannelCount == 2) {
      *(Iter++) += Frame;
      *(Iter++) += Frame;
      continue;
    }
    *(Iter++) += Frame;
  }
  return NoError;
}

i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, float Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, float WDenom, float HDenom, i32 XSpeed, i32 YSpeed, i32 SamplingStrategy) {
  i32 Result = NoError;

  i32 Width = Image->Width / WDenom;
  i32 Height = Image->Height / HDenom;

  // TODO(lucas): This size is arbitrary, calculate the exact number of padding needed.
  i32 Padding = 2 * 4096; // NOTE(lucas): Use padding to not overflow the sample buffer.
  i32 SampleCount = ((Width / (float)XSpeed) * (Height / (float)YSpeed) * ChannelCount * FrameCopies) + Padding;
  float Tick = 0.0f;

#if 0
  float TimeInSeconds = (float)SampleCount / SampleRate;
  i32 TimeInMinutes = (i32)TimeInSeconds / 60;
  printf(
    "Generating audio file '%s' from image file '%s':\n"
    "  Time:         %i min, %i sec\n"
    "  FrameCopies:  %i\n"
    "  ChannelCount: %i\n"
    "  WDenom:       %g\n"
    "  HDenom:       %g\n"
    "  XSpeed:       %i\n"
    "  YSpeed:       %i\n"
    "  Strategy:     %s [%i]\n"
    ,
    Path,
    ImagePath,
    TimeInMinutes,
    ((i32)TimeInSeconds) % 60,
    FrameCopies,
    ChannelCount,
    WDenom,
    HDenom,
    XSpeed,
    YSpeed,
    (SamplingStrategy >= 0 && SamplingStrategy < MAX_SAMPLING_STRATEGY) ? SamplingDesc[SamplingStrategy] : "Invalid sampling strategy!",
    SamplingStrategy
  );
#endif

  audio_source Source;
  if (InitAudioSource(&Source, SampleCount, ChannelCount) == NoError) {
    float* Iter = &Source.Buffer[0];
    float LastFrame = 0;
    float Frame = 0;
    switch (SamplingStrategy) {
      case S_DEFAULT: {
        for (i32 Y = 0; Y < Height; Y += YSpeed) {
          for (i32 X = 0; X < Width; X += XSpeed) {
            color_rgb* Color = (color_rgb*)&Image->PixelBuffer[(3 * ((X + (Y * Image->Width))) % (3 * (Image->Width * Image->Height)))];

            LastFrame = Frame;
            Frame = Amp * (float)(((Color->R + Color->G + Color->B) / 3) << 6) / SampleRate;
            Frame = Clamp(Frame, -1.0f, 1.0f);

            float InterpFactor = (fabs(LastFrame - Frame));
            for (i32 CopyIndex = 0; CopyIndex < FrameCopies; ++CopyIndex) {
              LastFrame = Lerp(LastFrame, Frame, InterpFactor);
              if (Source.ChannelCount == 2) {
                *(Iter++) += LastFrame;
                *(Iter++) += LastFrame;
                continue;
              }
              *(Iter++) += LastFrame;
              Tick++;
            }
          }
        }
        break;
      }
      case S_EXPERIMENTAL: {
        for (i32 Y = 0; Y < Height; Y += YSpeed) {
          for (i32 X = 0; X < Width; X += XSpeed) {
            color_rgb* Color = (color_rgb*)&Image->PixelBuffer[(3 * ((X + (Y * Image->Width))) % (3 * (Image->Width * Image->Height)))];

            LastFrame = Frame;
            float TickAdd = (float)(Color->R * Color->G * Color->B) / (255 * 255);
            Tick += TickAdd;
            Frame = Amp * sin((Tick * PI32 * 2 * 100) / SampleRate);
            Frame = Clamp(Frame, -1.0f, 1.0f);

            float InterpFactor = (fabs(LastFrame - Frame));
            for (i32 CopyIndex = 0; CopyIndex < FrameCopies; ++CopyIndex) {
              LastFrame = Lerp(LastFrame, Frame, InterpFactor);
              if (Source.ChannelCount == 2) {
                *(Iter++) += LastFrame;
                *(Iter++) += LastFrame;
                continue;
              }
              *(Iter++) += LastFrame;
              Tick++;
            }
          }
        }
        break;
      }
      default:
        Result = Error;
        goto Done;
    }
    Result = StoreAudioSource(Path, &Source);
Done:
    UnloadAudioSource(&Source);
    return Result;
  }
  return Error;
}

i32 GenAudio(i32 argc, char** argv) {
  args Args = (args) {
    .FrameCopies = 1,
    .ChannelCount = 1,
    .WDenom = 1.0f,
    .HDenom = 1.0f,
    .XSpeed = 1,
    .YSpeed = 1,
    .SamplingStrategy = S_DEFAULT,
  };
  char* ImagePath = NULL;

  parse_arg Arguments[] = {
    {0, NULL, "image/file path", ArgString, 0, &ImagePath},
    {'f', "frame-copies", "number of copies per frame", ArgInt, 1, &Args.FrameCopies},
    {'c', "channel-count", "number of channels", ArgInt, 1, &Args.ChannelCount},
    {'W', "width-denom", "width denominator (horizontal crop)", ArgFloat, 1, &Args.WDenom},
    {'H', "height-denom", "height denominator (vertical crop)", ArgFloat, 1, &Args.HDenom},
    {'x', "x-speed", "horizontal sampling speed", ArgInt, 1, &Args.XSpeed},
    {'y', "y-speed", "vertical sampling speed", ArgInt, 1, &Args.YSpeed},
    {'s', "strategy", "set sampling strategy [0-1]", ArgInt, 1, &Args.SamplingStrategy},
  };

  i32 Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result != NoError) {
    return Result;
  }
  if (ImagePath) {
    char* Ext = FetchExtension(ImagePath);
    u8 IsValidImage = 0;
    if (Ext) {
      if (strncmp(Ext, ".png", MAX_PATH_SIZE) == 0) {
        IsValidImage = 1;
      }
    }
    char OutPath[MAX_PATH_SIZE] = {0};
    i32 Length = 0;
    if (Ext) {
      Length = Ext - ImagePath;
    }
    else {
      Length = strnlen(ImagePath, MAX_PATH_SIZE);
    }
    snprintf(OutPath, MAX_PATH_SIZE, "%.*s.wav", Length, ImagePath);

    image Image;
    u8 LoadedImage = 0;
    if (IsValidImage) {
      if (LoadImage(ImagePath, &Image) != NoError) {
        fprintf(stderr, "Failed to read image file '%s' because it is corrupt or has wrong format\n", ImagePath);
        return Error;
      }
      LoadedImage = 1;
    }
    else {
      if (LoadFileAsImage(ImagePath, &Image) != NoError) {
        fprintf(stderr, "Failed to read binary file '%s'\n", ImagePath);
        return Error;
      }
      LoadedImage = 1;
    }
    if (LoadedImage) {
      if (GenerateFromImage(OutPath, ImagePath, &Image, 0.9f, G_SampleRate, Args.FrameCopies, Args.ChannelCount, Args.WDenom, Args.HDenom, Args.XSpeed, Args.YSpeed, Args.SamplingStrategy) != NoError) {
        fprintf(stderr, "Something went wrong when trying to generate audio from file '%s', of which were going to be generated to '%s'\n", ImagePath, OutPath);
      }
      UnloadImage(&Image);
    }
  }
  else {
    fprintf(stderr, "No image file was given\n");
    return NoError;
  }
  return NoError;
}
