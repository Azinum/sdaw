// image_interp.c

typedef struct image_interp_args {
  char* Path;
  char* From;
  char* To;
  float InterpFactor;
} image_interp_args;

static i32 InterpolateImages(image_interp_args* Args);

i32 InterpolateImages(image_interp_args* Args) {
  TIMER_START();

  i32 Result = NoError;
  image Image0;
  image Image1;
  image Dest;

  if ((Result = LoadImage(Args->From, &Image0)) != NoError) {
    fprintf(stderr, "Failed to read image file '%s'\n", Args->From);
    goto Done;
  }
  if ((Result = LoadImage(Args->To, &Image1)) != NoError) {
    fprintf(stderr, "Failed to read image file '%s'\n", Args->To);
    goto Done;
  }

  if ((Result = InitImage(Image0.Width, Image0.Height, Image0.BytesPerPixel, &Dest)) != NoError) {
    fprintf(stderr, "Failed to initialize blank image\n");
    goto Done;
  }

  for (i32 Y = 0; Y < Dest.Height; ++Y) {
    for (i32 X = 0; X < Dest.Width; ++X) {
      color_rgba* Color = (color_rgba*)FetchPixel(&Dest, X, Y);
      color_rgba* From = (color_rgba*)FetchPixel(&Image0, X, Y);
      v2 Scaling = V2(
        (float)Image1.Width / Image0.Width,
        (float)Image1.Height / Image0.Height
      );
      color_rgba* To = (color_rgba*)FetchPixel(&Image1, X * Scaling.X, Y * Scaling.Y);
      *Color = ColorRGBA(
        Lerp(From->R, To->R, Args->InterpFactor),
        Lerp(From->G, To->G, Args->InterpFactor),
        Lerp(From->B, To->B, Args->InterpFactor),
        255
      );
    }
  }

  StoreImage(Args->Path, &Dest);

Done:
  UnloadImage(&Image0);
  UnloadImage(&Image1);
  UnloadImage(&Dest);

  TIMER_END(
    printf("%s: %g s\n", __FUNCTION__, _DeltaTime);
  );
  return Result;
}

i32 ImageInterp(i32 argc, char** argv) {
  i32 Result = NoError;
  image_interp_args Args = {
    .Path = "out.png",
    .From = NULL,
    .To = NULL,
    .InterpFactor = 0.5f,
  };

  parse_arg Arguments[] = {
    {'o', "output-path", "path to output image", ArgString, 1, &Args.Path},
    {'f', "from", "image to interpolate from", ArgString, 1, &Args.From},
    {'t', "to", "image to interpolate to", ArgString, 1, &Args.To},
    {'F', "interp-factor", "interpolation factor (default: 0.5)", ArgFloat, 1, &Args.InterpFactor},
  };

  Result = ParseArgs(Arguments, ArraySize(Arguments), argc, argv);
  if (Result != NoError) {
    return Result;
  }
  if (!Args.From) {
    fprintf(stderr, "Missing 'from' image file\n");
    return Error;
  }
  if (!Args.To) {
    fprintf(stderr, "Missing 'to' image file\n");
    return Error;
  }
  Result = InterpolateImages(&Args);
  return Result;
}
