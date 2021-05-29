// osc_test.h

#ifndef _OSC_TEST_H
#define _OSC_TEST_H

struct note_state;

struct note_state* OscTestPlayNote(i32 FreqIndex, float AttackTime, float ReleaseTime, float Velocity);

i32 OscTestProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate);

void OscTestIncrAttackTime(float Amount);

void OscTestIncrReleaseTime(float Amount);

static i32 OscTestInit(instrument* Ins);

static i32 OscTestFree(instrument* Ins);

#endif
