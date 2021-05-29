// engine.h

#ifndef _ENGINE_H
#define _ENGINE_H

#include "audio_engine.h"
#include "mixer.h"
#include "instrument.h"
#include "effect.h"
#include "serial_midi.h"
#include "osc_test.h"
#include "sampler.h"
#include "audio_input.h"
#include "draw.h"
#include "debug_ui.h"
#include "window.h"

extern float NoteTable[];

i32 EngineInit();

void EngineFree();

#endif
