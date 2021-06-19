// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

#define TITLE "sdaw"

#define PROG_NAME "sdaw"

#define LOCAL_DATA_PATH "./build"
#define DATA_PATH "/usr/local/lib/" PROG_NAME
#define RES_DIR "resource"

#define SAMPLE_RATE_DEFAULT 44100
#define FRAMES_PER_BUFFER_DEFAULT 512

static i32 G_SampleRate = SAMPLE_RATE_DEFAULT;
static i32 G_FramesPerBuffer = FRAMES_PER_BUFFER_DEFAULT;
static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i32 G_FullScreen = 0;
static i32 G_Vsync = 0;
static i32 G_AudioInput = 0;

static v3 UIColorBackground = V3(0.7f, 0.7f, 0.7f);
static v3 UIColorAccept = V3(0.25f, 0.8f, 0.25f);
static v3 UIColorDecline = V3(0.9f, 0.25f, 0.25f);
static v3 UIColorStandard = V3(0.25f, 0.25f, 0.90f);
static v3 UIColorLight = V3(0.5f, 0.5f, 0.5f);
static v3 UIColorInactive = V3(0.3f, 0.3f, 0.3f);
static v3 UIColorNotPresent = V3(0.1f, 0.1f, 0.1f);

static const char* S_SampleRate = "sample_rate";
static const char* S_FramesPerBuffer = "frames_per_buffer";
static const char* S_WindowWidth = "window_width";
static const char* S_WindowHeight = "window_height";
static const char* S_FullScreen = "fullscreen";
static const char* S_Vsync = "vsync";
static const char* S_AudioInput = "audio_input";

static const char* S_UIColorBackground = "ui_color_background";
static const char* S_UIColorAccept = "ui_color_accept";
static const char* S_UIColorDecline = "ui_color_decline";
static const char* S_UIColorStandard = "ui_color_standard";
static const char* S_UIColorLight = "ui_color_light";
static const char* S_UIColorInactive = "ui_color_inactive";
static const char* S_UIColorNotPresent = "ui_color_not_present";

i32 WriteConfig(const char* Path);

i32 LoadConfig();

#endif
