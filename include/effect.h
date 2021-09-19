// effect.h

#ifndef _EFFECT_H
#define _EFFECT_H

typedef void (*effect_func)(r32*, i32, i32, r32, r32);

void StubEffect(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount);

void Distortion(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount);

void WeirdEffect(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount);

void WeirdEffect2(r32* Buffer, i32 ChannelCount, i32 FramesPerBuffer, r32 Mix, r32 Amount);

#endif
