// audio_engine.c

#include <portaudio.h>

static audio_engine AudioEngine;
static PaStream* Stream = NULL;
static PaStreamParameters OutPort;

static i32 StereoCallback(const void* InBuffer, void* OutBuffer, unsigned long FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData) {
  (void)InBuffer;
  (void)TimeInfo;
  (void)Flags;
  (void)UserData;

  float* Out = (float*)OutBuffer;
  float* Iter = Out;
  AudioEngine.Out = Out;
  static float Freq = 60 << 1;

  for (u32 FrameIndex = 0; FrameIndex < FramesPerBuffer; ++FrameIndex) {
    float FrameL = 0.0f;
    float FrameR = 0.0f;
    if (AudioEngine.IsPlaying) {
      float Frame = 0.5f * sin((AudioEngine.Tick * Freq * 2 * PI32) / AudioEngine.SampleRate);
      FrameL = Frame;
      FrameR = Frame;
      ++AudioEngine.Tick;
    }
    *(Iter++) = FrameL;
    *(Iter++) = FrameR;
  }

  if (AudioEngine.IsPlaying) {
    const float DeltaTime = (1.0f / AudioEngine.SampleRate) * FramesPerBuffer;
    AudioEngine.Time += DeltaTime;
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
  Pa_CloseStream(Stream);
  Pa_Terminate();
}
