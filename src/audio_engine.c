// audio_engine.c

#include <portaudio.h>

audio_engine AudioEngine;
static PaStream* Stream = NULL;
static PaStreamParameters OutPort;

static i32 StereoCallback(const void* InBuffer, void* OutBuffer, unsigned long FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData) {
  (void)InBuffer;
  (void)TimeInfo;
  (void)Flags;
  (void)UserData;

  float* Out = (float*)OutBuffer;
  AudioEngine.Out = Out;
  mixer* Mixer = &AudioEngine.Mixer;

  MixerSumBusses(Mixer, Out);

  if (AudioEngine.IsPlaying) {
    const float DeltaTime = (1.0f / AudioEngine.SampleRate) * FramesPerBuffer;
    AudioEngine.Time += DeltaTime;
    AudioEngine.Tick += FramesPerBuffer;
  }
  return paContinue;
}

static i32 OpenStream() {
  PaError Error = Pa_OpenStream(
    &Stream,
    NULL,
    &OutPort,
    AudioEngine.SampleRate,
    AudioEngine.FramesPerBuffer,
    0,
    StereoCallback,
    NULL
  );

  if (Error != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[PortAudio Error]: %s\n", Pa_GetErrorText(Error));
    return Error;
  }
  Pa_StartStream(Stream);
  return NoError;
}

i32 AudioEngineInit(u32 SampleRate, u32 FramesPerBuffer) {
  PaError Err = Pa_Initialize();
  if (Err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[PortAudio Error]: %s\n", Pa_GetErrorText(Err));
    return -1;
  }

  AudioEngine.SampleRate = SampleRate;
  AudioEngine.FramesPerBuffer = FramesPerBuffer;
  AudioEngine.Tick = 0;
  AudioEngine.Out = NULL;
  AudioEngine.Time = 0;
  AudioEngine.IsPlaying = 1;

  i32 OutputDevice = Pa_GetDefaultOutputDevice();
  OutPort.device = OutputDevice;
  OutPort.channelCount = 2;
  OutPort.sampleFormat = paFloat32; // paInt16;
  OutPort.suggestedLatency = Pa_GetDeviceInfo(OutPort.device)->defaultLowOutputLatency;
  OutPort.hostApiSpecificStreamInfo = NULL;

  MixerInit(&AudioEngine.Mixer, 2 /* channel count */, FramesPerBuffer);

  return NoError;
}

i32 AudioEngineStart(callback Callback) {
  i32 Result = OpenStream();
  if (Result != NoError) {
    return Result;
  }

  if (Callback) {
    Callback();
  }

  return NoError;
}

void AudioEngineTerminate() {
  MixerFree(&AudioEngine.Mixer);
  Pa_CloseStream(Stream);
  Pa_Terminate();
}
