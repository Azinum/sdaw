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

static const char* VertSource =
  "#version 330 core\n"
  "\n"
  "layout (location = 0) in vec4 Vertex;\n"
  "\n"
  "out vec2 TexCoords;\n"
  "\n"
  "uniform mat4 Projection;\n"
  "uniform mat4 View;\n"
  "uniform mat4 Model;\n"
  "\n"
  "void main() {\n"
  "	TexCoords = Vertex.zw;\n"
  "	gl_Position = Projection * View * Model * vec4(Vertex.xy, 0, 1.0);\n"
  "}\n";

static const char* FragSource =
  "#version 330 core\n"
  "\n"
  "in vec2 TexCoords;\n"
  "out vec4 Color;\n"
  "\n"
  "uniform vec4 InColor;\n"
  "\n"
  "void main() {\n"
  "	Color = InColor;\n"
  "	if (Color.r == 1 && Color.g == 0 && Color.b == 1) {\n"
  "		discard;\n"
  "	}\n"
  "}\n";

#define ERR_BUFFER_SIZE 512

static i32 CompileShaderFromSource(const char* VertSource, const char* FragSource, u32* Program);
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
    fprintf(stderr, "%s\n", ErrorLog);
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
    fprintf(stderr, "%s\n", ErrorLog);
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
  CompileShaderFromSource(VertSource, FragSource, &RectShader);
}

void DrawRect(v3 P, i32 W, i32 H) {
  u32 Handle = RectShader;
  glUseProgram(Handle);

  Model = Translate(P);

  Model = Translate2D(Model, 0.5f * W, 0.5f * H);
  Model = Rotate2D(Model, 0);
  Model = Translate2D(Model, -0.5f * W, -0.5f * H);
  Model = Scale2D(Model, W, H);

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);
  glUniform4f(glGetUniformLocation(Handle, "InColor"), 1.0f, 0.0f, 0.0f, 1.0f);

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
