// image.c
// utility for storing and loading of image files

#include <png.h>

inline u8* FetchPixel(const image* Source, i32 X, i32 Y) {
  return &Source->PixelBuffer[(Source->BytesPerPixel * ((X + (Y * Source->Width))) % (Source->BytesPerPixel * (Source->Width * Source->Height)))];
}

i32 LoadPNGFromFile(FILE* File, image* Image) {
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

  Image->PixelBuffer = M_Malloc(sizeof(u8) * Image->Width * Image->Height * Image->BytesPerPixel);

  i32 RowSize = sizeof(png_bytep) * Image->Height;
  png_bytep* Rows = M_Malloc(RowSize);
  u8* Pixels = Image->PixelBuffer;
  for (i32 Row = 0; Row < Image->Height; ++Row) {
    Rows[Row] = Pixels;
    Pixels += Image->Width * Image->BytesPerPixel;
  }

  png_read_image(PNG, Rows);
  png_read_end(PNG, NULL);
  png_destroy_read_struct(&PNG, &Info, NULL);
  M_Free(Rows, RowSize);

Done:
  return Result;
}

i32 StorePNGFromFile(FILE* File, image* Image) {
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

  i32 RowSize = 4 * Image->Width * sizeof(png_byte);
  png_bytep Row = (png_bytep)M_Malloc(RowSize);

  for (i32 Y = 0; Y < Image->Height; ++Y) {
    for (i32 X = 0; X < Image->Width; ++X) {
      png_byte* Pixel = &(Row[X * 4]);
      memcpy(Pixel, &Image->PixelBuffer[Image->BytesPerPixel * ((Y * Image->Width) + X)], Image->BytesPerPixel);
    }
    png_write_row(PNG, Row);
  }

  M_Free(Row, RowSize);
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

i32 LoadPNG(const char* Path, image* Image) {
  FILE* File = fopen(Path, "r");
  if (!File) {
    fprintf(stderr, "Failed to open '%s'\n", Path);
    return Error;
  }
  i32 Result = LoadPNGFromFile(File, Image);
  fclose(File);
  return Result;
}

i32 LoadImage(const char* Path, image* Image) {
  char* Ext = FetchExtension(Path);
  if (!strncmp(Ext, ".png", MAX_PATH_SIZE)) {
    return LoadPNG(Path, Image);
  }
  else {
    fprintf(stderr, "%s: Extension '%s' not supported for file '%s'\n", __FUNCTION__, Ext, Path);
  }
  return Error;
}

i32 LoadFileAsImage(const char* Path, image* Image) {
  i32 Result = NoError;
  buffer Buffer;
  memset(Image, 0, sizeof(image));
  if ((Result = ReadFile(Path, &Buffer)) == NoError) {
    i32 BytesPerPixel = 4;
    i32 Width = 1024;
    i32 Height = 1024;
    i32 FileIndex = 0;
    if ((Result = InitImage(Width, Height, BytesPerPixel, Image)) == NoError) {
      for (i32 Y = 0; Y < Image->Height; ++Y) {
        for (i32 X = 0; X < Image->Width; ++X) {
          color_rgb* Color = (color_rgb*)FetchPixel(Image, X, Y);
          char Data[] = {
            Buffer.Data[(FileIndex + 0) % Buffer.Count],
            Buffer.Data[(FileIndex + 1) % Buffer.Count],
            Buffer.Data[(FileIndex + 2) % Buffer.Count],
            Buffer.Data[(FileIndex + 3) % Buffer.Count],
          };
          FileIndex += 4;
          *Color = *(color_rgb*)Data;
        }
      }
    }
    BufferFree(&Buffer);
  }
  return Result;
}

i32 StoreImage(const char* Path, image* Image) {
  char* Ext = FetchExtension(Path);
  if (!strncmp(Ext, ".png", MAX_PATH_SIZE)) {
    FILE* File = fopen(Path, "w");
    if (!File) {
      fprintf(stderr, "Failed to create '%s'\n", Path);
      return Error;
    }
    i32 Result = StorePNGFromFile(File, Image);
    fclose(File);
    return Result;
  }
  else {
    fprintf(stderr, "%s: Extension '%s' not supported for file '%s'\n", __FUNCTION__, Ext, Path);
  }
  return Error;
}

i32 InitImage(i32 Width, i32 Height, u16 BytesPerPixel, image* Image) {
  Assert(Width > 0 && Height > 0 && Image);
  memset(Image, 0, sizeof(image));
  Image->PixelBuffer = M_Malloc(BytesPerPixel * Width * Height * sizeof(u8));
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

void UnloadImage(image* Image) {
  Assert(Image);
  if (Image->PixelBuffer != NULL) {
    M_Free(Image->PixelBuffer, sizeof(u8) * Image->Width * Image->Height * Image->BytesPerPixel);
  }
  memset(Image, 0, sizeof(image));
}
