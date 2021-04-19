// debug.h

#define DEBUG_TIMER 1

#if DEBUG_TIMER

typedef enum debug_event_type {
  MAX_DEBUG_EVENT = 128,
} debug_event_type;

typedef struct debug_event_info {
  const char* Name;
  union {
    float Value;
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
  _DebugEventInfo->Value = VALUE; \
}

#define TIMER_START(...) \
  struct timeval _TimeNow = {0}; \
  struct timeval _TimeStart = {0}; \
  gettimeofday(&_TimeStart, NULL); \
  __VA_ARGS__

#define TIMER_END(...) { \
  gettimeofday(&_TimeNow, NULL); \
  char* _Name = (char*)__FUNCTION__; \
  float _DeltaTime = (((((_TimeNow.tv_sec - _TimeStart.tv_sec) * 1000000.0f) + _TimeNow.tv_usec) - (_TimeStart.tv_usec)) / 1000000.0f); \
  DebugRecordEvent(_Name, _DeltaTime, 0); \
  __VA_ARGS__; \
}

#else

#define TIMER_START(...)

#define TIMER_END(...)

#endif

