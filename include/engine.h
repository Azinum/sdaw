// engine.h

#ifndef _ENGINE_H
#define _ENGINE_H

#include "stream.h"
#include "audio_engine.h"
#include "mixer.h"
#include "instrument.h"
#include "effect.h"

#include "midi.h"
#include "midi_serial.h"
#include "midi_apple.h"

#include "osc_test.h"
#include "sampler.h"
#include "audio_input.h"
#include "draw.h"
#include "ui.h"
#include "window.h"

extern float NoteTable[];

i32 EngineInit();

void EngineFree();

#endif
