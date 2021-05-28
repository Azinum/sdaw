// debug.h

#define DEBUG_TIMER 1

#if DEBUG_TIMER

#include <time.h>

#define MAX_DEBUG_EVENT 128

typedef enum debug_value_type {
  DebugTypeInteger32,
  DebugTypeFloat32,
  DebugTypeFloat64,

  MaxDebugValueType,
} debug_value_type;

typedef struct debug_event_info {
  const char* Name;
  debug_value_type Type;  // Unused for now
  union {
    i32 Integer;
    r32 Float;
    r64 Double;
  };
} debug_event_info;

static debug_event_info DebugEventTable[MAX_DEBUG_EVENT] = {0};

static i32 DebugNumEvents = 0;

// TODO(lucas): Add fixed ids for storing multiple debug events
#define DebugRecordEvent(NAME, VALUE, EVENT_ID) { \
  Assert(EVENT_ID >= 0 && EVENT_ID < MAX_DEBUG_EVENT);\
  debug_event_info* _DebugEventInfo = &DebugEventTable[EVENT_ID]; \
  if (!_DebugEventInfo->Name) { DebugNumEvents++; } \
  _DebugEventInfo->Name = NAME; \
  _DebugEventInfo->Double = VALUE; \
}

#define TIMER_START(...) \
  clock_t _TimeEnd = 0; \
  clock_t _TimeStart = 0; \
  _TimeStart = clock(); \
  Assert(_TimeStart != (clock_t)(-1)); \
  __VA_ARGS__

#define TIMER_END(...) { \
  _TimeEnd = clock(); \
  r64 _DeltaTime = ((r64) (_TimeEnd - _TimeStart)) / CLOCKS_PER_SEC; \
  char* _Name = (char*)__FUNCTION__; \
  DebugRecordEvent(_Name, _DeltaTime, 0); \
  __VA_ARGS__ \
}

#else

#define TIMER_START(...)

#define TIMER_END(...)

#endif

