// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "image.c"
#include "audio.c"
#include "riff.c"

static i32 GenerateSineWave(audio_source* Source, float Amp, float Freq);
static i32 GenerateFromImage(const char* Path, image* Image, float Amp);

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

i32 GenerateFromImage(const char* Path, image* Image, float Amp) {
  fprintf(stdout,
    "Generating audio file (to '%s') for image with the following specifications:\n"
    "  Width: %i\n"
    "  Height: %i\n"
    "  BytesPerPixel: %i\n"
    ,
    Path,
    Image->Width,
    Image->Height,
    Image->BytesPerPixel
  );
  i32 Result = NoError;
  i32 SampleRate = SAMPLE_RATE;
  i32 FrameCopies = 1 + rand() % 16;
  i32 ChannelCount = 1;
  float WDenom = 1 + rand() % 8;
  float HDenom = 1 + rand() % 8;
  i32 XSpeed = 1 + rand() % 4;
  i32 YSpeed = 1 + rand() % 4;
  i32 Width = Image->Width / WDenom;
  i32 Height = Image->Height / HDenom;

  printf(
    "FrameCopies:   %i\n"
    "ChannelCount:  %i\n"
    "WDenom:        %g\n"
    "HDenom:        %g\n"
    "XSpeed:        %i\n"
    "YSpeed:        %i\n"
    ,
    FrameCopies,
    ChannelCount,
    WDenom,
    HDenom,
    XSpeed,
    YSpeed
  );

  // TODO(lucas): This size is arbitrary, calculate the exact number of padding needed.
  i32 Padding = 4096; // NOTE(lucas): Use padding to not overflow the sample buffer.
  i32 SampleCount = ((Width / XSpeed) * (Height / YSpeed) * ChannelCount * FrameCopies) + Padding;

  audio_source Source;
  if (InitAudioSource(&Source, SampleCount, ChannelCount) == NoError) {
    float* Iter = &Source.Buffer[0];
    float LastFrame = 0;
    float Frame = 0;
    for (i32 Y = 0; Y < Height; Y += YSpeed) {
      for (i32 X = 0; X < Width; X += XSpeed) {
        color_rgb* Color = (color_rgb*)&Image->PixelBuffer[(1 * ((X + (Y * Image->Width))) % ((3 * Image->Width * Image->Height)))];

        LastFrame = Frame;
        Frame = Amp * (float)(Color->R + Color->G + Color->B) / (255 * 3);
        Frame = Clamp(Frame, -1.0f, 1.0f);

        // printf("Interpolate (%g -> %g):\n", LastFrame, Frame);
        float InterpFactor = (fabs(LastFrame - Frame));
        for (i32 CopyIndex = 0; CopyIndex < FrameCopies; ++CopyIndex) {
          LastFrame = Lerp(LastFrame, Frame, InterpFactor);
          if (Source.ChannelCount == 2) {
            *(Iter++) += LastFrame;
            *(Iter++) += LastFrame;
            continue;
          }
          *(Iter++) += LastFrame;
          // printf("  Interpolated frame: %g\n", LastFrame);
        }
        // printf("\n");
      }
    }
    Result = StoreAudioSource(Path, &Source);
    UnloadAudioSource(&Source);
    return Result;
  }
  return Error;
}

i32 SdawStart(i32 argc, char** argv) {
  srand(time(NULL));
  if (argc > 1) {
    char* ImagePath = argv[1];
    char* OutputPath = NULL;
    if (argc > 2) {
      OutputPath = argv[2];
    }
    else {
      char* Ext = strrchr(ImagePath, '.');
      if (Ext) {
        if (strcmp(Ext, ".png") != 0) {
          fprintf(stderr, "Invalid image file extension (is %s, should be .png)\n", Ext);
          return Error;
        }
      }
      else {
        fprintf(stderr, "No extension specified in image file, .png is assumed\n");
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
      OutputPath = OutPath;
    }

    image Image;
    if (LoadImage(ImagePath, &Image) == NoError) {
      if (GenerateFromImage(OutputPath, &Image, 0.9f) == NoError) {

      }
      UnloadImage(&Image);
    }
    else {
      fprintf(stderr, "Failed to open image file '%s' because it is corrupt or has wrong format\n", ImagePath);
      return Error;
    }
  }
  else {
    printf("Usage: ./sdaw <source png image> <output wave file>\n");
#if 0
    audio_source Source;
    if (InitAudioSource(&Source, 44100, 2) == NoError) {
      GenerateSineWave(&Source, 0.5f, 220.0f);
      StoreAudioSource("sine_out.wav", &Source);
      UnloadAudioSource(&Source);
    }
#endif
  }
  return NoError;
}
