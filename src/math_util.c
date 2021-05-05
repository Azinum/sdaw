// math_util.c

inline mat4 Mat4D(float Diagonal) {
  mat4 Result = {0};

  Result.Elements[0][0] = Diagonal;
  Result.Elements[1][1] = Diagonal;
  Result.Elements[2][2] = Diagonal;
  Result.Elements[3][3] = Diagonal;

  return Result;
}

inline float InnerV2(v2 A, v2 B) {
  float Result = 0;

  Result = (A.X * B.X) + (A.Y * B.Y);

  return Result;
}

inline v2 DifferenceV2(v2 A, v2 B) {
  v2 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;

  return Result;
}

inline v3 DifferenceV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;
  Result.Z -= B.Z;

  return Result;
}

inline float Powf(float A, float B) {
  float Result = 0;

  Result = powf(A, B);

  return Result;
}

inline float SquareRoot(float A) {
  float Result = 0;
#if USE_SSE
  Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(A)));
#else
  Result = sqrt(A);
#endif
  return Result;
}

inline float DistanceV2(v2 A, v2 B) {
  float Result = 0;
  v2 Delta = DifferenceV2(A, B);

  Result = SquareRoot(Powf(Delta.X, 2.0f) + Powf(Delta.Y, 2.0f));

  return Result;
}

inline float DistanceV3(v3 A, v3 B) {
  float Result = 0;

  v3 Delta = DifferenceV3(A, B);
  Result = SquareRoot(Powf(Delta.X, 2.0f) + Powf(Delta.Y, 2.0f) + Powf(Delta.Z, 2.0f));

  return Result;
}

inline float ToRadians(float Degrees) {
  return Degrees * (PI32 / 180.0f);
}

inline float Lerp(float V0, float V1, float T) {
  return (1.0f - T) * V0 + T * V1;
}

inline v2 LerpV2(v2 V0, v2 V1, v2 T) {
  return V2(
    Lerp(V0.X, V1.X, T.X),
    Lerp(V0.Y, V1.Y, T.Y)
  );
}

inline v2 LerpV2t(v2 V0, v2 V1, float T) {
  return V2(
    Lerp(V0.X, V1.X, T),
    Lerp(V0.Y, V1.Y, T)
  );
}

inline v3 LerpV3t(v3 V0, v3 V1, float T) {
  return V3(
    Lerp(V0.X, V1.X, T),
    Lerp(V0.Y, V1.Y, T),
    Lerp(V0.Z, V1.Z, T)
  );
}

inline mat4 Translate(v3 T) {
  mat4 Result = Mat4D(1.0f);

  Result.Elements[3][0] += T.X;
  Result.Elements[3][1] += T.Y;
  Result.Elements[3][2] += T.Z;

  return Result;
}

inline mat4 TranslateMat4(mat4 A, v3 T) {
  mat4 Result = A;

  Result.Elements[3][0] += T.X;
  Result.Elements[3][1] += T.Y;
  Result.Elements[3][2] += T.Z;

  return Result;
}

inline v3 MultiplyMatrixVector(mat4 M, v3 A) {
  v3 Result;
  float X = A.X, Y = A.Y, Z = A.Z;

  Result.X = X * M.Elements[0][0] + Y * M.Elements[1][0] + Z * M.Elements[2][0] + M.Elements[3][0];
  Result.Y = X * M.Elements[0][1] + Y * M.Elements[1][1] + Z * M.Elements[2][1] + M.Elements[3][1];
  Result.Z = X * M.Elements[0][2] + Y * M.Elements[1][2] + Z * M.Elements[2][2] + M.Elements[3][2];
  float W =  X * M.Elements[0][3] + Y * M.Elements[1][3] + Z * M.Elements[2][3] + M.Elements[3][3];

  if (W != 0.0f) {
    Result.X /= W;
    Result.Y /= W;
    Result.Z /= W;
  }

  return Result;
}

