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
  struct {
    float W, H;
  };
  struct {
    float L, R;
  };
} v2;

typedef union v3 {
  struct {
    float X, Y, Z;
  };
  struct {
    float R, G, B;
  };
} v3;

typedef union v4 {
  struct {
    float X, Y, Z, W;
  };
} v4;

typedef struct string {
  char* Data;
  u32 Count;
} string;

typedef string buffer;

#define V2(_X, _Y) ((v2) { .X = _X, .Y = _Y })
#define V3(_X, _Y, _Z) ((v3) { .X = _X, .Y = _Y, .Z = _Z })
#define V4(_X, _Y, _Z, _W) ((v4) { .X = _X, .Y = _Y, .Z = _Z, .W = _W })

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512
#define PI32 3.14159265359f
#define ArraySize(Arr) ((sizeof(Arr)) / (sizeof(Arr[0])))
#define MAX_PATH_SIZE 512

#define Assert(VALUE) assert(VALUE)
#define Clamp(Value, MinValue, MaxValue) (Value > MaxValue) ? (MaxValue) : ((Value < MinValue) ? (MinValue) : (Value))
#define Min(A, B) (A < B ? A : B)
#define DB_MIN (-100.0f)  // Somewhat arbitrary
#define Log10(Value) (Value <= 0 ? DB_MIN : log10(Value))

#if 1
#define Abs(Value) (Value < 0 ? -Value : Value)
#else
#define Abs(Value) abs(Value)
#endif

typedef enum error_code {
  NoError = 0,
  Error = -1,
} error_code;

char* FetchExtension(const char* Path);

i32 ReadFile(const char* Path, buffer* Buffer);

i32 ReadFileAndNullTerminate(const char* Path, buffer* Buffer);
