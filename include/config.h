// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

#define TITLE "sdaw"

#define PROG_NAME "sdaw"

#define LOCAL_DATA_PATH "./build"
#define DATA_PATH "/usr/local/lib/" PROG_NAME
#define RES_DIR "data"

#define SAMPLE_RATE_DEFAULT 44100
#define FRAMES_PER_BUFFER_DEFAULT 256

static i32 G_SampleRate = SAMPLE_RATE_DEFAULT;
static i32 G_FramesPerBuffer = FRAMES_PER_BUFFER_DEFAULT;

static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i32 G_FullScreen = 0;
static i32 G_Vsync = 1;
static i32 G_AudioInput = 0;

static v3 UIColorBackground = V3(0.0f, 0.0f, 0.0f);
static v3 UIColorAccept = V3(0.25f, 0.8f, 0.25f);
static v3 UIColorDecline = V3(0.9f, 0.25f, 0.25f);
static v3 UIColorStandard = V3(0.25f, 0.25f, 0.90f);
static v3 UIColorLight = V3(0.5f, 0.5f, 0.5f);
static v3 UIColorInactive = V3(0.3f, 0.3f, 0.3f);
static v3 UIColorNotPresent = V3(0.1f, 0.1f, 0.1f);

static v2 UIButtonSize = V2(64, 24);

typedef enum variable_type {
  TypeUndefined = 0,
  TypeInt32,
  TypeFloat32,

  MaxVariableType,
} variable_type;

typedef struct variable {
  const char* Name;
  void* Data;
  u32 NumFields;
  variable_type Type;
} variable;

typedef struct config_parser_state {
  variable* Variables;
  u32 VariableCount;
  buffer Source;
  char* Index;
} config_parser_state;

i32 ConfigParserInit();

i32 ConfigRead();

i32 ConfigWrite(const char* Path);

i32 DefineVariable(const char* Name, void* Data, u32 NumFields, variable_type Type);

void ConfigParserFree();

#endif
