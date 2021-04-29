// mixer.h

static i32 MixerInit(mixer* Mixer, i32 SampleRate, i32 FramesPerBuffer);

static i32 MixerAddBus(mixer* Mixer, i32 ChannelCount, float* Buffer);

static i32 MixerToggleActiveBus(mixer* Mixer, i32 BusIndex);

static i32 MixerClearBuffers(mixer* Mixer);

static i32 MixerSumBuses(mixer* Mixer, u8 IsPlaying, float* OutBuffer, float* InBuffer);

static i32 MixerRender(mixer* Mixer);

static void MixerFree(mixer* Mixer);
