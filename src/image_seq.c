// image_seq.c
// audio to image sequence generator

#include <signal.h>

static volatile u8 ShouldExit = 0;

typedef struct gen_image_args {
  char* Path;
  char* OutputPath;
  char* MaskPath;
  char* Module;
  i32 Width;
  i32 Height;
  float FrameRate;
  i32 StartIndex;
  i32 NumFrames;
  i32 Verbose;
} gen_image_args;

#define Vprintf(Verbose, Format, ...) ((Verbose) ? (printf(Format, ##__VA_ARGS__)) : (void)0)

typedef void (*image_seq_cb)(image_seq*);
typedef color_rgba (*image_seq_proc_cb)(image_seq*, i32, i32, float);

static void SigHandle(i32 Signal);
static i32 GenImageSequence(gen_image_args* Args);

void SigHandle(i32 Signal) {
  ShouldExit = 1;
}

i32 GenImageSequence(gen_image_args* Args) {
  i32 Result = NoError;
  float TotalRenderTime = 0.0f;
  float TotalTime = 0.0f;
  float AnimationTime = 0.0f;
  const float TimePerFrame = 1.0f / Args->FrameRate;
  char OutputPath[MAX_PATH_SIZE] = {};

  image_seq_cb InitCb = NULL;
  image_seq_cb DestroyCb = NULL;
  image_seq_proc_cb ProcessCb = NULL;
  void* ModuleHandle = NULL;

  image_seq Seq = {
    .Output = NULL,
    .Mask = NULL,
    .DbAverage = 0,
  };

  image Image;
  Result = InitImage(Args->Width, Args->Height, 4, &Image);
  if (Result != NoError) {
    goto Done;
  }
  Seq.Output = &Image;

  image Mask;
  if (Args->MaskPath) {
    if (LoadImage(Args->MaskPath, &Mask) == NoError) {
      Seq.Mask = &Mask;
    }
  }

  audio_source Audio;
  Result = LoadAudioSource(Args->Path, &Audio);
  if (Result != NoError) {
    goto Done;
  }

  i32 NumFrames = ((float)(Audio.SampleCount / Audio.ChannelCount) / G_SampleRate) * Args->FrameRate;
  if (Args->NumFrames > 0) {
    NumFrames = Clamp(Args->NumFrames, 0, NumFrames);
  }
  i32 MaxFrames = Args->StartIndex + NumFrames;

  if (Args->Module) {
    ModuleHandle = ModuleOpen(Args->Module);
    if (ModuleHandle) {
      InitCb = ModuleSymbol(ModuleHandle, "Init");
      DestroyCb = ModuleSymbol(ModuleHandle, "Destroy");
      ProcessCb = ModuleSymbol(ModuleHandle, "Process");
    }
    else {
      Result = Error;
      goto Done;
    }
  }

  if (InitCb) {
    InitCb(&Seq);
  }

  struct timeval TimeNow = {0};
  gettimeofday(&TimeNow, NULL);
  struct timeval TimeLast = {0};

  for (
      i32 FrameIndex = Args->StartIndex;
      FrameIndex < MaxFrames && !ShouldExit;
      ++FrameIndex, AnimationTime = FrameIndex * TimePerFrame) {

    TimeLast = TimeNow;
    gettimeofday(&TimeNow, NULL);
    float DeltaTime = ((((TimeNow.tv_sec - TimeLast.tv_sec) * 1000000.0f) + TimeNow.tv_usec) - (TimeLast.tv_usec)) / 1000000.0f;
    TotalTime += DeltaTime;

    i32 FramesLeft = MaxFrames - FrameIndex;
    float TimeLeft = DeltaTime * FramesLeft;
    Vprintf(Args->Verbose, "frame = %4i/%i, fps = %3i, last = %.4g ms, est. time left = %3.3g s\n", FrameIndex, MaxFrames - 1, (i32)(1.0f / DeltaTime), DeltaTime, TimeLeft);

    const i32 WindowSize = 1024;
    f32 Db = 0.0f;
    for (i32 WindowIndex = 0; WindowIndex < WindowSize; ++WindowIndex) {
      i32 Index = FrameIndex + WindowIndex;
      f32 Frame = Audio.Buffer[Index % Audio.SampleCount];
      Db += Frame * Frame;
    }
    Seq.DbAverage = SquareRoot(Db / (f32)WindowSize);

    for (i32 Y = 0; Y < Image.Height; ++Y) {
      for (i32 X = 0; X < Image.Width; ++X) {
        if (ProcessCb) {
          color_rgba* Color = (color_rgba*)FetchPixel(&Image, X, Y);
          *Color = ProcessCb(&Seq, X, Y, AnimationTime);
        }
        else {
          Result = Error;
          goto Done;
        }
      }
    }
    snprintf(OutputPath, MAX_PATH_SIZE, "%s%04i.png", Args->OutputPath, FrameIndex);
    StoreImage(OutputPath, &Image);
  }
Done:
  UnloadImage(&Image);
  UnloadImage(&Mask);
  UnloadAudioSource(&Audio);
  if (DestroyCb) DestroyCb(&Seq);
  ModuleClose(ModuleHandle);
  return Result;
}


i32 ImageSeq(i32 argc, char** argv) {
  i32 Result = NoError;
  signal(SIGINT, SigHandle);

  gen_image_args Args = {
    .Path = NULL,
    .OutputPath = "frame_",
    .MaskPath = NULL,
    .Module = NULL,
    .Width = 1024,
    .Height = 1024,
    .FrameRate = 24,
    .StartIndex = 0,
    .NumFrames = 0,
    .Verbose = 0,
  };

  parse_arg Arguments[] = {
    {0, NULL, "path to audio file", ArgString, 0, &Args.Path},
    {'o', "output-path", "path to output file", ArgString, 1, &Args.OutputPath},
    {'m', "mask", "path to mask image", ArgString, 1, &Args.MaskPath},
    {'s', "strategy", "path to image generating strategy module", ArgString, 1, &Args.Module},
    {'W', "width", "width of output image", ArgInt, 1, &Args.Width},
    {'H', "height", "height of output image", ArgInt, 1, &Args.Height},
    {'r', "frame-rate", "number of frames per second", ArgFloat, 1, &Args.FrameRate},
    {'i', "start-index", "which frame to start the animation at", ArgInt, 1, &Args.StartIndex},
    {'n', "num-frames", "number of frames to generate", ArgInt, 1, &Args.NumFrames},
    {'v', "verbose", "verbose output", ArgInt, 0, &Args.Verbose},
  };

  Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result != NoError) {
    return Result;
  }
  if (!Args.Path) {
    fprintf(stderr, "No audio file was given\n");
    return Result;
  }

  return GenImageSequence(&Args);
}
