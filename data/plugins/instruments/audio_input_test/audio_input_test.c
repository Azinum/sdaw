// audio_input_test.c
// simple daw test instrument plugin

#include <sdaw/sdaw.h>

static i32 PluginInit(struct instrument* Ins);
static i32 PluginProcess(struct instrument* Ins, struct bus* Bus, i32 FramesPerBuffer, i32 SampleRate);
static i32 PluginDraw(struct instrument* Ins);
static i32 PluginFree(struct instrument* Ins);
static instrument_def ExportInstrument();

i32 PluginInit(struct instrument* Ins) {
  return NoError;
}

i32 PluginProcess(struct instrument* Ins, struct bus* Bus, i32 FramesPerBuffer, i32 SampleRate) {
  return NoError;
}

i32 PluginDraw(struct instrument* Ins) {
  return NoError;
}

i32 PluginFree(struct instrument* Ins) {
  return NoError;
}

instrument_def ExportInstrument() {
  return (instrument_def) {
    .Name = "Test Plugin",
    .Init = PluginInit,
    .Destroy = PluginFree,
    .Draw = PluginDraw,
    .Process = PluginProcess,
  };
}
