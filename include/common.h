// common.h

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

typedef int64_t i64;
typedef uint64_t u64;
typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;
typedef float r32;
typedef double r64;

typedef union v2 {
  struct {
    float X, Y;
  };
  struct {
    float U, V;
  };
} v2;

#define V2(_X, _Y) ((v2) {.X = _X, .Y = _Y})

#define PI32 3.14159265359f
#define ArraySize(Arr) ((sizeof(Arr)) / (sizeof(Arr[0])))
#define MAX_PATH_SIZE 512
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

#define Assert(VALUE) assert(VALUE)
#define Clamp(Value, MinValue, MaxValue) (Value > MaxValue) ? (MaxValue) : ((Value < MinValue) ? (MinValue) : (Value))
#define Min(A, B) (A < B ? A : B)
#define Log10(Value) (Value <= 0 ? -200.0f : log10(Value))

#if 1
#define Abs(Value) (Value < 0 ? -Value : Value)
#else
#define Abs(Value) abs(Value)
#endif

typedef enum error_code {
  NoError = 0,
  Error = -1,
} error_code;

extern float Lerp(float V0, float V1, float T);

extern float Pow(float A, float B);

extern float SquareRoot(float A);

extern float DistanceV2(v2 A, v2 B);

extern v2 DiffV2(v2 A, v2 B);

extern v2 AddV2(v2 A, v2 B);

extern v2 DivV2(v2 A, v2 B);

extern i32 Inside(v2 P, v2 Start, v2 End);
