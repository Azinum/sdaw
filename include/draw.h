// draw.h

#ifndef _DRAW_H
#define _DRAW_H

static mat4 Projection;
static mat4 View;
static mat4 Model;
static v4 Clip;

void RendererInit();

void DrawRect(v3 P, v2 Size, v3 Color);

void DrawRectangle(v3 P, v2 Size, v3 Color, v3 BorderColor, float Thickness);

void DrawText(v3 P, v2 Size, v3 Color, const char* Text);

void RendererFree();

#endif
