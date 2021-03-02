// sdaw.c

#include "sdaw.h"

#include "common.c"
#include "image.c"
#include "audio.c"
#include "riff.c"

i32 SdawStart(i32 argc, char** argv) {
  audio_source Source;
  if (LoadAudioSource("test.wav", &Source) == NoError) {
    if (StoreAudioSource("test_copy.wav", &Source) == NoError) {
      printf("StoreAudioSource: Ok\n");
    }
    else {
      printf("StoreAudioSource: Error\n");
    }
    UnloadAudioSource(&Source);
    printf("LoadAudioSource: Ok\n");
  }
  else {
    printf("LoadAudioSource: Error\n");
  }
  return 0;
}
