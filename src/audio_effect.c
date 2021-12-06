// audio_effect.c

enum effect_type {
  EFFECT_NONE,
  EFFECT_DISTORTION,
  EFFECT_WEIRD_01,
  EFFECT_WEIRD_02,

  MAX_EFFECT_TYPE,
};

const char* EffectTypeDesc[MAX_EFFECT_TYPE] = {
  "none",
  "distortion",
  "weird",
  "weird 2",
};

effect_func EffectFuncs[MAX_EFFECT_TYPE] = {
  StubEffect,
  Distortion,
  WeirdEffect,
  WeirdEffect2,
};

typedef struct audio_effect_args {
  char* Input;
  char* Output;
  i32 EffectType;
  i32 SecondaryEffectType;  // NOTE(lucas): Temporary.
  f32 Mix;
  f32 Value;
} audio_effect_args;

static i32 AudioEffectPrintHelp(FILE* File);
static i32 AudioEffectRun(audio_effect_args* Args);

i32 AudioEffectPrintHelp(FILE* File) {
  i32 Result = NoError;

  fprintf(File, "EFFECTS:\n");
  for (u32 EffectType = 0; EffectType < MAX_EFFECT_TYPE; ++EffectType) {
    fprintf(File, "   %i: %s\n", EffectType, EffectTypeDesc[EffectType]);
  }

  return Result;
}

i32 AudioEffectRun(audio_effect_args* Args) {
  i32 Result = NoError;

  audio_source Audio;
  if ((Result = LoadAudioSource(Args->Input, &Audio)) == NoError) {
    if (Args->EffectType >= 0 && Args->EffectType < MAX_EFFECT_TYPE) {
      EffectFuncs[Args->EffectType](Audio.Buffer, 1, Audio.SampleCount, Args->Mix, Args->Value);
    }
    if (Args->SecondaryEffectType >= 0 && Args->SecondaryEffectType < MAX_EFFECT_TYPE) {
      EffectFuncs[Args->SecondaryEffectType](Audio.Buffer, 1, Audio.SampleCount, Args->Mix, Args->Value);
    }
    StoreAudioSource(Args->Output, &Audio);
    UnloadAudioSource(&Audio);
  }

  return Result;
}

i32 AudioEffect(i32 argc, char** argv) {
  i32 Result = NoError;

  audio_effect_args Args = {
    .Input = NULL,
    .Output = NULL,
    .EffectType = EFFECT_NONE,
    .SecondaryEffectType = EFFECT_NONE,
    .Mix = 0,
    .Value = 0,
  };

  parse_arg Arguments[] = {
    {0, NULL, "path to input audio file", ArgString, 0, &Args.Input},
    {'o', "output-path", "path to output audio file", ArgString, 1, &Args.Output},
    {'e', "effect", "which effect to use", ArgInt, 1, &Args.EffectType},
    {'E', "secondary effect", "which secondary effect to use", ArgInt, 1, &Args.SecondaryEffectType},
    {'m', "mix", "wet/dry mix factor of the effect", ArgFloat, 1, &Args.Mix},
    {'v', "value", "input value into the effect", ArgFloat, 1, &Args.Value},
  };
  Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result == Error) {
    return Result;
  }
  else if (Result == HelpStatus) {
    AudioEffectPrintHelp(stdout);
    return NoError;
  }
  else {
    if (!Args.Input) {
      fprintf(stderr, "No input audio file was given\n");
      return Result;
    }
    if (!Args.Output) {
      fprintf(stderr, "No output audio was given\n");
      return Result;
    }
  }
  return AudioEffectRun(&Args);
}
