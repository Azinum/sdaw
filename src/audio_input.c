// audio_input.c
// audio input instrument

i32 AudioInputInit(instrument* Ins) {
  return NoError;
}

i32 AudioInputProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  float* Input = AudioEngine.In;
  if (Bus->Buffer && Input) {
    // CopyFloatBuffer(Bus->Buffer, Input, sizeof(float) * Bus->ChannelCount * FramesPerBuffer);
    CopyFloatBufferEliminateEven(Bus->Buffer, Input, sizeof(float) * Bus->ChannelCount * FramesPerBuffer);
  }
  return NoError;
}

i32 AudioInputFree(instrument* Ins) {
  return NoError;
}
