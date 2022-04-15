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
  char* ImagePath;
  i32 FrameCopies;
  i32 ChannelCount;
  f32 WDenom;
  f32 HDenom;
  i32 XSpeed;
  i32 YSpeed;
  i32 SamplingStrategy;
  i32 Verbose;
} args;

static i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, f32 Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, f32 WDenom, f32 HDenom, i32 XSpeed, i32 YSpeed, i32 SamplingStrategy);

i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, f32 Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, f32 WDenom, f32 HDenom, i32 XSpeed, i32 YSpeed, i32 SamplingStrategy) {
  i32 Result = NoError;

  i32 Width = Image->Width / WDenom;
  i32 Height = Image->Height / HDenom;

  // TODO(lucas): This size is arbitrary, calculate the exact number of padding needed.
  i32 Padding = 2 * 4096; // NOTE(lucas): Use padding to not overflow the sample buffer.
  i32 SampleCount = ((Width / (f32)XSpeed) * (Height / (f32)YSpeed) * ChannelCount * FrameCopies) + Padding;
  f32 Tick = 0.0f;

#if 0
  f32 TimeInSeconds = (f32)SampleCount / SampleRate;
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
    f32* Iter = &Source.Buffer[0];
    f32 LastFrame = 0;
    f32 Frame = 0;
    switch (SamplingStrategy) {
      case S_DEFAULT: {
        for (i32 Y = 0; Y < Height; Y += YSpeed) {
          for (i32 X = 0; X < Width; X += XSpeed) {
            color_rgb* Color = (color_rgb*)&Image->PixelBuffer[(3 * ((X + (Y * Image->Width))) % (3 * (Image->Width * Image->Height)))];

            LastFrame = Frame;
            Frame = Amp * (f32)(((Color->R + Color->G + Color->B) / 3) << 6) / SampleRate;
            Frame = Clamp(Frame, -1.0f, 1.0f);

            f32 InterpFactor = (fabs(LastFrame - Frame));
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
            f32 TickAdd = (f32)(Color->R * Color->G * Color->B) / (255 * 255);
            Tick += TickAdd;
            Frame = Amp * sin((Tick * PI32 * 2 * 55.0f) / SampleRate);
            Frame = Clamp(Frame, -1.0f, 1.0f);

            f32 InterpFactor = (fabs(LastFrame - Frame));
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
  (void)SamplingDesc;
  args Args = (args) {
    .ImagePath = NULL,
    .FrameCopies = 1,
    .ChannelCount = 1,
    .WDenom = 1.0f,
    .HDenom = 1.0f,
    .XSpeed = 1,
    .YSpeed = 1,
    .SamplingStrategy = S_DEFAULT,
    .Verbose = 0,
  };

  parse_arg Arguments[] = {
    {0, NULL, "image/file path", ArgString, 0, &Args.ImagePath},
    {'f', "frame-copies", "number of copies per frame", ArgInt, 1, &Args.FrameCopies},
    {'c', "channel-count", "number of channels", ArgInt, 1, &Args.ChannelCount},
    {'W', "width-denom", "width denominator (horizontal crop)", ArgFloat, 1, &Args.WDenom},
    {'H', "height-denom", "height denominator (vertical crop)", ArgFloat, 1, &Args.HDenom},
    {'x', "x-speed", "horizontal sampling speed", ArgInt, 1, &Args.XSpeed},
    {'y', "y-speed", "vertical sampling speed", ArgInt, 1, &Args.YSpeed},
    {'s', "strategy", "set sampling strategy [0-1]", ArgInt, 1, &Args.SamplingStrategy},
    {'v', "verbose", "verbose output", ArgInt, 0, &Args.Verbose},
  };

  i32 Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result != NoError) {
    return Result;
  }
  if (Args.ImagePath) {
    char* Ext = FetchExtension(Args.ImagePath);
    u8 IsValidImage = 0;
    if (Ext) {
      if (strncmp(Ext, ".png", MAX_PATH_SIZE) == 0) {
        IsValidImage = 1;
      }
    }
    char OutPath[MAX_PATH_SIZE] = {0};
    i32 Length = 0;
    if (Ext) {
      Length = Ext - Args.ImagePath;
    }
    else {
      Length = strnlen(Args.ImagePath, MAX_PATH_SIZE);
    }
    snprintf(OutPath, MAX_PATH_SIZE, "%.*s.wav", Length, Args.ImagePath);

    image Image;
    u8 LoadedImage = 0;
    if (IsValidImage) {
      if (LoadImage(Args.ImagePath, &Image) != NoError) {
        fprintf(stderr, "Failed to read image file '%s' because it is corrupt or has wrong format\n", Args.ImagePath);
        return Error;
      }
      LoadedImage = 1;
    }
    else {
      if (LoadFileAsImage(Args.ImagePath, &Image) != NoError) {
        fprintf(stderr, "Failed to read binary file '%s'\n", Args.ImagePath);
        return Error;
      }
      LoadedImage = 1;
    }
    if (LoadedImage) {
      if (GenerateFromImage(OutPath, Args.ImagePath, &Image, 0.9f, G_SampleRate, Args.FrameCopies, Args.ChannelCount, Args.WDenom, Args.HDenom, Args.XSpeed, Args.YSpeed, Args.SamplingStrategy) != NoError) {
        fprintf(stderr, "Something went wrong when trying to generate audio from file '%s', of which were going to be generated to '%s'\n", Args.ImagePath, OutPath);
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
