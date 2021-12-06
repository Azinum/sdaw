// effect.c

#define EFFECT_BUFFER_SIZE (1024 * 56)
static f32 EffectBuffer[EFFECT_BUFFER_SIZE] = {0};
static i32 EffectIndex = 0;
static i32 CurrentEffectIndex = 0;

void StubEffect(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount) {
  (void)Buffer; (void)ChannelCount; (void)FramesPerBuffer; (void)Mix; (void)Amount;
}

void Distortion(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount) {
  f32 Dry = 1 - Mix;
  f32 Wet = 1 - Dry;
  f32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    f32 WetFrame = *Iter;
    f32 DryFrame = WetFrame;
    WetFrame *= Amount;
    WetFrame = Clamp(WetFrame, -1.0f, 1.0f);
    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount) {
  f32 Dry = 1 - Mix;
  f32 Wet = 1 - Dry;
  f32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    f32 WetFrame = *Iter;
    f32 DryFrame = WetFrame;

    EffectBuffer[EffectIndex] = DryFrame;
    EffectIndex = (EffectIndex + 1) % EFFECT_BUFFER_SIZE;
    WetFrame = EffectBuffer[(EffectIndex & (i32)Amount) % EFFECT_BUFFER_SIZE];

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect2(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount) {
  f32 Dry = 1 - Mix;
  f32 Wet = 1 - Dry;
  f32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    f32 WetFrame = *Iter;
    f32 DryFrame = WetFrame;

    WetFrame = EffectBuffer[CurrentEffectIndex];
    EffectBuffer[(i32)(CurrentEffectIndex + (EFFECT_BUFFER_SIZE / (1.0f + FrameIndex))) % EFFECT_BUFFER_SIZE] = DryFrame;
    CurrentEffectIndex = (CurrentEffectIndex + 5) % EFFECT_BUFFER_SIZE;

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

