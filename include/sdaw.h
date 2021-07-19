// sdaw.h

#ifndef _SDAW_H
#define _SDAW_H

#include "common.h"
#include "module.h"
#include "memory.h"
#include "hash.h"
#include "config.h"
#include "lut.h"
#include "debug.h"
#include "list.h"
#include "str.h"
#include "math_util.h"
#include "arg_parser.h"
#include "image.h"
#include "audio.h"
#include "riff.h"
#include "vorbis.h"
#include "image_seq.h"
#include "gen_audio.h"
#include "image_interp.h"

#ifndef NO_ENGINE
  #include "engine.h"
#endif

i32 SdawStart(i32 argc, char** argv);

#endif
