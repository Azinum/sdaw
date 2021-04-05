// gen_image.c

typedef struct gen_image_args {
  char* Path;
  char* OutputPath;
  i32 Width;
  i32 Height;
  i32 StartIndex;
} gen_image_args;

static i32 GenerateFromAudio(const char* Path, audio_source* Audio, gen_image_args* Args);

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
  };

  parse_arg Arguments[] = {
    {0, NULL, "path to audio file", ArgString, 0, &Args.Path},
    {'W', "width", "image width", ArgInt, 1, &Args.Width},
    {'H', "height", "image height", ArgInt, 1, &Args.Height},
    {'s', "start-index", "audio frame index to start at", ArgInt, 1, &Args.StartIndex},
    {'o', "output-path", "path to output file", ArgString, 1, &Args.OutputPath},
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
  Result = GenerateFromAudio(Args.OutputPath, &Audio, &Args);
  UnloadAudioSource(&Audio);
  return Result;
}
