//
// main.c
// sdaw - simple daw (poor man's daw / audio generator)
//

#include "sdaw.c"

int main(int argc, char** argv) {
#if 0
  audio_source Audio;
  InitAudioSource(&Audio, SAMPLE_RATE, 1);
  GenerateSineWave(&Audio, 1.0f, 110);
  StoreAudioSource("sine.wav", &Audio);
  UnloadAudioSource(&Audio);
#endif
  return SdawStart(argc, argv);
}
