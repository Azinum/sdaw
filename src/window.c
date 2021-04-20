// window.c

window Window;

static i8 KeyDown[GLFW_KEY_LAST] = {0};
static i8 KeyPressed[GLFW_KEY_LAST] = {0};

static void FrameBufferSizeCallback(GLFWwindow* Win, i32 Width, i32 Height) {
  glViewport(0, 0, Width, Height);
  glfwGetWindowSize(Win, &Width, &Height);
  Window.Width = Width;
  Window.Height = Height;
  Projection = Orthographic(0.0f, Window.Width, Window.Height, 0.0f, -1.0f, 1.0f);
}

static void ConfigureOpenGL() {
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glShadeModel(GL_FLAT);
  glEnable(GL_TEXTURE_2D);
  glAlphaFunc(GL_GREATER, 1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static i32 WindowOpen(u32 Width, u32 Height, const char* Title, u8 Vsync, u8 FullScreen) {
  Window.Title = Title;
  Window.FullScreen = FullScreen;
  Window.Width = Window.InitWidth = Width;
  Window.Height = Window.InitHeight = Height;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  if (Window.FullScreen) {
    const GLFWvidmode* Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    Window.Width = Mode->width;
    Window.Height = Mode->height;
    Window.Window = glfwCreateWindow(Window.Width, Window.Height, Title, glfwGetPrimaryMonitor(), NULL);
  }
  else {
    Window.Window = glfwCreateWindow(Window.Width, Window.Height, Title, NULL, NULL);
  }
  if (!Window.Window) {
    fprintf(stderr, "Failed to create window\n");
    return -1;
  }

  glfwMakeContextCurrent(Window.Window);
  glfwSetFramebufferSizeCallback(Window.Window, FrameBufferSizeCallback);

  i32 Error = glewInit();
  if (Error != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(Error));
    return -1;
  }
  glfwSwapInterval(Vsync);
  ConfigureOpenGL();
  return 0;
}

static void WindowSetFullScreen() {
  const GLFWvidmode* Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  Window.Width = Mode->width;
  Window.Height = Mode->height;
  Window.FullScreen = 1;
  glfwSetWindowSize(Window.Window, Window.Width, Window.Height);
}

static void WindowToggleFullScreen() {
  Window.FullScreen = !Window.FullScreen;
  if (Window.FullScreen) {
    const GLFWvidmode* Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    Window.Width = Mode->width;
    Window.Height = Mode->height;
  }
  else {
    Window.Width = Window.InitWidth;
    Window.Height = Window.InitHeight;
  }
  glfwSetWindowSize(Window.Window, Window.Width, Window.Height);
}

static i32 WindowPollEvents() {
  glfwPollEvents();
  glfwGetCursorPos(Window.Window, &MouseX, &MouseY);

  for (u16 Index = 0; Index < GLFW_KEY_LAST; Index++) {
    i32 KeyState = glfwGetKey(Window.Window, Index);
    if (KeyState == GLFW_PRESS) {
      KeyPressed[Index] = !KeyDown[Index];
      KeyDown[Index] = 1;
    }
    else {
      KeyDown[Index] = 0;
      KeyPressed[Index] = 0;
    }
  }

  i32 LeftMouseButtonState = glfwGetMouseButton(Window.Window, 0);
  i32 RightMouseButtonState = glfwGetMouseButton(Window.Window, 1);
  i32 MiddleMouseButtonState = glfwGetMouseButton(Window.Window, 2);

  (LeftMouseButtonState && !(MouseState & (1 << 7))) ? MouseState |= (1 << 6) : (MouseState &= ~(1 << 6));
  LeftMouseButtonState ? MouseState |= (1 << 7) : (MouseState &= ~(1 << 7));
  (RightMouseButtonState && !(MouseState & (1 << 5))) ? MouseState |= (1 << 4) : (MouseState &= ~(1 << 4));
  RightMouseButtonState ? MouseState |= (1 << 5) : (MouseState &= ~(1 << 5));

  (MiddleMouseButtonState && !(MouseState & (1 << 3))) ? MouseState |= (1 << 2) : (MouseState &= ~(1 << 2));
  MiddleMouseButtonState ? MouseState |= (1 << 3) : (MouseState &= ~(1 << 3));

  if (KeyPressed[GLFW_KEY_F11]) {
    WindowToggleFullScreen();
  }

  if (KeyPressed[GLFW_KEY_ESCAPE]) {
    return -1;
  }
  return 0;
}

static void WindowSwapBuffers() {
  glfwSwapBuffers(Window.Window);
}

static void WindowClear(float R, float G, float B) {
  glClearColor(R, G, B, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void WindowClose() {
  glfwDestroyWindow(Window.Window);
  glfwTerminate();
}
