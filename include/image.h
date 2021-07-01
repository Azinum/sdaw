// image.h

#ifndef _IMAGE_H
#define _IMAGE_H

typedef struct image {
  u8* PixelBuffer;
  i32 Width;
  i32 Height;
  u16 Depth;
  u16 Pitch;
  u16 BytesPerPixel;
} image;

typedef struct color_rgb {
  u8 R;
  u8 G;
  u8 B;
} color_rgb;

typedef union color_rgba {
  struct {
    u8 R;
    u8 G;
    u8 B;
    u8 A;
  };
  struct {
    i32 Value;
  };
} color_rgba;

typedef struct color_rgb16 {
  u16 R;
  u16 G;
  u16 B;
} color_rgb16;

#define ColorRGB(_R, _G, _B) ((color_rgb) { .R = _R, .G = _G, .B = _B })
#define ColorRGBA(_R, _G, _B, _A) ((color_rgba) { .R = _R, .G = _G, .B = _B, .A = _A })
#define ColorRGBa(_R, _G, _B) ((color_rgba) { .R = _R, .G = _G, .B = _B, .A = 255 })
#define ColorRGB16(_R, _G, _B) ((color_rgb16) { .R = _R, .G = _G, .B = _B })

extern u8* FetchPixel(const image* Source, i32 X, i32 Y);

v3 ColorInvert(v3 Color);

v3 ColorGray(v3 Color);

v3 ColorGrayAverage(v3 Color);

v3 ColorGain(v3 Color, float Gain);

v3 RandomColor();

i32 LoadPNGFromFile(FILE* File, image* Image);

i32 StorePNGFromFile(FILE* File, image* Image);

i32 LoadPNG(const char* Path, image* Image);

i32 LoadImage(const char* Path, image* Image);

i32 LoadFileAsImage(const char* Path, image* Image);

i32 StoreImage(const char* Path, image* Image);

i32 InitImage(i32 Width, i32 Height, u16 BytesPerPixel, image* Image);

void UnloadImage(image* Image);

#endif
