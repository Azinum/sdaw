// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

#define TITLE "sdaw"

#define PROG_NAME "sdaw"

#define LOCAL_DATA_PATH "./build"
#define DATA_PATH "/usr/local/lib/" PROG_NAME
#define RES_DIR "data"

#define SAMPLE_RATE_DEFAULT 44100
#define FRAMES_PER_BUFFER_DEFAULT 512

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
static v3 UIColorContainer = V3(0.32f, 0.20f, 0.25f);
static v3 UIColorContainerBright = V3(0.7f, 0.7f, 0.7f);
static v3 UIColorButton = V3(0.9f, 0.9f, 0.9f);
static v3 UIColorBorder = V3(0, 0, 0);
static float UIBorderThickness = 0.0f;

static v2 UIButtonSize = V2(64, 38);
static float UIMargin = 12.0f;
static i32 UITextSize = 11;
static float UITextKerning = 0.6f;
static float UITextLeading = 1.5f;

static i32 G_GamepadButtonUp = 11;
static i32 G_GamepadButtonDown = 12;
static i32 G_GamepadButtonLeft = 13;
static i32 G_GamepadButtonRight = 14;

static i32 G_PaInputDevice = -1;
static i32 G_PaOutputDevice = -1;

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
  hash_table VariableLocations;
  buffer Source;
  char* Index;
} config_parser_state;

i32 ConfigParserInit();

i32 ConfigRead();

i32 ConfigWrite(const char* Path);

i32 DefineVariable(const char* Name, void* Data, u32 NumFields, variable_type Type);

void ConfigParserFree();

#endif
