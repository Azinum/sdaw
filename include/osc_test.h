// osc_test.h

struct note_state;

struct note_state* OscTestPlayNote(i32 FreqIndex, float AttackTime, float ReleaseTime);

i32 OscTestProcess(float* Buffer, i32 ChannelCount, i32 FramesPerBuffer, i32 SampleRate);

void OscTestIncrAttackTime(float Amount);

void OscTestIncrReleaseTime(float Amount);

void OscTestRender();
