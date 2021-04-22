// draw.h

static mat4 Projection;
static mat4 View;
static mat4 Model;
static v4 Clip;

void RendererInit();

void DrawRect(v3 P, i32 W, i32 H);

void RendererFree();
