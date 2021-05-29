// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

#define TITLE "sdaw"

#define PROG_NAME "sdaw"
#define DATA_PATH "/var/lib/" PROG_NAME

#define SAMPLE_RATE_DEFAULT 44100
#define FRAMES_PER_BUFFER_DEFAULT 512

static i32 G_SampleRate = SAMPLE_RATE_DEFAULT;
static i32 G_FramesPerBuffer = FRAMES_PER_BUFFER_DEFAULT;
static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i32 G_FullScreen = 0;
static i32 G_Vsync = 1;

static const char* S_SampleRate = "sample_rate";
static const char* S_FramesPerBuffer = "frames_per_buffer";
static const char* S_WindowWidth = "window_width";
static const char* S_WindowHeight = "window_height";
static const char* S_FullScreen = "fullscreen";
static const char* S_Vsync = "vsync";

i32 LoadConfig();

#endif
