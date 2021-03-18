// common.h

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

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

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))
#define MAX_PATH_SIZE 512

#define Assert(VALUE) assert(VALUE)
#define Clamp(Value, MinValue, MaxValue) (Value > MaxValue) ? (MaxValue) : ((Value < MinValue) ? (MinValue) : (Value))

typedef enum error_code {
  NoError = 0,
  Error = -1,
} error_code;

extern float Lerp(float V0, float V1, float T);
