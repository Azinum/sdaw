// common.c

inline float Lerp(float V0, float V1, float T) {
  return (1.0f - T) * V0 + T * V1;
}

inline float Pow(float A, float B) {
  float Result = 0.0f;

  Result = pow(A, B);

  return Result;
}

inline float SquareRoot(float A) {
  float Result = 0.0f;

  Result = sqrt(A);

  return Result;
}

inline float DistanceV2(v2 A, v2 B) {
  float Result = 0.0f;

  v2 Delta = DiffV2(A, B);
  Result = SquareRoot(Pow(Delta.X, 2.0f) + Pow(Delta.Y, 2.0f));

  return Result;
}

inline v2 DiffV2(v2 A, v2 B) {
  v2 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;

  return Result;
}

inline v2 AddV2(v2 A, v2 B) {
  v2 Result = A;

  Result.X += B.X;
  Result.Y += B.Y;

  return Result;
}

inline v2 DivV2(v2 A, v2 B) {
  v2 Result = A;

  if (B.X != 0.0f) Result.X /= B.X;
  if (B.Y != 0.0f) Result.Y /= B.Y;

  return Result;
}

inline i32 Inside(v2 P, v2 Start, v2 End) {
  i32 Result = 0;

  if (P.X >= Start.X && P.Y >= Start.Y && P.X <= End.X && P.Y <= End.Y) {
    return 1;
  }

  return Result;
}
