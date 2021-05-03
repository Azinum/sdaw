// audio_input.h

static i32 AudioInputInit(instrument* Ins);

static i32 AudioInputProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

static i32 AudioInputFree(instrument* Ins);
