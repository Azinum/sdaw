// gen_image.c

enum gen_image_strategy {
  IMG_GEN_STRAT_DEFAULT = 0,
  IMG_GEN_STRAT_EXPERIMENTAL,
  IMG_GEN_STRAT_LOUDNESS,
  MAX_STRATEGY,
};

typedef struct gen_image_args {
  char* Path;
  char* OutputPath;
  i32 Width;
  i32 Height;
  i32 StartIndex;
  i32 GenSequence;
  i32 SeqFrameRate;
  i32 Strategy;
  i32 NumFrames;
} gen_image_args;

#define FetchPixel(X, Y) (color_rgb*)&Image.PixelBuffer[(3 * ((X + (Y * Image.Width))) % (3 * (Image.Width * Image.Height)))]

static i32 GenerateImageSequence(const char* Path, audio_source* Audio, gen_image_args* Args);
static i32 GenerateFromAudio(const char* Path, audio_source* Audio, gen_image_args* Args);

i32 GenerateImageSequence(const char* Path, audio_source* Audio, gen_image_args* Args) {
  i32 Result = NoError;
  char OutputPath[MAX_PATH_SIZE] = {0};
  image Image;
  Result = InitImage(Args->Width, Args->Height, &Image);
  if (Result == NoError) {
    i32 NumFrames = ((float)(Audio->SampleCount / Audio->ChannelCount) / SAMPLE_RATE) * Args->SeqFrameRate;
    if (Args->NumFrames > 0) {
      NumFrames = Clamp(Args->NumFrames, 0, NumFrames);
    }
    i32 WindowSize = 2048;
    i32 FrameSize = (float)(SAMPLE_RATE * Audio->ChannelCount) / Args->SeqFrameRate;
    float ImageSize = DistanceV2(V2(0, 0), V2(Image.Width, Image.Height));

    for (i32 FrameIndex = Args->StartIndex; FrameIndex < Args->StartIndex + NumFrames; ++FrameIndex) {
      float SampleIndex = FrameIndex * FrameSize;
      float Db = 0.0f;
      for (i32 WindowIndex = 0; WindowIndex < WindowSize; ++WindowIndex) {
        i32 Index = SampleIndex + WindowIndex;
        float Frame = Audio->Buffer[Index % Audio->SampleCount];
        Db += -(20 * Log10(Abs(Frame)));
      }
      float DbAverage = (float)Db / WindowSize;
      float Amp = Min(20.0f / (1 + DbAverage), 1.0f) * 10;

      switch (Args->Strategy) {
        case IMG_GEN_STRAT_DEFAULT: {
          for (i32 Y = 0; Y < Image.Height; ++Y) {
            for (i32 X = 0; X < Image.Width; ++X) {
              i32 Start = SampleIndex;
              i32 End = Start + FrameSize;
              float Factor = (float)((X - (Image.Width * 0.5f)) * (Y - (Image.Height * 0.5f))) / (Image.Width * Image.Height);
              i32 ResultAt = Abs(Lerp(Start, Start * 2, Factor));
              float* Frames = &Audio->Buffer[(ResultAt + 3) % Audio->SampleCount];
              color_rgb* Color = (color_rgb*)&Image.PixelBuffer[(3 * ((X + (Y * Image.Width))) % (3 * (Image.Width * Image.Height)))];
              if (Abs(Frames[0]) > 0.19f) {
                Color->R += (u8)((Amp * Abs(Frames[0])) * INT8_MAX);
                Color->G += (u8)((Amp * Abs(Frames[1])) * INT8_MAX);
                Color->B += (u8)((Amp * Abs(Frames[2])) * INT8_MAX);
              }
              else {
                Color->R = (u8)((0.25f * Abs(Frames[0])) * INT8_MAX);
                Color->G = (u8)((0.25f * Abs(Frames[1])) * INT8_MAX);
                Color->B = (u8)((0.25f * Abs(Frames[2])) * INT8_MAX);
              }
            }
          }
          snprintf(OutputPath, MAX_PATH_SIZE, "%s%04i.png", Path, FrameIndex);
          StoreImage(OutputPath, &Image);
          break;
        }
        case IMG_GEN_STRAT_EXPERIMENTAL: {
          for (i32 Y = 0; Y < Image.Height; ++Y) {
            for (i32 X = 0; X < Image.Width; ++X) {
              i32 Start = SampleIndex;
              i32 End = Start + FrameSize;
              v2 Target = V2(Image.Width / 2, Image.Height / 2);
              v2 P = V2(X, Y);
              float Dist = DistanceV2(P, Target);
              float Factor = 1.0f - (Dist / ImageSize);
              i32 ResultAt = Abs(Lerp(End, Start, Factor));
#if 0

              float* Frame0 = &Audio->Buffer[(ResultAt * 1) % Audio->SampleCount];
              float* Frame1 = &Audio->Buffer[(ResultAt * 2) % Audio->SampleCount];
              float* Frame2 = &Audio->Buffer[(ResultAt * 3) % Audio->SampleCount];
#else
              i32 ResultAt0 = ResultAt;
              i32 ResultAt1 = Abs(Lerp(End, Start, 1.0f - ((Dist * 0.75) / ImageSize)));
              i32 ResultAt2 = Abs(Lerp(End, Start, 1.0f - ((Dist * 0.50) / ImageSize)));

              float* Frame0 = &Audio->Buffer[ResultAt0 % Audio->SampleCount];
              float* Frame1 = &Audio->Buffer[ResultAt1 % Audio->SampleCount];
              float* Frame2 = &Audio->Buffer[ResultAt2 % Audio->SampleCount];
#endif
              color_rgb* Color = (color_rgb*)&Image.PixelBuffer[(3 * ((X + (Y * Image.Width))) % (3 * (Image.Width * Image.Height)))];
              Color->R = (Amp * Abs(Frame0[0])) * INT8_MAX;
              Color->G = (Amp * Abs(Frame1[0])) * INT8_MAX;
              Color->B = (Amp * Abs(Frame2[0])) * INT8_MAX;
            }
          }
          snprintf(OutputPath, MAX_PATH_SIZE, "%s%04i.png", Path, FrameIndex);
          StoreImage(OutputPath, &Image);
          break;
        }
        case IMG_GEN_STRAT_LOUDNESS: {
          // First pass
          for (i32 Y = 0; Y < Image.Height; ++Y) {
            for (i32 X = 0; X < Image.Height; ++X) {
              color_rgb* Color = (color_rgb*)&Image.PixelBuffer[(3 * ((X + (Y * Image.Width))) % (3 * (Image.Width * Image.Height)))];
              i32 Start = SampleIndex;
              i32 End = Start + FrameSize;
              v2 Target = V2(Image.Width / 2, Image.Height / 2);
              v2 P = V2(X, Y);

              float Dist = DistanceV2(P, Target) * Amp;
              float Factor = 1.0f - (Dist / ImageSize / 2) * 5.0f;
              i32 ResultAt = Abs(Lerp(End, Start, Factor));
              float Frame = Audio->Buffer[ResultAt % Audio->SampleCount];

              Color->R = Abs(Factor * Frame) * INT8_MAX;
              Color->G = Abs(Factor * Frame) * 0xa;
              Color->B = Abs(Factor * Frame) * 0xa;
            }
          }

#if 1
          // Second pass
          for (i32 Y = 0; Y < Image.Height; ++Y) {
            for (i32 X = 0; X < Image.Height; ++X) {
              #define NUM_PIXELS 4
              color_rgb* Color = FetchPixel(X, Y);
              color_rgb* Pixels[NUM_PIXELS] = {
                FetchPixel(X + 0, Y - 1), // Top
                FetchPixel(X + 1, Y + 0), // Right
                FetchPixel(X + 0, Y - 1), // Bottom
                FetchPixel(X - 1, Y + 0), // Left
              };
              color_rgb16 Sum = ColorRGB16(0, 0, 0);
              for (i32 PixelIndex = 0; PixelIndex < NUM_PIXELS; ++PixelIndex) {
                color_rgb* Pixel = Pixels[PixelIndex];
                Sum.R += Pixel->R;
                Sum.G += Pixel->G;
                Sum.B += Pixel->B;
              }
              Sum.R /= NUM_PIXELS;
              Sum.G /= NUM_PIXELS;
              Sum.B /= NUM_PIXELS;
              Color->R += 0.5f * Sum.R;
              Color->G += 0.5f * Sum.G;
              Color->B += 0.5f * Sum.B;
            }
          }
#else
          #define BLOCK_SIZE 4
          for (i32 Y = 0; Y < Image.Height; Y += BLOCK_SIZE) {
            for (i32 X = 0; X < Image.Height; X += BLOCK_SIZE) {
              color_rgb* Color = FetchPixel(X, Y);
              color_rgb Copy = *Color;
              for (i32 BlockY = Y; BlockY < Y + BLOCK_SIZE; ++BlockY) {
                for (i32 BlockX = X; BlockX < X + BLOCK_SIZE; ++BlockX) {
                  color_rgb* Pixel = FetchPixel(BlockX, BlockY);
                  *Pixel = Copy;
                }
              }
            }
          }
#endif
          snprintf(OutputPath, MAX_PATH_SIZE, "%s%04i.png", Path, FrameIndex);
          StoreImage(OutputPath, &Image);
          break;
        }
        default: {
          fprintf(stderr, "Invalid image generating strategy (got %i, should be 0-%i)\n", Args->Strategy, MAX_STRATEGY - 1);
          Result = Error;
          goto Done;
        }
      }
    }
  }
  else {
    return Result;
  }
Done:
  UnloadImage(&Image);
  return Result;
}

