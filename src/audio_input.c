// audio_input.c
// audio input instrument

typedef struct audio_input_data {
  u8 MonoL;
  u8 MonoR;
  u8 Distort;
  u8 Weird;
} audio_input_data;

i32 AudioInputInit(instrument* Ins) {
  i32 Result = NoError;
  if ((Result = InstrumentAllocUserData(Ins, sizeof(audio_input_data))) == NoError) {
    audio_input_data* Data = (audio_input_data*)Ins->UserData.Data;
    Data->MonoL = 0;
    Data->MonoR = 0;
    Data->Distort = 0;
    Data->Weird = 0;
  }
  return Result;
}

i32 AudioInputProcess(instrument* Ins, bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  audio_input_data* Data = (audio_input_data*)Ins->UserData.Data;
  if (!Data) {
    return NoError;
  }

  float* Input = AudioEngine.In;
  if (!Input) {
    return NoError;
  }

  if (Bus->Buffer && Input) {
    if (Data->MonoL) {
      CopyFloatBufferEliminateEven(Bus->Buffer, Input, sizeof(float) * Bus->ChannelCount * FramesPerBuffer);
    }
    else if (Data->MonoR) {
      CopyFloatBufferEliminateOdd(Bus->Buffer, Input, sizeof(float) * Bus->ChannelCount * FramesPerBuffer);
    }
    else {
      CopyFloatBuffer(Bus->Buffer, Input, sizeof(float) * Bus->ChannelCount * FramesPerBuffer);
    }
    if (Data->Weird) {
      WeirdEffect(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.25f, 2000.0f);
    }
    if (Data->Distort) {
      Distortion(Bus->Buffer, Bus->ChannelCount, FramesPerBuffer, 0.25f, 80.0f);
    }
  }
  return NoError;
}

i32 AudioInputDraw(instrument* Ins) {
  audio_input_data* Data = (audio_input_data*)Ins->UserData.Data;
  UI_DoTextToggle(UI_ID, "Stereo -> Mono (L)", &Data->MonoL);
  UI_DoTextToggle(UI_ID, "Stereo -> Mono (R)", &Data->MonoR);
  UI_DoTextToggle(UI_ID, "Distort", &Data->Distort);
  UI_DoTextToggle(UI_ID, "Weird", &Data->Weird);
  return NoError;
}

i32 AudioInputFree(instrument* Ins) {
  audio_input_data* Data = (audio_input_data*)Ins->UserData.Data;
  Assert(Data);
  return NoError;
}
