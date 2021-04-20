// window.h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if __APPLE__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else

#include <GL/gl.h>

#endif

typedef struct window {
  const char* Title;
  u8 FullScreen;
  u32 Width;
  u32 Height;
  u32 InitWidth;
  u32 InitHeight;
  void* Window;
} window;

extern window Window;

static double MouseX = 0;
static double MouseY = 0;
static i8 MouseState = 0;

#define LeftMouseDown      (MouseState & (1 << 7))
#define LeftMousePressed   (MouseState & (1 << 6))
#define RightMouseDown     (MouseState & (1 << 5))
#define RightMousePressed  (MouseState & (1 << 4))

#define MiddleMouseDown    (MouseState & (1 << 3))
#define MiddleMousePressed (MouseState & (1 << 2))
