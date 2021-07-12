// config.h

#ifndef _CONFIG_H
#define _CONFIG_H

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