inline v3 MultiplyMatrixVectorW(mat4 M, v3 A, float* W) {
  v3 Result;
  float X = A.X, Y = A.Y, Z = A.Z;

  Result.X = X * M.Elements[0][0] + Y * M.Elements[1][0] + Z * M.Elements[2][0] + M.Elements[3][0];
  Result.Y = X * M.Elements[0][1] + Y * M.Elements[1][1] + Z * M.Elements[2][1] + M.Elements[3][1];
  Result.Z = X * M.Elements[0][2] + Y * M.Elements[1][2] + Z * M.Elements[2][2] + M.Elements[3][2];
  *W =       X * M.Elements[0][3] + Y * M.Elements[1][3] + Z * M.Elements[2][3] + M.Elements[3][3];

  if (*W != 0.0f) {
    // Result.X /= *W;
    // Result.Y /= *W;
    // Result.Z /= *W;
  }

  return Result;
}

inline float DotVec3(v3 A, v3 B) {
  float Result = 0;

  Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);

  return Result;
}

inline float LengthSquareVec3(v3 A) {
  float Result = 0;

  Result = DotVec3(A, A);

  return Result;
}

inline float LengthVec3(v3 A) {
  float Result = 0;

  Result = sqrtf(LengthSquareVec3(A));

  return Result;
}

inline v3 NormalizeVec3(v3 A) {
  v3 Result = {0};

  float Length = LengthVec3(A);

  if (Length != 0.0f) {
    Result.X = A.X * (1.0f / Length);
    Result.Y = A.Y * (1.0f / Length);
    Result.Z = A.Z * (1.0f / Length);
  }

  return Result;
}

inline v3 Cross(v3 A, v3 B) {
  v3 Result = {0};

  Result.X = (A.Y * B.Z) - (A.Z * B.Y);
  Result.Y = (A.Z * B.X) - (A.X * B.Z);
  Result.Z = (A.X * B.Y) - (A.Y * B.X);

  return Result;
}

inline v3 MultiplyV3(v3 A, float Value) {
  v3 Result = A;

  Result.X *= Value;
  Result.Y *= Value;
  Result.Z *= Value;

  return Result;
}

inline v3 MultiplyToV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X *= B.X;
  Result.Y *= B.Y;
  Result.Z *= B.Z;

  return Result;
}

inline v2 AddV2(v2 A, float Value) {
  v2 Result = A;

  Result.X += Value;
  Result.Y += Value;

  return Result;
}

inline v3 AddV3(v3 A, float Value) {
  v3 Result = A;

  Result.X += Value;
  Result.Y += Value;
  Result.Z += Value;

  return Result;
}

inline v2 DivideV2(v2 A, float Value) {
  v2 Result = A;

  if (Value != 0.0f) {
    Result.X /= Value;
    Result.Y /= Value;
  }

  return Result;
}

inline v3 DivideV3(v3 A, float Value) {
  v3 Result = A;

  if (Value != 0.0f) {
    Result.X /= Value;
    Result.Y /= Value;
    Result.Z /= Value;
  }

  return Result;
}

inline v2 AddToV2(v2 A, v2 B) {
  v2 Result = A;

  Result.X += B.X;
  Result.Y += B.Y;

  return Result;
}

inline v3 AddToV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X += B.X;
  Result.Y += B.Y;
  Result.Z += B.Z;

  return Result;
}

inline v3 SubToV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;
  Result.Z -= B.Z;

  return Result;
}

inline mat4 MultiplyMat4(mat4 A, mat4 B) {
  mat4 Result;

#if USE_SSE
  mat4 Left = Transpose(A);
  mat4 Right = Transpose(B);

  Result.Rows[0] = LinearCombineSSE(Left.Rows[0], Right);
  Result.Rows[1] = LinearCombineSSE(Left.Rows[1], Right);
  Result.Rows[2] = LinearCombineSSE(Left.Rows[2], Right);
  Result.Rows[3] = LinearCombineSSE(Left.Rows[3], Right);

  Result = Transpose(Result);
#else
  for (i32 Column = 0; Column < 4; ++Column) {
    for (i32 Row = 0; Row < 4; ++Row) {
      float Sum = 0;
      i32 CurrentMatrice;
      for (CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice) {
        Sum += A.Elements[CurrentMatrice][Row] * B.Elements[Column][CurrentMatrice];
      }
      Result.Elements[Column][Row] = Sum;
    }
  }
#endif
  return Result;
}

