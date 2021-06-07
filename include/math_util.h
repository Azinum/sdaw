// math_util.h

#ifndef _MATH_UTIL_H
#define _MATH_UTIL_H

#ifndef NO_SSE

#if __SSE__
#define USE_SSE 1
#endif

#endif

#if USE_SSE
#include <xmmintrin.h>
#endif

#define Translate2D(MODEL, X, Y) MultiplyMat4(MODEL, Translate(V3(X, Y, 0)))

#define Rotate2D(MODEL, ANGLE) MultiplyMat4(MODEL, Rotate(ANGLE, V3(0, 0, 1)))

#define Scale2D(MODEL, X, Y) MultiplyMat4(MODEL, ScaleMat4(V3(X, Y, 1)))

typedef union mat4 {
  float Elements[4][4];
#if USE_SSE
  __m128 Rows[4];
#endif
} mat4;

#define ClampMax(Value, MaxValue) (Value > MaxValue) ? (MaxValue) : (Value)
#define Clamp(Value, MinValue, MaxValue) (Value > MaxValue) ? (MaxValue) : ((Value < MinValue) ? (MinValue) : (Value))

extern mat4 Mat4D(float Diagonal);
extern float InnerV2(v2 A, v2 B);
extern v2 DifferenceV2(v2 A, v2 B);
extern v3 DifferenceV3(v3 A, v3 B);
extern float Powf(float A, float B);
extern float SquareRoot(float A);
extern float DistanceV2(v2 A, v2 B);
extern float DistanceV3(v3 A, v3 B);
extern float ToRadians(float Degrees);
extern float Lerp(float V0, float V1, float T);
extern v2 LerpV2(v2 V0, v2 V1, v2 T);
extern v2 LerpV2t(v2 V0, v2 V1, float T);
extern v3 LerpV3t(v3 V0, v3 V1, float T);
extern mat4 Translate(v3 T);
extern mat4 TranslateMat4(mat4 A, v3 T);
extern v3 MultiplyMatrixVector(mat4 M, v3 A);
extern v3 MultiplyMatrixVectorW(mat4 M, v3 A, float* W);
extern float DotVec3(v3 A, v3 B);
extern float LengthSquareVec3(v3 A);
extern float LengthVec3(v3 A);
extern v3 NormalizeVec3(v3 A);
extern v3 Cross(v3 A, v3 B);

extern v3 MultiplyV3(v3 A, float Value);
extern v2 MultiplyV2(v2 A, float Value);
extern v3 MultiplyToV3(v3 A, v3 B);
extern v2 MultiplyToV2(v2 A, v2 B);
extern v2 AddV2(v2 A, float Value);
extern v3 AddV3(v3 A, float Value);
extern v2 DivideV2(v2 A, float Value);
extern v3 DivideV3(v3 A, float Value);
extern v2 AddToV2(v2 A, v2 B);
extern v3 AddToV3(v3 A, v3 B);
extern v3 SubToV3(v3 A, v3 B);
extern mat4 Mat4D(float Diagonal);
extern mat4 MultiplyMat4(mat4 A, mat4 B);
extern mat4 MultiplyMat4f(mat4 A, float B);
extern mat4 Rotate(float Angle, v3 Axis);
extern mat4 ScaleMat4(v3 A);
extern mat4 ScaleOnMat4(mat4 A, v3 B);
extern mat4 Perspective(float Fov, float AspectRatio, float ZNear, float ZFar);
extern mat4 Orthographic(float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
extern mat4 InverseMat4(mat4 A);

extern float Sin(float Value);

extern i32 Inside(v2 P, v2 Start, v2 End);

extern void PrintMat4(FILE* File, mat4 A);
extern void PrintV3(FILE* File, v3 A);

#if USE_SSE
extern mat4 Transpose(mat4 A);
extern __m128 LinearCombineSSE(__m128 Left, mat4 Right);
#endif

#endif
