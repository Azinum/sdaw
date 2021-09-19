// effect.c

#define EFFECT_BUFFER_SIZE (1024 * 56)
static r32 EffectBuffer[EFFECT_BUFFER_SIZE] = {0};
static i32 EffectIndex = 0;
static i32 CurrentEffectIndex = 0;

void StubEffect(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount) {
  (void)Buffer; (void)ChannelCount; (void)FramesPerBuffer; (void)Mix; (void)Amount;
}

void Distortion(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount) {
  r32 Dry = 1 - Mix;
  r32 Wet = 1 - Dry;
  r32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    r32 WetFrame = *Iter;
    r32 DryFrame = WetFrame;
    WetFrame *= Amount;
    WetFrame = Clamp(WetFrame, -1.0f, 1.0f);
    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount) {
  r32 Dry = 1 - Mix;
  r32 Wet = 1 - Dry;
  r32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    r32 WetFrame = *Iter;
    r32 DryFrame = WetFrame;

    EffectBuffer[EffectIndex] = DryFrame;
    EffectIndex = (EffectIndex + 1) % EFFECT_BUFFER_SIZE;
    WetFrame = EffectBuffer[(EffectIndex & (i32)Amount) % EFFECT_BUFFER_SIZE];

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

void WeirdEffect2(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount) {
  r32 Dry = 1 - Mix;
  r32 Wet = 1 - Dry;
  r32* Iter = Buffer;

  for (i32 FrameIndex = 0; FrameIndex < FramesPerBuffer * ChannelCount; ++FrameIndex) {
    r32 WetFrame = *Iter;
    r32 DryFrame = WetFrame;

    WetFrame = EffectBuffer[CurrentEffectIndex];
    EffectBuffer[(i32)(CurrentEffectIndex + (EFFECT_BUFFER_SIZE / (1.0f + FrameIndex))) % EFFECT_BUFFER_SIZE] = DryFrame;
    CurrentEffectIndex = (CurrentEffectIndex + 5) % EFFECT_BUFFER_SIZE;

    *(Iter++) = (DryFrame * Dry) + (WetFrame * Wet);
  }
}