inline mat4 MultiplyMat4f(mat4 A, float B) {
  mat4 Result;

  __m128 Scalar = _mm_set1_ps(B);
  Result.Rows[0] = _mm_mul_ps(A.Rows[0], Scalar);
  Result.Rows[1] = _mm_mul_ps(A.Rows[1], Scalar);
  Result.Rows[2] = _mm_mul_ps(A.Rows[2], Scalar);
  Result.Rows[3] = _mm_mul_ps(A.Rows[3], Scalar);

  return Result;
}

inline mat4 Rotate(float Angle, v3 Axis) {
  mat4 Result = Mat4D(1.0f);

  Axis = NormalizeVec3(Axis);

  float SinTheta = sinf(ToRadians(Angle));
  float CosTheta = cosf(ToRadians(Angle));
  float CosValue = 1.0f - CosTheta;

  Result.Elements[0][0] = (Axis.X * Axis.X * CosValue) + CosTheta;
  Result.Elements[0][1] = (Axis.X * Axis.Y * CosValue) + (Axis.Z * SinTheta);
  Result.Elements[0][2] = (Axis.X * Axis.Z * CosValue) - (Axis.Y * SinTheta);

  Result.Elements[1][0] = (Axis.Y * Axis.X * CosValue) - (Axis.Z * SinTheta);
  Result.Elements[1][1] = (Axis.Y * Axis.Y * CosValue) + CosTheta;
  Result.Elements[1][2] = (Axis.Y * Axis.Z * CosValue) + (Axis.X * SinTheta);

  Result.Elements[2][0] = (Axis.Z * Axis.X * CosValue) + (Axis.Y * SinTheta);
  Result.Elements[2][1] = (Axis.Z * Axis.Y * CosValue) - (Axis.X * SinTheta);
  Result.Elements[2][2] = (Axis.Z * Axis.Z * CosValue) + CosTheta;

  return Result;
}

inline mat4 ScaleMat4(v3 A) {
  mat4 Result = Mat4D(1.0f);

  Result.Elements[0][0] = A.X;
  Result.Elements[1][1] = A.Y;
  Result.Elements[2][2] = A.Z;

  return Result;
}

inline mat4 ScaleOnMat4(mat4 A, v3 B) {
  mat4 Result = A;

  Result.Elements[0][0] *= B.X;
  Result.Elements[1][1] *= B.Y;
  Result.Elements[2][2] *= B.Z;

  return Result;
}

inline mat4 Perspective(float Fov, float AspectRatio, float ZNear, float ZFar) {
  mat4 Result = {0};

  float TanThetaOver2 = tanf(Fov * (PI32 / 360.0f));

  Result.Elements[0][0] = 1.0f / TanThetaOver2;
  Result.Elements[1][1] = AspectRatio / TanThetaOver2;
  Result.Elements[2][3] = -1.0f;
  Result.Elements[2][2] = (ZNear + ZFar) / (ZNear - ZFar);
  Result.Elements[3][2] = (1.0f * ZNear * ZFar) / (ZNear - ZFar);
  Result.Elements[3][3] = 0.0f;

  return Result;
}

inline mat4 Orthographic(float Left, float Right, float Bottom, float Top, float ZNear, float ZFar) {
  mat4 Result = {0};

  Result.Elements[0][0] = 2.0f / (Right - Left);
  Result.Elements[1][1] = 2.0f / (Top - Bottom);
  Result.Elements[2][2] = 2.0f / (ZNear - ZFar);
  Result.Elements[3][3] = 1.0f;

  Result.Elements[3][0] = (Left + Right) / (Left - Right);
  Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
  Result.Elements[3][2] = (ZFar + ZNear) / (ZNear - ZFar);

  return Result;
}

