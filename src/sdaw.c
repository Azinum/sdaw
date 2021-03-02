// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "image.c"
#include "audio.c"
#include "riff.c"

i32 SdawStart(i32 argc, char** argv) {
  const char* Path = "test.wav";
  audio_source Source;
  if (LoadAudioSource(Path, &Source) == NoError) {
    fprintf(stdout, "Ok\n");
    UnloadAudioSource(&Source);
  }
  else {
    fprintf(stderr, "Failed to load WAVE file '%s'\n", Path);
  }
  return 0;
}
