// sampler.h

#ifndef _SAMPLER_H
#define _SAMPLER_H

i32 SamplerInit(instrument* Ins);

i32 SamplerProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

i32 SamplerFree(instrument* Ins);

#endif