inline mat4 LookAt(v3 Eye, v3 Center, v3 Up) {
  mat4 Result = Mat4D(1.0f);

  v3 F = NormalizeVec3(DifferenceV3(Center, Eye));
  v3 S = NormalizeVec3(Cross(F, Up));
  v3 U = Cross(S, F);

  Result.Elements[0][0] = S.X;
  Result.Elements[0][1] = U.X;
  Result.Elements[0][2] = -F.X;

  Result.Elements[1][0] = S.Y;
  Result.Elements[1][1] = U.Y;
  Result.Elements[1][2] = -F.Y;

  Result.Elements[2][0] = S.Z;
  Result.Elements[2][1] = U.Z;
  Result.Elements[2][2] = -F.Z;

  Result.Elements[3][0] = -DotVec3(S, Eye);
  Result.Elements[3][1] = -DotVec3(U, Eye);
  Result.Elements[3][2] = DotVec3(F, Eye);
  Result.Elements[3][3] = 1.0f;

  return Result;
}

inline mat4 InverseMat4(mat4 A) {
  mat4 Result;

  Result.Elements[0][0] = A.Elements[0][0]; Result.Elements[0][1] = A.Elements[1][0]; Result.Elements[0][2] = A.Elements[2][0]; Result.Elements[0][3] = 0.0f;
  Result.Elements[1][0] = A.Elements[0][1]; Result.Elements[1][1] = A.Elements[1][1]; Result.Elements[1][2] = A.Elements[2][1]; Result.Elements[1][3] = 0.0f;
  Result.Elements[2][0] = A.Elements[0][2]; Result.Elements[2][1] = A.Elements[1][2]; Result.Elements[2][2] = A.Elements[2][2]; Result.Elements[2][3] = 0.0f;

  Result.Elements[3][0] = -(A.Elements[3][0] * Result.Elements[0][0] + A.Elements[3][1] * Result.Elements[1][0] + A.Elements[3][2] * Result.Elements[2][0]);
  Result.Elements[3][1] = -(A.Elements[3][0] * Result.Elements[0][1] + A.Elements[3][1] * Result.Elements[1][1] + A.Elements[3][2] * Result.Elements[2][1]);
  Result.Elements[3][2] = -(A.Elements[3][0] * Result.Elements[0][2] + A.Elements[3][1] * Result.Elements[1][2] + A.Elements[3][2] * Result.Elements[2][2]);

  Result.Elements[3][3] = 1.0f;

  return Result;
}

inline float Sin(float Value) {
  float Result = 0.0f;

  Result = sinf(Value);

  return Result;
}

inline i32 Inside(v2 P, v2 Start, v2 End) {
  i32 Result = 0;

  if (P.X >= Start.X && P.Y >= Start.Y && P.X <= End.X && P.Y <= End.Y) {
    return 1;
  }

  return Result;
}

void PrintMat4(FILE* File, mat4 A) {
  for (i32 Column = 0; Column < 4; ++Column) {
    for (i32 Row = 0; Row < 4; ++Row) {
      float Value = A.Elements[Column][Row];
      if (Row == 0) fprintf(File, "[");
      fprintf(File, "%3g", Value);
      if (Row < 3) fprintf(File, ", ");
      if (Row == 3) fprintf(File, "]\n");
    }
  }
}

void PrintV3(FILE* File, v3 A) {
  fprintf(File, "(%3g, %3g, %3g)\n", A.X, A.Y, A.Z);
}

#if USE_SSE

inline mat4 Transpose(mat4 A) {
  mat4 Result = A;

  _MM_TRANSPOSE4_PS(Result.Rows[0], Result.Rows[1], Result.Rows[2], Result.Rows[3]);

  return Result;
}

inline __m128 LinearCombineSSE(__m128 Left, mat4 Right) {
  __m128 Result;

  Result = _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x00), Right.Rows[0]);
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x55), Right.Rows[1]));
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xaa), Right.Rows[2]));
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xff), Right.Rows[3]));

  return Result;
}

#endif  // USE_SSE
