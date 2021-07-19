// audio_input.h

#ifndef _AUDIO_INPUT_H
#define _AUDIO_INPUT_H

i32 AudioInputInit(instrument* Ins);

i32 AudioInputProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

i32 AudioInputDraw(instrument* Ins);

i32 AudioInputFree(instrument* Ins);

#endif
