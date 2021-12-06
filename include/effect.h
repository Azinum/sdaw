// effect.h

#ifndef _EFFECT_H
#define _EFFECT_H

typedef void (*effect_func)(f32*, i32, i32, f32, f32);

void StubEffect(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount);

void Distortion(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount);

void WeirdEffect(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount);

void WeirdEffect2(f32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, f32 Mix, f32 Amount);

#endif
