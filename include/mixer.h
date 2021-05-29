// mixer.h

#ifndef _MIXER_H
#define _MIXER_H

static i32 MixerInit(mixer* Mixer, i32 SampleRate, i32 FramesPerBuffer);

static i32 MixerAddBus(mixer* Mixer, i32 ChannelCount, float* Buffer);

static bus* MixerAddBus0(mixer* Mixer, i32 ChannelCount, float* Buffer, i32* BusIndex);

static i32 MixerRemoveBus(mixer* Mixer, i32 BusIndex);

static i32 MixerAttachInstrumentToBus(mixer* Mixer, i32 BusIndex, instrument* Ins);

static i32 MixerAttachInstrumentToBus0(mixer* Mixer, bus* Bus, instrument* Ins);

static i32 MixerToggleActiveBus(mixer* Mixer, i32 BusIndex);

static i32 MixerClearBuffers(mixer* Mixer);

static i32 MixerSumBuses(mixer* Mixer, u8 IsPlaying, float* OutBuffer, float* InBuffer);

static i32 MixerRender(mixer* Mixer);

static void MixerFree(mixer* Mixer);

#endif
