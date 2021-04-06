// image.h

typedef struct image {
  u8* PixelBuffer;
  u32 Width;
  u32 Height;
  u16 Depth;
  u16 Pitch;
  u16 BytesPerPixel;
} image;

typedef struct color_rgb {
  u8 R;
  u8 G;
  u8 B;
} color_rgb;

typedef struct color_rgb16 {
  u16 R;
  u16 G;
  u16 B;
} color_rgb16;

#define ColorRGB(_R, _G, _B) ((color_rgb) { .R = _R, .G = _G, .B = _B })
#define ColorRGB16(_R, _G, _B) ((color_rgb16) { .R = _R, .G = _G, .B = _B })
