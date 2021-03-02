// image.h

typedef struct image {
  u8* PixelBuffer;
  u32 Width;
  u32 Height;
  u16 Depth;
  u16 Pitch;
  u16 BytesPerPixel;
} image;
