// stream.h

i32 StreamInit(i32 FramesPerBuffer, i32 ChannelCount, const char* Path);

i32 StreamStartRecording();

i32 StreamStopRecording();

u8 StreamIsRecording();

i32 StreamWriteBuffer(float* Buffer, u32 BufferSize);

void StreamFree();
