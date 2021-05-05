// engine.h

#include "audio_engine.h"
#include "mixer.h"
#include "instrument.h"
#include "effect.h"
#include "osc_test.h"
#include "sampler.h"
#include "audio_input.h"
#include "draw.h"
#include "debug_ui.h"
#include "window.h"

static i32 EngineInit();

static void EngineFree();
