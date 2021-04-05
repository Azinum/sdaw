// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "arg_parser.c"
#include "image.c"
#include "audio.c"
#include "riff.c"
#include "gen_audio.c"
#include "gen_image.c"

i32 ImageToAudioGen = 0;
i32 AudioToImageGen = 0;

static parse_arg Arguments[] = {
  {'g', "audio-gen", "image to audio generator", ArgInt, 0, &ImageToAudioGen},
  {'i', "image-gen", "audio to image generator", ArgInt, 0, &AudioToImageGen},
};

i32 SdawStart(i32 argc, char** argv) {
  srand(time(NULL));
  if (argc <= 1) {
    ArgsPrintHelp(stdout, Arguments, ArraySize(Arguments), argc, argv);
    return NoError;
  }
  i32 Result = ParseArgs(Arguments, ArraySize(Arguments), 2 /* parse only the first command */, argv);
  if (Result != NoError)
    return Result;

  if (ImageToAudioGen) {
    if ((Result = GenAudio(argc - 1, &argv[1])) != NoError) {
      return Result;
    }
  }
  else if (AudioToImageGen) {
    if ((Result = GenImage(argc - 1, &argv[1])) != NoError) {
      return Result;
    }
  }
  return NoError;
}
