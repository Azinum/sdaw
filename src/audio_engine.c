// audio_engine.c

audio_engine AudioEngine;

#if USE_PA
  #include "audio_pa.c"
#else
  #include "audio_sdl.c"
#endif

