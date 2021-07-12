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
static u32 RectShader = 0,
  TextShader = 0;

static image FontTexture;
static u32 FontTextureId;

#define ERR_BUFFER_SIZE 512

static i32 CompileShaderFromSource(const char* VertSource, const char* FragSource, u32* Program);
static i32 CompileShader(const char* Path, u32* Program);
static void InitQuadData();
static void UploadTexture(image* Texture, u32* TextureId);

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

void UploadTexture(image* Texture, u32* TextureId) {
  i32 TextureFormat = Texture->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
  glGenTextures(1, TextureId);
  glBindTexture(GL_TEXTURE_2D, *TextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D, 0, TextureFormat, Texture->Width, Texture->Height, 0, TextureFormat, GL_UNSIGNED_BYTE, Texture->PixelBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererInit() {
  InitQuadData();
  View = Mat4D(1.0f);
  Model = Mat4D(1.0f);
  CompileShader(DataPathConcat("data/shader/rect"), &RectShader);
  CompileShader(DataPathConcat("data/shader/text"), &TextShader);
  LoadImage(DataPathConcat("data/texture/font_source_code_bold.png"), &FontTexture);
  UploadTexture(&FontTexture, &FontTextureId);
  RendererUpdateMatrices();
}

void RendererBeginFrame() {
  Clip = V4(0.0f, 0.0f, Window.Width, Window.Height);
}

void RendererEndFrame() {
  WindowSwapBuffers();
  WindowClear(UIColorBackground.R, UIColorBackground.G, UIColorBackground.B);
}

// Make function name a bit more descriptive
void RendererUpdateMatrices() {
  Projection = Orthographic(0.0f, (float)Window.Width, (float)Window.Height, 0.0f, -1.0f, 1.0f);
}

void SetClipping(v4 Clipping) {
  Clip = Clipping;
}

void DrawRect(v3 P, v2 Size, v3 Color) {
  u32 Handle = RectShader;
  glUseProgram(Handle);

  Model = Translate(P);

  Model = Translate2D(Model, 0.5f * Size.W, 0.5f * Size.H);
  Model = Rotate2D(Model, 0);
  Model = Translate2D(Model, -0.5f * Size.W, -0.5f * Size.H);
  Model = Scale2D(Model, Size.W, Size.H);

  // TODO(lucas): Pre-calculate the model-view-projection matrices into one single matrix
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);

  glUniform4f(glGetUniformLocation(Handle, "InColor"), Color.R, Color.G, Color.B, 1.0f);
  glUniform4f(glGetUniformLocation(Handle, "InBorderColor"), Color.R, Color.G, Color.B, 1.0f);
  glUniform1f(glGetUniformLocation(Handle, "Thickness"), 0);
  glUniform2f(glGetUniformLocation(Handle, "RectSize"), Size.W, Size.H);
  glUniform4f(glGetUniformLocation(Handle, "Clip"), Clip.X, Clip.Y, Clip.Z, Clip.W);

  glBindVertexArray(QuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, ArraySize(QuadVertices) / 4);
  glBindVertexArray(0);

  glUseProgram(0);
}

void DrawRectangle(v3 P, v2 Size, v3 Color, v3 BorderColor, float Thickness) {
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
  glUniform4f(glGetUniformLocation(Handle, "InBorderColor"), BorderColor.R, BorderColor.G, BorderColor.B, 1.0f);
  glUniform1f(glGetUniformLocation(Handle, "Thickness"), Thickness);
  glUniform2f(glGetUniformLocation(Handle, "RectSize"), Size.W, Size.H);
  glUniform4f(glGetUniformLocation(Handle, "Clip"), Clip.X, Clip.Y, Clip.Z, Clip.W);

  glBindVertexArray(QuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, ArraySize(QuadVertices) / 4);
  glBindVertexArray(0);

  glUseProgram(0);

}

void DrawText(v3 P, v2 Size, v3 Color, float Kerning, float Leading, i32 TextSize, const char* Text) {
  u32 Handle = TextShader;
  glUseProgram(Handle);

  image* Texture = &FontTexture;
  u32 TextureId = FontTextureId;

  i32 FontSize = Texture->Width;
  i32 TextLength = strlen(Text);
  v3 GlyphP = P;
  v2 GlyphSize = V2(
    TextSize,
    TextSize
  );
  v2 GlyphArea = V2(
    TextSize * Kerning,
    TextSize * Leading
  );

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniform4f(glGetUniformLocation(Handle, "Tint"), Color.R, Color.G, Color.B, 1.0f);
  glUniform4f(glGetUniformLocation(Handle, "Clip"), Clip.X, Clip.Y, Clip.Z, Clip.W);

  if (!Texture) {
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TextureId);
  glBindVertexArray(QuadVAO);
  for (i32 Index = 0; Index < TextLength; ++Index) {
    char Ch = Text[Index];
    if (Ch == '\0') {
      break;
    }
    if (GlyphP.X + GlyphArea.W >= P.X + Size.W || GlyphP.Y + GlyphArea.H >= P.Y + Size.H) {
      break;
    }
    if (Ch >= 32 && Ch < 127 && Ch != '\n') {
      v2 Offset = V2(
        0,
        (Ch - 32) * FontSize
      );
      v2 Range = V2(FontSize, FontSize);

      Model = Translate(GlyphP);
      Model = Scale2D(Model, TextSize, TextSize);

      glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);
      glUniform2f(glGetUniformLocation(Handle, "Offset"), Offset.X / Texture->Width, Offset.Y / Texture->Height);
      glUniform2f(glGetUniformLocation(Handle, "Range"), Range.X / Texture->Width, Range.Y / Texture->Height);

      glDrawArrays(GL_TRIANGLES, 0, ArraySize(QuadVertices) / 4);

      GlyphP.X += GlyphArea.W;
    }
    if (Ch == '\n') {
      GlyphP.X = P.X;
      GlyphP.Y += GlyphArea.H;
    }
  }

  glBindVertexArray(0);
}

void RendererResizeWindowCallback(i32 Width, i32 Height) {
  RendererUpdateMatrices();
}

void RendererFree() {
  UnloadImage(&FontTexture);
  glDeleteTextures(1, &FontTextureId);
  glDeleteShader(RectShader);
  glDeleteShader(TextShader);
  glDeleteVertexArrays(1, &QuadVAO);
  glDeleteVertexArrays(1, &QuadVBO);
}
