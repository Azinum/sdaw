// audio_sdl.c

#include <SDL2/SDL.h>

static SDL_AudioSpec InSpec = {0};
static SDL_AudioSpec OutSpec = {0};
static SDL_AudioSpec AudioSpec = {0};
static i32 InputDevice = 0;
static i32 OutputDevice = 0;

static void* StereoThreadCallback(void* UserData);

void StereoCallback(void* UserData, u8* Stream, i32 Length) {
  if (Length <= 0) {
    return;
  }
  AudioEngineProcess(NULL, (void*)Stream);
}

i32 AudioEngineInit(audio_engine* Engine, i32 SampleRate, i32 FramesPerBuffer) {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "Failed to initialize SDL audio subsystem\n");
    return Error;
  }

  i32 Driver = 0;
  const char* DesiredDriver = "alsa";
  const char* DriverName = NULL;
  for (; Driver < SDL_GetNumAudioDrivers(); ++Driver) {
    const char* AudioDriver = SDL_GetAudioDriver(Driver);
    printf("Driver: %s", AudioDriver);
    if (!strcmp(AudioDriver, DesiredDriver)) {
      DriverName = AudioDriver;
      printf(" [SELECTED]");
    }
    printf("\n");
  }

  if (SDL_AudioInit(DriverName) != 0) {
    fprintf(stderr, "Failed to initialize audio driver: %s\n", DriverName);
    return Error;
  }

  memset(&InSpec, 0, sizeof(InSpec));
  InSpec.freq = SampleRate;
  InSpec.format = AUDIO_F32;
  InSpec.channels = 2;
  InSpec.samples = FramesPerBuffer;
  InSpec.callback = NULL; // StereoCallbackIn;

  if ((InputDevice = SDL_OpenAudioDevice(NULL, 1, &InSpec, &AudioSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE)) == 0) {
    fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
  }
  else {
    if (InSpec.format != AudioSpec.format) {
      fprintf(stderr, "Format that was requested is not supported\n");
    }
  }

  for (i32 DeviceIndex = 0; DeviceIndex < SDL_GetNumAudioDevices(1); ++DeviceIndex) {
    const char* DeviceName = SDL_GetAudioDeviceName(DeviceIndex, 1 /* input device*/);
    fprintf(stdout, "%-2i: %s", DeviceIndex, DeviceName);
    if (DeviceIndex == InputDevice) {
      fprintf(stdout, " [SELECTED INPUT DEVICE]");
    }
    fprintf(stdout, "\n");
  }

  memset(&OutSpec, 0, sizeof(OutSpec));
  OutSpec.freq = SampleRate;
  OutSpec.format = AUDIO_F32;
  OutSpec.channels = 2;
  OutSpec.samples = FramesPerBuffer;
  OutSpec.callback = StereoCallback;
  if ((OutputDevice = SDL_OpenAudioDevice(NULL, 0, &OutSpec, &AudioSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE)) == 0) {
    fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
  }
  else {
    if (OutSpec.format != AudioSpec.format) {
      fprintf(stderr, "Format that was requested is not supported\n");
    }
  }

  for (i32 DeviceIndex = 0; DeviceIndex < SDL_GetNumAudioDevices(0); ++DeviceIndex) {
    const char* DeviceName = SDL_GetAudioDeviceName(DeviceIndex, 0);
    fprintf(stdout, "%-2i: %s", DeviceIndex, DeviceName);
    if (DeviceIndex == OutputDevice) {
      fprintf(stdout, " [SELECTED OUTPUT DEVICE]");
    }
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "Using SDL audio backend\n");
  return NoError;
}

i32 AudioEngineStart(callback Callback) {
  SDL_PauseAudioDevice(OutputDevice, 0);
  SDL_PauseAudioDevice(InputDevice, 0);
  if (!AudioEngine.Initialized) {
    return Error;
  }

  if (Callback) {
    Callback(&AudioEngine);
  }
  return NoError;
}

void AudioEngineExit() {
  SDL_CloseAudioDevice(InputDevice);
  SDL_CloseAudioDevice(OutputDevice);
  SDL_AudioQuit();
}
