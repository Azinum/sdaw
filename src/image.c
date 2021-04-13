// image.c
// utility for storing and loading image files

#include <png.h>

static i32 LoadPNGFromFile(FILE* File, image* Image) {
  i32 Result = NoError;
  png_structp PNG;
  png_infop Info;

  u8 Signature[8];
  if (fread(Signature, 1, sizeof(Signature), File) < 8) {
    Result = Error;
    goto Done;
  }

  if (png_sig_cmp(Signature, 0, 8)) {
    Result = Error;
    goto Done;
  }

  PNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!PNG) {
    Result = Error;
    goto Done;
  }

  Info = png_create_info_struct(PNG);
  if (!Info) {
    Result = Error;
    goto Done;
  }
  if (setjmp(png_jmpbuf(PNG))) {
    png_destroy_read_struct(&PNG, &Info, NULL);
    Result = Error;
    goto Done;
  }

  png_init_io(PNG, File);
  png_set_sig_bytes(PNG, sizeof(Signature));
  png_read_info(PNG, Info);

  Image->Width = png_get_image_width(PNG, Info);
  Image->Height = png_get_image_height(PNG, Info);
  Image->Depth = png_get_bit_depth(PNG, Info);

  if (Image->Depth < 8) {
    png_set_packing(PNG);
  }

  if (png_get_valid(PNG, Info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(PNG);
  }

  switch (png_get_color_type(PNG, Info)) {
    case PNG_COLOR_TYPE_GRAY: {
      Image->Pitch = 3 * Image->Width;
      png_set_gray_to_rgb(PNG);
      break;
    }
    case PNG_COLOR_TYPE_RGB: {
      Image->Pitch = 3 * Image->Width;
      break;
    }
    case PNG_COLOR_TYPE_RGBA: {
      Image->Pitch = 4 * Image->Width;
      break;
    }
    default: {
      // Assert(0);
      Image->Pitch = 3 * Image->Width;
      break;
    }
  }

  Image->BytesPerPixel = png_get_rowbytes(PNG, Info) / Image->Width;

  png_set_interlace_handling(PNG);
  png_read_update_info(PNG, Info);

  Image->PixelBuffer = malloc(sizeof(u8) * Image->Width * Image->Height * Image->BytesPerPixel);

  png_bytep* Rows = malloc(sizeof(png_bytep) * Image->Height);
  u8* Pixels = Image->PixelBuffer;
  for (u32 Row = 0; Row < Image->Height; ++Row) {
    Rows[Row] = Pixels;
    Pixels += Image->Width * Image->BytesPerPixel;
  }

  png_read_image(PNG, Rows);
  png_read_end(PNG, NULL);
  png_destroy_read_struct(&PNG, &Info, NULL);
  free(Rows);

Done:
  return Result;
}

static i32 StorePNGFromFile(FILE* File, image* Image) {
  i32 Result = Error;
  png_structp PNG = {0};
  png_infop Info = {0};

  PNG = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!PNG) {
    goto Done;
  }

  Info = png_create_info_struct(PNG);
  if (!Info)
    goto Done;

  if (setjmp(png_jmpbuf(PNG)))
    goto Done;

  png_init_io(PNG, File);

  png_set_IHDR(
    PNG,
    Info,
    Image->Width, Image->Height,
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_BASE
  );
  png_set_filter(PNG, PNG_FILTER_TYPE_BASE, PNG_FILTER_NONE);

  png_write_info(PNG, Info);
  png_set_filler(PNG, 0, PNG_FILLER_AFTER); // NOTE(lucas): This removes the alpha channel

  png_bytep row = (png_bytep)malloc(4 * Image->Width * sizeof(png_byte));

  for (u32 Y = 0; Y < Image->Height; ++Y) {
    for (u32 X = 0; X < Image->Width; ++X) {
      png_byte* Pixel = &(row[X * 4]);
      memcpy(Pixel, &Image->PixelBuffer[Image->BytesPerPixel * ((Y * Image->Width) + X)], Image->BytesPerPixel);
    }
    png_write_row(PNG, row);
  }

  free(row);
  png_write_end(PNG, NULL);

  Result = NoError;
Done:
  if (Result != NoError) {
    fprintf(stderr, "Failed to write PNG file\n");
  }
  if (PNG && Info) {
    png_destroy_write_struct(&PNG, &Info);
  }
  return Result;
}

static i32 LoadPNG(const char* Path, image* Image) {
  FILE* File = fopen(Path, "r");
  if (!File) {
    fprintf(stderr, "Failed to open '%s'\n", Path);
    return Error;
  }
  i32 Result = LoadPNGFromFile(File, Image);
  fclose(File);
  return Result;
}

static i32 LoadImage(const char* Path, image* Image) {
  return LoadPNG(Path, Image);
}

static i32 StoreImage(const char* Path, image* Image) {
  FILE* File = fopen(Path, "w");
  if (!File) {
    fprintf(stderr, "Failed to create '%s'\n", Path);
    return Error;
  }
  i32 Result = StorePNGFromFile(File, Image);
  fclose(File);
  return Result;
}

static i32 InitImage(u32 Width, u32 Height, u16 BytesPerPixel, image* Image) {
  Assert(Width > 0 && Height > 0 && Image);
  memset(Image, 0, sizeof(image));
  Image->PixelBuffer = malloc(BytesPerPixel * Width * Height * sizeof(u8));
  memset(Image->PixelBuffer, 0, BytesPerPixel * Width * Height * sizeof(u8));
  if (!Image->PixelBuffer) {
    return Error;
  }
  Image->Width = Width;
  Image->Height = Height;
  Image->Depth = 8; // 8 bits for each color component in any given pixel
  Image->Pitch = Width * BytesPerPixel;
  Image->BytesPerPixel = BytesPerPixel;
  return NoError;
}

static void UnloadImage(image* Image) {
  Assert(Image);
  if (Image->PixelBuffer != NULL) {
    free(Image->PixelBuffer);
  }
  memset(Image, 0, sizeof(image));
}
