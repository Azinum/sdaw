// sampler.h

#ifndef _SAMPLER_H
#define _SAMPLER_H

static i32 SamplerInit(instrument* Ins);

static i32 SamplerProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

static i32 SamplerFree(instrument* Ins);

#endif
