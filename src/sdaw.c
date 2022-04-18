// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "module.c"
#include "memory.c"
#include "hash.c"
#include "config.c"
#include "lut.c"
#include "debug.c"
#include "list.c"
#include "str.c"
#include "math_util.c"
#include "arg_parser.c"
#include "image.c"
#include "audio.c"
#include "riff.c"
#include "vorbis.c"
#include "image_seq.c"
#include "gen_audio.c"
#include "image_interp.c"
#include "audio_effect.c"
#include "audio_convert.c"

#ifndef NO_ENGINE
  #include "engine.c"
#else
  #define EngineInit() NoError
  #define EngineFree()
#endif

typedef struct options {
  i32 ImageToAudioGen;
  i32 AudioToImageGen;
  i32 ImageInterpolation;
  i32 AudioEffect;
  i32 AudioConvert;
} options;

i32 SdawStart(i32 argc, char** argv) {
  i32 Result = NoError;
  srand(time(NULL));

  ConfigParserInit();
  ConfigRead();

#if INSTALL_APPLE && __APPLE__
  Result = EngineInit();
  EngineFree();
#else
  options Options = (options) {
    .ImageToAudioGen = 0,
    .AudioToImageGen = 0,
    .ImageInterpolation = 0,
    .AudioEffect = 0,
    .AudioConvert = 0,
  };
  parse_arg Arguments[] = {
    {'a', "audio-gen", "image to audio generator", ArgInt, 0, &Options.ImageToAudioGen},
    {'i', "image-seq", "audio to image sequence generator", ArgInt, 0, &Options.AudioToImageGen},
    {'I', "image-interpolate", "image interpolation", ArgInt, 0, &Options.ImageInterpolation},
    {'e', "effect", "apply audio effects on audio files", ArgInt, 0, &Options.AudioEffect},
    {'c', "audio-convert", "convert audio from one format to the other", ArgInt, 0, &Options.AudioConvert},
  };

  if (argc <= 1) {
    Result = EngineInit();
    EngineFree();
  }
  else {
    Result = ParseArgs(Arguments, ArraySize(Arguments), 2 /* only parse the first command */, argv);
    if (Result != NoError) {
      return Result;
    }
    if (Options.ImageToAudioGen) {
     Result = GenAudio(argc - 1, &argv[1]);
    }
    else if (Options.AudioToImageGen) {
     Result = ImageSeq(argc - 1, &argv[1]);
    }
    else if (Options.ImageInterpolation) {
     Result = ImageInterp(argc - 1, &argv[1]);
    }
    else if (Options.AudioEffect) {
     Result = AudioEffect(argc - 1, &argv[1]);
    }
    else if (Options.AudioConvert) {
     Result = AudioConvert(argc - 1, &argv[1]);
    }
  }
#endif
  ConfigParserFree();
  if (MemoryTotal() != 0) {
    fprintf(stderr, "Memory leak!\n");
    MemoryPrintInfo(stdout);
    Assert(0);
  }
  return Result;
}