i32 GenerateFromAudio(const char* Path, audio_source* Audio, gen_image_args* Args) {
  i32 Result = NoError;
  image Image;
  if ((Result = InitImage(Args->Width, Args->Height, &Image)) == NoError) {
    i32 SampleIndex = Args->StartIndex;
    for (i32 Y = 0; Y < Image.Height; ++Y) {
      for (i32 X = 0; X < Image.Width; ++X) {
        color_rgb* Color = (color_rgb*)&Image.PixelBuffer[(3 * ((X + (Y * Image.Width))) % (3 * (Image.Width * Image.Height)))];
        float* Samples = &Audio->Buffer[(SampleIndex + 3) % Audio->SampleCount]; // Grab 3 samples, one for each color component
        SampleIndex += Audio->ChannelCount * 3;
        Color->R = (u8)(Samples[0] * INT8_MAX);
        Color->G = (u8)(Samples[1] * INT8_MAX);
        Color->B = (u8)(Samples[2] * INT8_MAX);
      }
    }
  }
  else {
    return Result;
  }
  StoreImage(Path, &Image);
  UnloadImage(&Image);
  return Result;
}

i32 GenImage(i32 argc, char** argv) {
  i32 Result = NoError;
  gen_image_args Args = {
    .Path = NULL,
    .OutputPath = "out.png",
    .Width = 256,
    .Height = 256,
    .StartIndex = 0,
    .GenSequence = 0,
    .SeqFrameRate = 24,
    .Strategy = IMG_GEN_STRAT_DEFAULT,
    .NumFrames = 0,
  };

  parse_arg Arguments[] = {
    {0, NULL, "path to audio file", ArgString, 0, &Args.Path},
    {'W', "width", "image width", ArgInt, 1, &Args.Width},
    {'H', "height", "image height", ArgInt, 1, &Args.Height},
    {'i', "start-index", "audio frame index to start at", ArgInt, 1, &Args.StartIndex},
    {'o', "output-path", "path to output file", ArgString, 1, &Args.OutputPath},
    {'S', "sequence", "generate image sequence", ArgInt, 0, &Args.GenSequence},
    {'r', "seq-frame-rate", "frame rate of the sequence", ArgInt, 1, &Args.SeqFrameRate},
    {'s', "strategy", "image sequence generator strategy", ArgInt, 1, &Args.Strategy},
    {'n', "num-frames", "maximum amount of frames in the image sequence generator", ArgInt, 1, &Args.NumFrames},
  };

  Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result != NoError) {
    return Result;
  }
  if (!Args.Path) {
    fprintf(stderr, "No audio file was given\n");
    return Result;
  }
  audio_source Audio;
  Result = LoadAudioSource(Args.Path, &Audio);
  if (Result != NoError) {
    fprintf(stderr, "'%s' is not an audio file\n", Args.Path);
    return Result;
  }
  if (Args.GenSequence) {
    Result = GenerateImageSequence(Args.OutputPath, &Audio, &Args);
  }
  else {
    Result = GenerateFromAudio(Args.OutputPath, &Audio, &Args);
  }
  UnloadAudioSource(&Audio);
  return Result;
}
