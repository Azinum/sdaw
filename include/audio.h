// audio.h

#ifndef _AUDIO_H
#define _AUDIO_H

typedef struct audio_source {
  float* Buffer;
  i32 SampleCount;
  i32 ChannelCount;
} audio_source;

i32 ConvertToFloatBuffer(float* OutBuffer, i16* InBuffer, u32 SampleCount);

i32 ConvertToInt16Buffer(i16* OutBuffer, float* InBuffer, u32 SampleCount);

void ClearFloatBuffer(float* Buffer, i32 Size);

void CopyFloatBuffer(float* DestBuffer, float* Source, i32 Size);

i32 LoadAudioSource(const char* Path, audio_source* Source);

i32 LoadAudioSourceFromDataPath(const char* Path, audio_source* Source);

i32 StoreAudioSource(const char* Path, audio_source* Source);

i32 InitAudioSource(audio_source* Source, u32 SampleCount, u32 ChannelCount);

void UnloadAudioSource(audio_source* Source);

#endif
