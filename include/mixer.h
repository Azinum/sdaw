// mixer.h

#ifndef _MIXER_H
#define _MIXER_H

i32 MixerInit(mixer* Mixer, i32 SampleRate, i32 FramesPerBuffer);

i32 MixerAddBus(mixer* Mixer, i32 ChannelCount, float* Buffer);

bus* MixerAddBus0(mixer* Mixer, i32 ChannelCount, float* Buffer, i32* BusIndex);

i32 MixerRemoveBus(mixer* Mixer, i32 BusIndex);

i32 MixerAttachInstrumentToBus(mixer* Mixer, i32 BusIndex, instrument* Ins);

i32 MixerAttachInstrumentToBus0(mixer* Mixer, bus* Bus, instrument* Ins);

i32 MixerToggleActiveBus(mixer* Mixer, i32 BusIndex);

i32 MixerClearBuffers(mixer* Mixer);

i32 MixerSumBuses(mixer* Mixer, u8 IsPlaying, float* OutBuffer, float* InBuffer);

i32 MixerRender(mixer* Mixer);

void MixerFree(mixer* Mixer);

#endif
