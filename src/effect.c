// effect.c

#define EFFECT_BUFFER_SIZE (1024 * 32)
static float EffectBuffer[EFFECT_BUFFER_SIZE] = {0};
static i32 EffectIndex = 0;
static i32 CurrentEffectIndex = 0;

void Distortion(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;
    WetFrame *= Amount;
    WetFrame = Clamp(WetFrame, -1.0f, 1.0f);
    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;

    EffectBuffer[EffectIndex] = DryFrame;
    EffectIndex = (EffectIndex + 1) % EFFECT_BUFFER_SIZE;
    WetFrame = EffectBuffer[(EffectIndex & (i32)Amount) % EFFECT_BUFFER_SIZE];

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect2(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, float Mix, float Amount) {
  float Dry = 1 - Mix;
  float Wet = 1 - Dry;
  float* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    float WetFrame = *Iter;
    float DryFrame = WetFrame;

    WetFrame = EffectBuffer[CurrentEffectIndex];
    EffectBuffer[(i32)(CurrentEffectIndex + (EFFECT_BUFFER_SIZE / (1.0f + FrameIndex))) % EFFECT_BUFFER_SIZE] = DryFrame;
    CurrentEffectIndex = (CurrentEffectIndex + 5) % EFFECT_BUFFER_SIZE;

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

