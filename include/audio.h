// audio.h

typedef struct audio_source {
  float* Buffer;
  i32 SampleCount;
  i32 ChannelCount;
} audio_source;

static i32 ConvertToFloatBuffer(float* OutBuffer, i16* InBuffer, u32 SampleCount);

static i32 ConvertToInt16Buffer(i16* OutBuffer, float* InBuffer, u32 SampleCount);

static void ClearFloatBuffer(float* Buffer, i32 Size);

static i32 LoadAudioSource(const char* Path, audio_source* Source);

static i32 StoreAudioSource(const char* Path, audio_source* Source);

static i32 InitAudioSource(audio_source* Source, u32 SampleCount, u32 ChannelCount);

static void UnloadAudioSource(audio_source* Source);
