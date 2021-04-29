// draw.c

#if __APPLE__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else

#include <GL/gl.h>

#endif

static float QuadVertices[] = {
  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f,

  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
};

static u32 QuadVAO = 0;
static u32 QuadVBO = 0;
static u32 RectShader = 0;

#define ERR_BUFFER_SIZE 512

static i32 CompileShaderFromSource(const char* VertSource, const char* FragSource, u32* Program);
static i32 CompileShader(const char* Path, u32* Program);
static void InitQuadData();

i32 CompileShaderFromSource(const char* VertSource, const char* FragSource, u32* Program) {
  i32 Result = NoError;
  i32 Report = -1;
  char ErrorLog[ERR_BUFFER_SIZE] = {0};
  u32 VertShader = 0;
  u32 FragShader = 0;

  VertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertShader, 1, &VertSource, NULL);
  glCompileShader(VertShader);
{
  glGetShaderiv(VertShader, GL_COMPILE_STATUS, &Report);
  if (!Report) {
    glGetShaderInfoLog(VertShader, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "vertex shader: %s\n", ErrorLog);
    Result = Error;
    goto Done;
  }
}

  FragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragShader, 1, &FragSource, NULL);
  glCompileShader(FragShader);
{
  glGetShaderiv(FragShader, GL_COMPILE_STATUS, &Report);
  if (!Report) {
    glGetShaderInfoLog(FragShader, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "fragment shader: %s\n", ErrorLog);
    Result = Error;
    goto Done;
  }
}

  *Program = glCreateProgram();
  glAttachShader(*Program, VertShader);
  glAttachShader(*Program, FragShader);
  glLinkProgram(*Program);

{
  glGetProgramiv(*Program, GL_VALIDATE_STATUS, &Report);
  if (Report != GL_NO_ERROR) {
    glGetProgramInfoLog(*Program, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "%s", ErrorLog);
    goto Done;
  }
}

Done:
  if (VertShader > 0) glDeleteShader(VertShader);
  if (FragShader > 0) glDeleteShader(FragShader);
  return Result;
}

i32 CompileShader(const char* Path, u32* Program) {
  i32 Result = NoError;
  char VertPath[MAX_PATH_SIZE] = {0};
  char FragPath[MAX_PATH_SIZE] = {0};
  snprintf(VertPath, MAX_PATH_SIZE, "%s.vert", Path);
  snprintf(FragPath, MAX_PATH_SIZE, "%s.frag", Path);
  buffer VertSource = {0};
  buffer FragSource = {0};

  if ((Result = ReadFileAndNullTerminate(VertPath, &VertSource)) != NoError) {
    goto Done;
  }
  if ((Result = ReadFileAndNullTerminate(FragPath, &FragSource)) != NoError) {
    goto Done;
  }

  Result = CompileShaderFromSource(VertSource.Data, FragSource.Data, Program);
Done:
  BufferFree(&VertSource);
  BufferFree(&FragSource);
  return Result;
}

void InitQuadData() {
  glGenVertexArrays(1, &QuadVAO);
  glGenBuffers(1, &QuadVBO);

  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

  glBindVertexArray(QuadVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void RendererInit() {
  InitQuadData();
  View = Mat4D(1.0f);
  Model = Mat4D(1.0f);
  CompileShader("resource/shader/rect", &RectShader);
}

void DrawRect(v3 P, v2 Size, v3 Color) {
  u32 Handle = RectShader;
  glUseProgram(Handle);

  Model = Translate(P);

  Model = Translate2D(Model, 0.5f * Size.W, 0.5f * Size.H);
  Model = Rotate2D(Model, 0);
  Model = Translate2D(Model, -0.5f * Size.W, -0.5f * Size.H);
  Model = Scale2D(Model, Size.W, Size.H);

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);
  glUniform4f(glGetUniformLocation(Handle, "InColor"), Color.R, Color.G, Color.B, 1.0f);
  glUniform4f(glGetUniformLocation(Handle, "Clip"), Clip.X, Clip.Y, Clip.Z, Clip.W);

  glBindVertexArray(QuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, ArraySize(QuadVertices) / 4);
  glBindVertexArray(0);

  glUseProgram(0);
}

void RendererFree() {
  glDeleteShader(RectShader);
  glDeleteVertexArrays(1, &QuadVAO);
  glDeleteVertexArrays(1, &QuadVBO);
}
