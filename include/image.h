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
#define ColorRGB16(_R, _G, _B) ((color_rgb16) { .R = _R, .G = _G, .B = _B })
