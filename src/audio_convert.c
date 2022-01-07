// audio_convert.c

typedef struct audio_convert_args {
  char* Input;
  char* Output;
} audio_convert_args;

static i32 AudioConvertRun(audio_convert_args* Args);

i32 AudioConvertRun(audio_convert_args* Args) {
  i32 Result = NoError;

  audio_source Audio;
  if ((Result = LoadAudioSource(Args->Input, &Audio)) == NoError) {
    Result = StoreAudioSource(Args->Output, &Audio);
    UnloadAudioSource(&Audio);
  }

  return Result;
}

i32 AudioConvert(i32 argc, char** argv) {
  i32 Result = NoError;
  audio_convert_args Args = (audio_convert_args) {
    .Input = NULL,
    .Output = NULL,
  };

  parse_arg Arguments[] = {
    {'i', "input", "path to input file", ArgString, 1, &Args.Input},
    {'o', "output", "path to output file", ArgString, 1, &Args.Output},
  };

  Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result == Error) {
    return Result;
  }
  else if (Result == HelpStatus) {
    return NoError;
  }
  else {
    if (!Args.Input) {
      fprintf(stderr, "No input audio file was given\n");
      return Result;
    }
    else if (!Args.Output) {
      fprintf(stderr, "No output audio file was given\n");
      return Result;
    }
  }
  return AudioConvertRun(&Args);
}
