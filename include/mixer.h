// mixer.h

static i32 MixerInit(mixer* Mixer, i32 ChannelCount, i32 FramesPerBuffer);

static i32 MixerInitBus(mixer* Mixer, i32 BusIndex, i32 ChannelCount, float* Buffer);

static i32 MixerClearBuffers(mixer* Mixer);

static i32 MixerSumBuses(mixer* Mixer, float* OutBuffer);

static void MixerFree(mixer* Mixer);
