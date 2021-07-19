// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

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

typedef struct buffer {
  char* Data;
  u32 Count;
} buffer;

typedef struct string {
  char* Data;
  u32 Count;
  u32 Size;
} string;

#define V2(_X, _Y) ((v2) { .X = _X, .Y = _Y })
#define V3(_X, _Y, _Z) ((v3) { .X = _X, .Y = _Y, .Z = _Z })
#define V4(_X, _Y, _Z, _W) ((v4) { .X = _X, .Y = _Y, .Z = _Z, .W = _W })

#define PI32 3.14159265359f
#define MAX_PATH_SIZE 512
#define ArraySize(Arr) ((sizeof(Arr)) / (sizeof(Arr[0])))

#define Assert(VALUE) assert(VALUE)
#define Clamp(Value, MinValue, MaxValue) (Value > MaxValue) ? (MaxValue) : ((Value < MinValue) ? (MinValue) : (Value))
#define Min(A, B) (A < B ? A : B)
#define DB_MIN (-100.0f)  // Somewhat arbitrary
#define Log10(Value) (Value <= 0 ? DB_MIN : log10(Value))
#define MouseOver(M_X, M_Y, X, Y, W, H) (M_X >= X && M_X <= X + W && M_Y >= Y && M_Y <= Y + H)

#if 1
#define Abs(Value) (Value < 0 ? -Value : Value)
#else
#define Abs(Value) abs(Value)
#endif

#define LowNibble(Byte) (u8)(Byte & 0x0f)
#define HighNibble(Byte) (u8)((Byte & 0xf0) >> 4)

#define CALL(Callback, ...) (Callback ? (Callback(__VA_ARGS__)) : (void)0)

#define MAX_BUFFER_SIZE 512

typedef enum error_code {
  NoError = 0,
  Error = -1,
} error_code;

extern char DataPath[];

char* FetchExtension(const char* Path);

char* HomePath();

char* FromDataPath(char* FileName);

const char* GetDataPath();

const char* DataPathConcat(const char* Path);

i32 ReadFile(const char* Path, buffer* Buffer);

i32 ReadFileFromDataPath(const char* Path, buffer* Buffer);

i32 ReadFileAndNullTerminate(const char* Path, buffer* Buffer);

float RandomFloat(float From, float To);

u64 Hash(char* String);

u64 HashString(char* String, u32 Length);

u32 RandomSeed();

#endif
