// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "image.c"
#include "audio.c"
#include "riff.c"

typedef struct args {
  i32 FrameCopies;
  i32 ChannelCount;
  float WDenom;
  float HDenom;
  i32 XSpeed;
  i32 YSpeed;
} args;

static i32 GenerateSineWave(audio_source* Source, float Amp, float Freq);
static i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, float Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, float WDenom, float HDenom, i32 XSpeed, i32 YSpeed);
static void PrintHelp(FILE* File);

i32 GenerateSineWave(audio_source* Source, float Amp, float Freq) {
  float* Iter = Source->Buffer;
  for (u32 SampleIndex = 0; SampleIndex < Source->SampleCount; ++SampleIndex) {
    float Frame = Amp * sin((SampleIndex * Freq * 2 * PI32) / SAMPLE_RATE);
    Amp = Lerp(Amp, 0.0f, 0.0001f);
    if (Source->ChannelCount == 2) {
      *(Iter++) += Frame;
      *(Iter++) += Frame;
      continue;
    }
    *(Iter++) += Frame;
  }
  return NoError;
}

i32 GenerateFromImage(const char* Path, const char* ImagePath, image* Image, float Amp, i32 SampleRate, i32 FrameCopies, i32 ChannelCount, float WDenom, float HDenom, i32 XSpeed, i32 YSpeed) {
  i32 Result = NoError;

  i32 Width = Image->Width / WDenom;
  i32 Height = Image->Height / HDenom;

#if 1
  printf(
    "Generating audio file '%s' from image file '%s', with the following options:\n"
    "  FrameCopies:   %i\n"
    "  ChannelCount:  %i\n"
    "  WDenom:        %g\n"
    "  HDenom:        %g\n"
    "  XSpeed:        %i\n"
    "  YSpeed:        %i\n"
    ,
    Path,
    ImagePath,
    FrameCopies,
    ChannelCount,
    WDenom,
    HDenom,
    XSpeed,
    YSpeed
  );
#endif

  // TODO(lucas): This size is arbitrary, calculate the exact number of padding needed.
  i32 Padding = 2 * 4096; // NOTE(lucas): Use padding to not overflow the sample buffer.
  i32 SampleCount = ((Width / (float)XSpeed) * (Height / (float)YSpeed) * ChannelCount * FrameCopies) + Padding;
  i32 Tick = 1;

  audio_source Source;
  if (InitAudioSource(&Source, SampleCount, ChannelCount) == NoError) {
    float* Iter = &Source.Buffer[0];
    float LastFrame = 0;
    float Frame = 0;
    for (i32 Y = 0; Y < Height; Y += YSpeed) {
      for (i32 X = 0; X < Width; X += XSpeed) {
        color_rgb* Color = (color_rgb*)&Image->PixelBuffer[(1 * ((X + (Y * Image->Width))) % ((3 * Image->Width * Image->Height)))];

        LastFrame = Frame;
        Frame = Amp * (float)(((Color->R + Color->G + Color->B) / 3) << 6) / (SAMPLE_RATE);
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
    Result = StoreAudioSource(Path, &Source);
    UnloadAudioSource(&Source);
    return Result;
  }
  return Error;
}

void PrintHelp(FILE* File) {
  fprintf(File,
    "Usage: ./sdaw <image path> [options]\n"
    "Avaliable options are:\n"
    "  -c value   number of frame copies\n"
    "  -w value   width denominator\n"
    "  -h value   height denominator\n"
    "  -x value   horizontal sampling speed\n"
    "  -y value   vertical sampling speed\n"
    "  -h         show this menu\n"
  );
}

i32 SdawStart(i32 argc, char** argv) {
  srand(time(NULL));
  args Args = (args) {
    .FrameCopies = 1,
    .ChannelCount = 1,
    .WDenom = 1.0f,
    .HDenom = 1.0f,
    .XSpeed = 1,
    .YSpeed = 1,
  };
  char* ImagePath = NULL;

  for (i32 Index = 1; Index < argc; ++Index) {
    char* Arg = argv[Index];
    if (Arg[0] == '-') {
      if (Arg[1] == '-') {
        continue;
      }
      if (Index + 1 < argc) {
        switch (Arg[1]) {
          case 'c': {
            sscanf(argv[++Index], "%i", &Args.FrameCopies);
            break;
          }
          case 'w': {
            sscanf(argv[++Index], "%f", &Args.WDenom);
            break;
          }
          case 'h': {
            sscanf(argv[++Index], "%f", &Args.HDenom);
            break;
          }
          case 'x': {
            sscanf(argv[++Index], "%i", &Args.XSpeed);
            break;
          }
          case 'y': {
            sscanf(argv[++Index], "%i", &Args.YSpeed);
            break;
          }
          default:
            break;
        }
      }
      else {
        switch (Arg[1]) {
          case 'h': {
            PrintHelp(stdout);
            break;
          }
          default:
            break;
        }
      }
    }
    else {
      ImagePath = argv[Index];
    }
  }
  if (argc <= 1) {
    PrintHelp(stdout);
    return NoError;
  }
  if (ImagePath) {
    char* Ext = strrchr(ImagePath, '.');
    if (Ext) {
      if (strcmp(Ext, ".png") != 0) {
        fprintf(stderr, "Invalid image file extension (is %s, should be png)\n", Ext + 1);
        return Error;
      }
    }
    else {
      fprintf(stderr, "No extension specified in image file, png is assumed\n");
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
    if (LoadImage(ImagePath, &Image) == NoError) {
      if (GenerateFromImage(OutPath, ImagePath, &Image, 0.9f, SAMPLE_RATE, Args.FrameCopies, 1, Args.WDenom, Args.HDenom, Args.XSpeed, Args.YSpeed) != NoError) {
        fprintf(stderr, "Something went wrong when trying to generate audio for image '%s', which were going to be generated to '%s'\n", ImagePath, OutPath);
      }
      UnloadImage(&Image);
    }
    else {
      fprintf(stderr, "Failed to read image file '%s' because it is corrupt or has wrong format\n", ImagePath);
      return Error;
    }
  }
  else {
    fprintf(stderr, "No image file was given\n");
    return Error;
  }
  return NoError;
}
