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
