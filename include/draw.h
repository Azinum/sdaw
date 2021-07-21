// draw.h

#ifndef _DRAW_H
#define _DRAW_H

static mat4 Projection;
static mat4 View;
static mat4 Model;
static v4 Clip;

void RendererInit();

void UploadTexture(image* Texture, u32* TextureId);

void UnloadTexture(u32* TextureId);

void RendererBeginFrame();

void RendererEndFrame();

void RendererUpdateMatrices();

void SetClipping(v4 Clipping);

void DrawRect(v3 P, v2 Size, v3 Color);

void DrawRectangle(v3 P, v2 Size, v3 Color, v3 BorderColor, float Thickness, float Angle);

void DrawText(v3 P, v2 Size, v3 Color, float Kerning, float Leading, i32 TextSize, const char* Text);

void DrawString(v3 P, v2 Size, v3 Color, float Kerning, float Leading, i32 TextSize, const char* Text, u32 TextLength);

void RendererResizeWindowCallback(i32 Width, i32 Height);

void RendererFree();

#endif
